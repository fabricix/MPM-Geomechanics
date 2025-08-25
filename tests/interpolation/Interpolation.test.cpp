#include <gtest/gtest.h>

#include <random>
#include <iostream>
#include <vector>

#include "BodyCuboid.h"
#include "Interpolation.h"
#include "MPM.h"
#include "Material.h"
#include "Output.h"
#include "Particle.h"
#include "ShapeGimp.h"

using namespace std;

float interpolation(vector<Particle *> particles, Mesh mesh) {
  // configures particle
  for (size_t i = 0; i < particles.size(); i++) {
    // id
    particles.at(i)->setId(i);

    // mass
    particles.at(i)->setMass(1.0);

    // shape
    particles.at(i)->setShape(new ShapeGimp);
  }
  // update contributions nodes
  for (size_t i = 0; i < particles.size(); i++) {
    particles.at(i)->updateContributionNodes(&mesh);
  }

  // interpolate particle mass to nodes
  Interpolation::nodalMass(&mesh, &particles);

  // get grid nodes
  vector<Node *> *gridNodes = mesh.getNodes();

  float sum = 0;

  // nodal mass
  for (size_t j = 0; j < gridNodes->size(); ++j) {
    sum = sum + gridNodes->at(j)->getMass();
  }
  return sum;
};

TEST(Interpolation, Particles_1) {
  // create the mesh
  Mesh mesh;
  mesh.setNumCells(10, 10, 10);
  mesh.setCellDimension(1, 1, 1);
  mesh.createGrid(false);

  // create particles
  std::vector<Particle *> particles;
  particles.push_back(new Particle(Vector3d(0.0, 0.0, 0.0), NULL, Vector3d(1.0, 1.0, 1.0)));

  float mass = interpolation(particles, mesh);
  EXPECT_EQ(mass, 1.0);
};

TEST(Interpolation, Particles_3) {
  // create the mesh
  Mesh mesh;
  mesh.setNumCells(10, 10, 10);
  mesh.setCellDimension(1, 1, 1);
  mesh.createGrid(false);

  // create particles
  vector<Particle *> particles;
  particles.push_back(new Particle(Vector3d(0.0, 0.0, 0.0), NULL, Vector3d(1.0, 1.0, 1.0)));
  particles.push_back(new Particle(Vector3d(0.0, 0.0, 0.0), NULL, Vector3d(1.0, 1.0, 1.0)));
  particles.push_back(new Particle(Vector3d(0.0, 0.0, 0.0), NULL, Vector3d(1.0, 1.0, 1.0)));

  float mass = interpolation(particles, mesh);
  EXPECT_EQ(mass, 3.0);
};


TEST(Interpolation, Particles_10_position_rng) {
  // create the mesh
  Mesh mesh;
  mesh.setNumCells(10, 10, 10);
  mesh.setCellDimension(1, 1, 1);
  mesh.createGrid(false);

  // create particles
  vector<Particle *> particles;

  // random number generator
  std::random_device rd;
  std::mt19937 gen(rd()); 
  std::uniform_int_distribution<int> dist(0, 9);

  // create 10 particles with random positions
  for (int i = 0; i < 10; i++) {
    particles.push_back(new Particle(Vector3d(dist(gen), dist(gen), dist(gen)), NULL, Vector3d(1.0, 1.0, 1.0)));
  }

  float mass = interpolation(particles, mesh);
  EXPECT_EQ(mass, 10.0);
};