#include <gtest/gtest.h>
#include <chrono>
#include <random>
#include <vector>
#include <iostream>

#include "Mesh.h"
#include "Particle.h"
#include "ShapeGimp.h"
#include "Update.h"
#include "Materials/Elastic.h"
#include "Body/BodyParticle.h"

using namespace std;

// Global configuration variables
const int numParticles = 500000;
const int numThreads = 10;
const Vector3d particleSize(1.0, 1.0, 1.0);
const double particleMass = 1.0;
const Vector3d cellDimension(1.0, 1.0, 1.0);
const Vector3i numCells(100, 100, 100);
const int ramdomSeed = 42;

TEST(UpdatePerformance, ContributionNodes_nParticles)
{
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

    vector<Body*> bodies;
    Body* body = new BodyParticle();
    body->setParticles(particles);
    bodies.push_back(body);


	// Measure performance of contributionNodes
	auto start = std::chrono::high_resolution_clock::now();
	Update::contributionNodes(&mesh, &bodies);
	auto end = std::chrono::high_resolution_clock::now();

	auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	std::cout << "[ PERF ] contributionNodes took " << duration_ms << " ms" << std::endl;

	// Clean up
	for (auto p : particles)
		delete p;
}

TEST(UpdatePerformance, UpdatePerformance_ParticleDensity_nParticles)
{
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

    vector<Body*> bodies;
    Body* body = new BodyParticle();
    body->setParticles(particles);
    bodies.push_back(body);

    // Measure performance
    auto start = std::chrono::high_resolution_clock::now();
    Update::particleDensity(&bodies);
    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "[ PERF ] particleDensity took " << duration.count() << " ms" << std::endl;

    // Verify result
    for (auto p : particles)
        EXPECT_NEAR(p->getDensity(), expectedDensity, 1e-5);

    // Cleanup
    for (auto p : particles) delete p;
}

TEST(UpdatePerformance, ParticleStress_nParticles)
{
#ifdef _OPENMP
    std::cout << "[ INFO ] _OPENMP is defined\n";
    omp_set_num_threads(numThreads);
    std::cout << "[ INFO ] OpenMP threads: " << omp_get_max_threads() << '\n';
#endif

    // Material parameters
    const double E = 1000.0;   // Young's modulus
    const double nu = 0.3;     // Poisson's ratio
	const double epsilon0 = 0.01; // Initial strain increment

    // Create particles
    vector<Particle*> particles;
    particles.reserve(numParticles);
    for (int i = 0; i < numParticles; ++i) {
        Vector3d position(0.0, 0.0, 0.0);
        Particle* p = new Particle(position, nullptr, particleSize);
        p->setId(i);
        p->setMass(particleMass);
        p->setShape(new ShapeGimp);
        p->setActive(true);
        p->setStress(Matrix3d::Zero());
        p->setStrainIncrement(Matrix3d::Zero());

        // Apply a uniaxial strain increment in the x-direction
        Matrix3d dstrain = Matrix3d::Zero();
        dstrain(0, 0) = epsilon0;
        p->setStrainIncrement(dstrain);

        // Assign linear elastic material
        Material* mat = new Elastic(1, 1, E, nu);
        p->setMaterial(mat);

        particles.push_back(p);
    }

    vector<Body*> bodies;
    Body* body = new BodyParticle();
    body->setParticles(particles);
    bodies.push_back(body);

    // Measure performance
    auto start = chrono::high_resolution_clock::now();
    Update::particleStress(&bodies);
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    std::cout << "[ PERF ] particleStress took " << duration.count() << " ms\n";

	/*
	Analytical verification (uniaxial strain test):

	We apply a uniaxial strain state:
		strain_xx = strain0
		strain_yy = strain_zz = 0

	The expected stress components for an isotropic linear elastic material are:

		stress_xx = (lambda + 2G) * strain0
		stress_yy = stress_zz = lambda * strain0
		stress_xy = stress_yz = stress_xz = 0

	where:
		lambda = (E * v) / [(1 + v)(1 - 2v)]
		G = E / [2(1 + v)]

	These values are used to compare against the computed stress in each particle.
	*/


	double lambda = (E * nu) / ((1 + nu) * (1 - 2 * nu));
	double G = E / (2 * (1 + nu));

	double expected_sigma_xx = (lambda + 2.0 * G) * epsilon0;
	double expected_sigma_yy = lambda * epsilon0;
	double expected_sigma_zz = lambda * epsilon0;

    // Check stress tensor for each particle
    for (auto& p : particles) {
        Matrix3d stress = p->getStress();

        // Diagonal terms
        EXPECT_NEAR(stress(0, 0), expected_sigma_xx, 1e-5);
        EXPECT_NEAR(stress(1, 1), expected_sigma_yy, 1e-5);
        EXPECT_NEAR(stress(2, 2), expected_sigma_zz, 1e-5);

        // Off-diagonal terms must be zero
        EXPECT_NEAR(stress(0, 1), 0.0, 1e-5);
        EXPECT_NEAR(stress(1, 0), 0.0, 1e-5);
        EXPECT_NEAR(stress(0, 2), 0.0, 1e-5);
        EXPECT_NEAR(stress(2, 0), 0.0, 1e-5);
        EXPECT_NEAR(stress(1, 2), 0.0, 1e-5);
        EXPECT_NEAR(stress(2, 1), 0.0, 1e-5);
    }

    // Clean up
    for (auto p : particles) {
        delete p;
    }
}
