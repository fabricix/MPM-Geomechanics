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
	#ifdef USE_PARALLEL_INTERPOLATION
		std::cout << "[ INFO ] USE_PARALLEL_INTERPOLATION is defined" << std::endl;
	#else
		std::cout << "[ INFO ] USE_PARALLEL_INTERPOLATION is NOT defined" << std::endl;
	#endif

	#ifdef _OPENMP
		std::cout << "[ INFO ] _OPENMP is defined" << std::endl;
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
	std::random_device rd;
	std::mt19937 gen(rd());
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
