#include <gtest/gtest.h>
#include <chrono>
#include <random>
#include <vector>

#include "Mesh.h"
#include "Particle.h"
#include "ShapeGimp.h"
#include "Interpolation.h"
#include "ConfigParallel.h"

using namespace std;

TEST(InterpolationPerformance, NodalMass_nParticles)
{
#ifdef USE_PARALLEL_MASS
		std::cout << "[ INFO ] USE_PARALLEL_MASS is defined" << std::endl;
#else
		std::cout << "[ INFO ] USE_PARALLEL_MASS is NOT defined" << std::endl;
#endif

#ifdef _OPENMP
		std::cout << "[ INFO ] _OPENMP is defined" << std::endl;
		omp_set_num_threads(10);  // Set the number of threads for the test
		std::cout << "[ INFO ] OpenMP threads: " << omp_get_max_threads() << std::endl;
#else
		std::cout << "[ INFO ] _OPENMP is NOT defined" << std::endl;
#endif

	// create the mesh
	Mesh mesh;
	mesh.setNumCells(50, 50, 50);
	mesh.setCellDimension(1.0, 1.0, 1.0);
	mesh.createGrid(false);

	// create particles
	vector<Particle*> particles;
	//std::random_device rd;
	std::mt19937 gen(42);
	std::uniform_real_distribution<double> dist(0.0, 49.0);

	int numParticles = 150000; // number of particles to create
	double particleSize = 1.0; // size of each particle
	double particleMass = 1.0; // mass of each particle

	std::cout << "[ INFO ] Total particles: " << numParticles << std::endl;

	for (int i = 0; i < numParticles; ++i) {
		Particle* p = new Particle(Vector3d(dist(gen), dist(gen), dist(gen)), NULL, Vector3d(particleSize, particleSize, particleSize));
		p->setId(i);
		p->setMass(particleMass);
		p->setShape(new ShapeGimp);
		p->updateContributionNodes(&mesh);
		particles.push_back(p);
	}

	// measure execution time of nodalMass
	auto t0 = std::chrono::high_resolution_clock::now();
	Interpolation::nodalMass(&mesh, &particles);
	auto t1 = std::chrono::high_resolution_clock::now();

	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
	std::cout << "[ PERF ] nodalMass took " << duration << " ms" << std::endl;

	// optional: check total nodal mass
	double totalMass = 0.0;
	vector<Node*>* nodes = mesh.getNodes();
	for (size_t j = 0; j < nodes->size(); ++j) {
		totalMass += nodes->at(j)->getMass();
	}
	EXPECT_NEAR(totalMass, numParticles*particleMass, 1e-8);

	// free memory
	for (auto* p : particles)
		delete p;
}

TEST(InterpolationPerformance, NodalMomentum_nParticles)
{
#ifdef _OPENMP
	std::cout << "[ INFO ] _OPENMP is defined" << std::endl;
    omp_set_num_threads(10);  // Set the number of threads for the test
    std::cout << "[ INFO ] OpenMP threads: " << omp_get_max_threads() << std::endl;
#endif

#if defined(USE_PARALLEL_MOMENTUM)
    std::cout << "[ INFO ] USE_PARALLEL_MOMENTUM is defined" << std::endl;
#else
    std::cout << "[ INFO ] USE_PARALLEL_MOMENTUM is NOT defined" << std::endl;
#endif

    const int numParticles = 150000;
    std::cout << "[ INFO ] Total particles: " << numParticles << std::endl;

	double particleSize = 1.0; // size of each particle
	double particleMass = 1.0; // mass of each particle
	Vector3d particleVelocity (1.0, 2.0, 3.0);
    // Create the mesh
    Mesh mesh;
    mesh.setNumCells(50, 50, 50);
    mesh.setCellDimension(1, 1, 1);
    mesh.createGrid(false);

    // Create particles with random positions
    std::vector<Particle*> particles;
    std::mt19937 gen(42);
    std::uniform_real_distribution<double> dist(0.0, 49.9);

    for (int i = 0; i < numParticles; i++) {
        Vector3d pos(dist(gen), dist(gen), dist(gen));
        Particle* p = new Particle(pos, nullptr, Vector3d(particleSize, particleSize, particleSize));
        p->setId(i);
        p->setMass(particleMass);
        p->setVelocity(particleVelocity);
        p->setShape(new ShapeGimp);
        particles.push_back(p);
    }

    // Update contribution nodes
    for (auto& p : particles)
        p->updateContributionNodes(&mesh);

    // Start timing
    auto t1 = std::chrono::high_resolution_clock::now();

    // Interpolate momentum
    Interpolation::nodalMomentum(&mesh, &particles);

    auto t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = t2 - t1;

    std::cout << "[ PERF ] nodalMomentum took " << elapsed.count() << " ms" << std::endl;

    // Optional: sum total nodal momentum to verify correctness
    Vector3d totalMomentum = Vector3d::Zero();
    for (Node* n : *mesh.getNodes())
        totalMomentum += n->getMomentum();

    EXPECT_NEAR(totalMomentum(0), particleVelocity(0) * numParticles, 1e-5);
    EXPECT_NEAR(totalMomentum(1), particleVelocity(1) * numParticles, 1e-5);
    EXPECT_NEAR(totalMomentum(2), particleVelocity(2) * numParticles, 1e-5);

    // Clean up
    for (auto& p : particles)
        delete p;
}

