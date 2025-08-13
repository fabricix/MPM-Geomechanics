#include <gtest/gtest.h>
#include <chrono>
#include <random>
#include <vector>
#include <iostream>

#include "Mesh.h"
#include "Particle.h"
#include "ShapeGimp.h"
#include "Update.h"
#include "ConfigParallel.h"

using namespace std;

// Global configuration variables
const int numParticles = 150000;
const int numThreads = 10;
const Vector3d particleSize(1.0, 1.0, 1.0);
const double particleMass = 1.0;
const Vector3d cellDimension(1.0, 1.0, 1.0);
const Vector3i numCells(50, 50, 50);
const int ramdomSeed = 42;

TEST(UpdatePerformance, ContributionNodes_nParticles)
{
#if defined(USE_PARALLEL_CONTRIBUTION)
	std::cout << "[ INFO ] USE_PARALLEL_CONTRIBUTION is defined" << std::endl;
#else
	std::cout << "[ INFO ] USE_PARALLEL_CONTRIBUTION is NOT defined" << std::endl;
#endif

#ifdef _OPENMP
		std::cout << "[ INFO ] _OPENMP is defined" << std::endl;
		omp_set_num_threads(numThreads);  // Set the number of threads for the test
		std::cout << "[ INFO ] OpenMP threads: " << omp_get_max_threads() << std::endl;
#else
		std::cout << "[ INFO ] _OPENMP is NOT defined" << std::endl;
#endif

	// Number of particles to test
	std::cout << "[ INFO ] Total particles: " << numParticles << std::endl;

	// create the mesh
	Mesh mesh;
	mesh.setNumCells(numCells(0), numCells(1),numCells(2));
	mesh.setCellDimension(cellDimension(0), cellDimension(1), cellDimension(2));
	mesh.createGrid();

	// create particles
	vector<Particle*> particles;
    particles.reserve(numParticles);

	std::mt19937 gen(ramdomSeed);
	std::uniform_real_distribution<double> dist(0.0, cellDimension.maxCoeff());

	for (int i = 0; i < numParticles; ++i) {
		Particle* p = new Particle(Vector3d(dist(gen), dist(gen), dist(gen)), NULL, particleSize);
		p->setShape(new ShapeGimp());
		p->setActive(true);
		particles.push_back(p);
	}

	// Measure performance of contributionNodes
	auto start = std::chrono::high_resolution_clock::now();
	Update::contributionNodes(&mesh, &particles);
	auto end = std::chrono::high_resolution_clock::now();

	auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	std::cout << "[ PERF ] contributionNodes took " << duration_ms << " ms" << std::endl;

	// Clean up
	for (auto p : particles)
		delete p;
}

TEST(UpdatePerformance, UpdatePerformance_ParticleDensity_nParticles)
{
    #if defined(USE_PARALLEL_DENSITY)
        std::cout << "[ INFO ] USE_PARALLEL_DENSITY is defined" << std::endl;
    #else
        std::cout << "[ INFO ] USE_PARALLEL_DENSITY is NOT defined" << std::endl;
    #endif

    #ifdef _OPENMP
        std::cout << "[ INFO ] _OPENMP is defined" << std::endl;
        omp_set_num_threads(numThreads);
        std::cout << "[ INFO ] OpenMP threads: " << omp_get_max_threads() << std::endl;
    #endif

    // Test parameters
    const double initialDensity = 2000.0;
    const double traceStrain = 0.05;  // Volumetric strain increment
    const double expectedDensity = initialDensity / (1.0 + traceStrain);

    // Create particles
    std::vector<Particle*> particles;
    particles.reserve(numParticles);

    for (int i = 0; i < numParticles; ++i) {
        Particle* p = new Particle(Vector3d(0.0, 0.0, 0.0), nullptr, particleSize);
        p->setId(i);
        p->setMass(particleMass);
        p->setShape(new ShapeGimp);
        p->setActive(true);
        p->setDensity(initialDensity);

        // Set a strain increment with known volumetric part
        Matrix3d strain = Matrix3d::Zero();
        strain(0,0) = strain(1,1) = strain(2,2) = traceStrain / 3.0;
        p->setStrainIncrement(strain);
        particles.push_back(p);
    }

    // Measure performance
    auto start = std::chrono::high_resolution_clock::now();
    Update::particleDensity(&particles);
    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "[ PERF ] particleDensity took " << duration.count() << " ms" << std::endl;

    // Verify result
    for (auto p : particles)
        EXPECT_NEAR(p->getDensity(), expectedDensity, 1e-5);

    // Cleanup
    for (auto p : particles) delete p;
}