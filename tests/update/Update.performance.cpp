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

TEST(UpdatePerformance, ContributionNodes_nParticles)
{
#if defined(USE_PARALLEL_CONTRIBUTION)
	std::cout << "[ INFO ] USE_PARALLEL_CONTRIBUTION is defined" << std::endl;
#else
	std::cout << "[ INFO ] USE_PARALLEL_CONTRIBUTION is NOT defined" << std::endl;
#endif

#ifdef _OPENMP
		std::cout << "[ INFO ] _OPENMP is defined" << std::endl;
		omp_set_num_threads(10);  // Set the number of threads for the test
		std::cout << "[ INFO ] OpenMP threads: " << omp_get_max_threads() << std::endl;
#else
		std::cout << "[ INFO ] _OPENMP is NOT defined" << std::endl;
#endif

	// Number of particles to test
	const int numParticles = 150000;
	std::cout << "[ INFO ] Total particles: " << numParticles << std::endl;
	double particleSize = 1.0; // size of each particle

	// Create mesh
	Mesh mesh;
	mesh.setNumCells(50, 50, 50);
	mesh.setCellDimension(1.0, 1.0, 1.0);
	mesh.createGrid(false);

	// Create particles with random positions
	//std::random_device rd;
	std::mt19937 gen(42);
	std::uniform_real_distribution<double> dist(0.0, 49.9);

	std::vector<Particle*> particles;
	particles.reserve(numParticles);
	for (int i = 0; i < numParticles; ++i) {
		Vector3d pos(dist(gen), dist(gen), dist(gen));
		Vector3d size(particleSize, particleSize, particleSize);
		Particle* p = new Particle(pos, nullptr, size);
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
