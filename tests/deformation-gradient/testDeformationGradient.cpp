/*
 * testDeformationGradient.cpp
 *
 *  Created on: 12 de jan de 2024
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include <iostream>
using std::cout;
using std::endl;

#include <vector>
using std::vector;

#include <fstream>
using std::ofstream;

#include <string>
using std::string;
using std::to_string;

#include <iomanip>
using std::fixed;
using std::scientific;
using std::setw;


#include "MPM.h"
#include "Model.h"
#include "Particle/Particle.h"
#include "Materials/Elastic.h"
#include "Shape/ShapeGimp.h"
#include "Shape/ShapeLinear.h"
#include "Interpolation.h"
#include "Body/BodyParticle.h"

int main(int argc, char **argv)
{
	// create a mesh
	Mesh mesh;
	mesh.setNumCells(1, 1, 1);
	mesh.setCellDimension(1, 1, 1);
	mesh.setNumGhosts(2);
	mesh.createGrid(false);
		
	// set time step 
	ModelSetup::setTimeStep(0.124);
	 
	// create an elastic material
	int id = 0;
	double density = 0.0, young = 0.0, poisson = 0.0;
	Elastic* mat_elastic = new Elastic(id=1, density=1750, young=5066, poisson=0.25);

	// create a particle vector
	std::vector<Particle*> particles;

	// add a particle
	Vector3d size(1, 1, 1), position(0.5,0.5,0.5);
	particles.push_back(new Particle(position, mat_elastic, size));

	// configures particles
	for (size_t i = 0; i < particles.size(); i++) {

		// shape
		particles.at(i)->setShape(new ShapeGimp);
	}

	// update contributions nodes
	for (size_t i=0; i<particles.size(); i++){

		particles.at(i)->updateContributionNodes(&mesh);
	}

	// create a body
	BodyParticle body = BodyParticle::BodyParticle();
	body.create(mesh, mat_elastic);
	body.setParticles(particles);
	std::vector<Body*> body_vector;
	body_vector.push_back(&body);

	// set nodal velocity field in nodes containing the particle
	vector<int> nodes_in_cell = mesh.getNodesInCell(position);
	
	// set velocity field

#if 0
	// rigid body motion
	for (size_t i = 0; i < nodes_in_cell.size(); i++)
	{
		mesh.getNodes()->at(nodes_in_cell.at(i))->setVelocity(Vector3d(1, 0, 0));
	}
#endif

#if 0
	// simple traction
	mesh.getNodes()->at(nodes_in_cell.at(1))->setVelocity(Vector3d(1, 0, 0));
	mesh.getNodes()->at(nodes_in_cell.at(3))->setVelocity(Vector3d(1, 0, 0));
	mesh.getNodes()->at(nodes_in_cell.at(5))->setVelocity(Vector3d(1, 0, 0));
	mesh.getNodes()->at(nodes_in_cell.at(7))->setVelocity(Vector3d(1, 0, 0));
#endif

#if 1
	// A general expansion field
	
	// expansion in x
	// plane x0
	mesh.getNodes()->at(nodes_in_cell.at(0))->setVelocity(Vector3d(-1.15, 0, 0));
	mesh.getNodes()->at(nodes_in_cell.at(2))->setVelocity(Vector3d(-1.33, 0, 0));
	mesh.getNodes()->at(nodes_in_cell.at(4))->setVelocity(Vector3d(-1.52, 0, 0));
	mesh.getNodes()->at(nodes_in_cell.at(6))->setVelocity(Vector3d(-1.91, 0, 0));

	// plane x1
	mesh.getNodes()->at(nodes_in_cell.at(1))->setVelocity(Vector3d(0.124, 0, 0));
	mesh.getNodes()->at(nodes_in_cell.at(3))->setVelocity(Vector3d(0.132, 0, 0));
	mesh.getNodes()->at(nodes_in_cell.at(5))->setVelocity(Vector3d(0.845, 0, 0));
	mesh.getNodes()->at(nodes_in_cell.at(6))->setVelocity(Vector3d(0.985, 0, 0));

	// expansion in y
	
	// plane y0
	mesh.getNodes()->at(nodes_in_cell.at(0))->setVelocity(Vector3d(-0.15, 0, 0));
	mesh.getNodes()->at(nodes_in_cell.at(1))->setVelocity(Vector3d(-0.33, 0, 0));
	mesh.getNodes()->at(nodes_in_cell.at(4))->setVelocity(Vector3d(-0.52, 0, 0));
	mesh.getNodes()->at(nodes_in_cell.at(5))->setVelocity(Vector3d(-0.91, 0, 0));

	// plane y1
	mesh.getNodes()->at(nodes_in_cell.at(2))->setVelocity(Vector3d(0.212, 0, 0));
	mesh.getNodes()->at(nodes_in_cell.at(3))->setVelocity(Vector3d(0.313, 0, 0));
	mesh.getNodes()->at(nodes_in_cell.at(6))->setVelocity(Vector3d(0.484, 0, 0));
	mesh.getNodes()->at(nodes_in_cell.at(7))->setVelocity(Vector3d(0.598, 0, 0));


	// expansion in z

	// plane z0
	mesh.getNodes()->at(nodes_in_cell.at(0))->setVelocity(Vector3d(-0.115, 0, 0));
	mesh.getNodes()->at(nodes_in_cell.at(1))->setVelocity(Vector3d(-0.333, 0, 0));
	mesh.getNodes()->at(nodes_in_cell.at(2))->setVelocity(Vector3d(-0.352, 0, 0));
	mesh.getNodes()->at(nodes_in_cell.at(3))->setVelocity(Vector3d(-0.791, 0, 0));

	// plane z1
	mesh.getNodes()->at(nodes_in_cell.at(4))->setVelocity(Vector3d(0.1212, 0, 0));
	mesh.getNodes()->at(nodes_in_cell.at(5))->setVelocity(Vector3d(0.1313, 0, 0));
	mesh.getNodes()->at(nodes_in_cell.at(6))->setVelocity(Vector3d(0.1484, 0, 0));
	mesh.getNodes()->at(nodes_in_cell.at(7))->setVelocity(Vector3d(0.1598, 0, 0));

#endif

	// interpolate the defomation gradient at the particle
	// dFp = (I + dt grad_Ip v_I)
	//  Fp^(n+1) = dFp^(n) Fp^(n)
	Interpolation::particleDeformationGradient(&mesh, &body_vector, ModelSetup::getTimeStep());
	
	// interpolate strain increment
	Interpolation::particleStrainIncrement(&mesh, &body_vector, ModelSetup::getTimeStep());

	// compute particle volume
	for (size_t i = 0; i < particles.size(); i++) {
		
		// initial volume
		double v0 = particles.at(i)->getInitialVolume();

		// curretnt volume using deformation gradient
		double v1 = particles.at(i)->getDeformationGradient().determinant()*v0;

		// current volume using density (strain increment)
		particles.at(i)->updateDensity();
		double v2 = particles.at(i)->getCurrentVolume();

		cout << "v0\t\tv1\t\tv2\n";
		cout << scientific << v0 << "\t" << v1 << "\t" << v2 << "\n";
	}

	// get grid nodes
	vector<Node*> * gridNodes = mesh.getNodes();

	// write the mesh
	Output::writeGrid(&mesh,Output::POINTS);

	// write the particles
	vector<string> resultados{ "all" };
	Output::configureResultFiels(resultados);
	Output::writeParticles(&particles);

	return 0;
}