using namespace std;

TEST(InterpolationPerformance, NodalInternalForce_nParticles)
{
#if defined(USE_PARALLEL_INTERNAL_FORCE)
    std::cout << "[ INFO ] USE_PARALLEL_INTERNAL_FORCE is defined" << std::endl;
#else
    std::cout << "[ INFO ] USE_PARALLEL_INTERNAL_FORCE is NOT defined" << std::endl;
#endif

#ifdef _OPENMP
	std::cout << "[ INFO ] _OPENMP is defined" << std::endl;
    omp_set_num_threads(10);  // Set the number of threads for the test
    std::cout << "[ INFO ] OpenMP threads: " << omp_get_max_threads() << std::endl;
#endif

    // Number of particles
    const int numParticles = 150000;
    std::cout << "[ INFO ] Total particles: " << numParticles << std::endl;

    // Create mesh
    Mesh mesh;
    mesh.setNumCells(50, 50, 50);
    mesh.setCellDimension(1.0, 1.0, 1.0);
    mesh.createGrid(false);

    // Create particles with uniform properties
    std::random_device rd;
    std::mt19937 gen(42);
    std::uniform_real_distribution<double> dist(0.0, 49.9);

    std::vector<Particle*> particles;
    particles.reserve(numParticles);

    for (int i = 0; i < numParticles; ++i) {
        Vector3d pos(dist(gen), dist(gen), dist(gen));
        Vector3d size(1.0, 1.0, 1.0);
        Particle* p = new Particle(pos, nullptr, size);
        p->setShape(new ShapeGimp());
        p->setActive(true);
        p->setStress(Matrix3d::Identity());
        p->setPorePressure(0.0);
        particles.push_back(p);
    }

    // Update contributions
    for (auto& p : particles) {
        p->updateContributionNodes(&mesh);
    }

    // Measure performance of nodalInternalForce
    auto start = std::chrono::high_resolution_clock::now();
    Interpolation::nodalInternalForce(&mesh, &particles);
    auto end = std::chrono::high_resolution_clock::now();

    auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "[ PERF ] nodalInternalForce took " << duration_ms << " ms" << std::endl;

    // Clean up
    for (auto p : particles)
        delete p;
}

TEST(InterpolationPerformance, InterpolationPerformance_NodalExternalForce_nParticles)
{
    const int numParticles = 15000;
    const Vector3d appliedForce(1.0, 2.0, 3.0);

    #if defined(USE_PARALLEL_EXTERNAL_FORCE)
    std::cout << "[ INFO ] USE_PARALLEL_EXTERNAL_FORCE is defined\n";
    #else
    std::cout << "[ INFO ] USE_PARALLEL_EXTERNAL_FORCE is NOT defined\n";
    #endif

    #ifdef _OPENMP
	std::cout << "[ INFO ] _OPENMP is defined" << std::endl;
    omp_set_num_threads(10);  // Set the number of threads for the test
    std::cout << "[ INFO ] OpenMP threads: " << omp_get_max_threads() << std::endl;
    #endif

    // Create mesh
    Mesh mesh;
    mesh.setNumCells(50, 50, 50);
    mesh.setCellDimension(1.0, 1.0, 1.0);
    mesh.createGrid(false);

    // Create particles
    std::mt19937 gen(42);
    std::uniform_real_distribution<double> dist(0.0, 49.9);
    std::vector<Particle*> particles;
    particles.reserve(numParticles);

    for (int i = 0; i < numParticles; ++i)
    {
        Vector3d pos(dist(gen), dist(gen), dist(gen));
        Particle* p = new Particle(pos, nullptr, Vector3d(1.0, 1.0, 1.0));
        p->setId(i);
        p->setMass(1.0);
        p->setShape(new ShapeGimp);
        p->addExternalForce(appliedForce);
        particles.push_back(p);
    }

    // Update contribution nodes
    for (auto& p : particles)
        p->updateContributionNodes(&mesh);

    // Interpolate external force
    auto start = std::chrono::high_resolution_clock::now();
    Interpolation::nodalExternalForce(&mesh, &particles);
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> duration = end - start;
    std::cout << "[ PERF ] nodalExternalForce took " << duration.count() << " ms\n";

    // Verify result
    Vector3d totalForce = Vector3d::Zero();
    for (const auto& node : *mesh.getNodes())
        totalForce += node->getExternalForce();

    Vector3d expectedForce = appliedForce * static_cast<double>(numParticles);
    EXPECT_NEAR(totalForce(0), expectedForce(0), 1e-5);
    EXPECT_NEAR(totalForce(1), expectedForce(1), 1e-5);
    EXPECT_NEAR(totalForce(2), expectedForce(2), 1e-5);

    // Clean up
    for (auto& p : particles)
        delete p;
}