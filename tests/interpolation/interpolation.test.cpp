#include <gtest/gtest.h>

#include <iostream>
using std::cout;
using std::endl;

#include <vector>
using std::vector;

#include <fstream>
using std::ofstream;

#include <string>
using std::to_string;

#include "BodyCuboid.h"
#include "Interpolation.h"
#include "MPM.h"
#include "Material.h"
#include "Output.h"
#include "Particle.h"
#include "ShapeGimp.h"

using namespace std;

TEST(Interpolation, Interpolation_1) {
  // create the mesh
  Mesh mesh;
  mesh.setNumCells(10, 10, 10);
  mesh.setCellDimension(1, 1, 1);
  mesh.createGrid(false);

  // create particles
  std::vector<Particle *> particles;
  particles.push_back(new Particle(Vector3d(0.0, 0.0, 0.0), NULL, Vector3d(1.0, 1.0, 1.0)));

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
  Interpolation::nodalMassWithParticles(&mesh, &particles);

  // get grid nodes
  vector<Node *> *gridNodes = mesh.getNodes();

  float sum = 0;

  // write the nodal mass
  for (size_t i = 0; i < particles.size(); i++) {
    // contribution vector
    vector<Contribution> *contribution = particles.at(i)->getContributionNodes();

    // nodal mass
    for (size_t j = 0; j < contribution->size(); ++j) {
      sum = sum + gridNodes->at(contribution->at(j).getNodeId())->getMass();
    }
  }

  EXPECT_EQ(sum, 1.0);
};