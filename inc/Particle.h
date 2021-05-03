/*
 * Particle.h
 *
 *  Created on: 13 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#ifndef PARTICLE_H_
#define PARTICLE_H_

#include "../inc/Eigen/Core"
using Eigen::Matrix3d;
using Eigen::Vector3d;

#include "Contribution.h"
#include "Mesh.h"
#include "Model.h"
#include "Shape.h"

#include <vector>

class Mesh;

/// \class Particle
/// \brief Represents a material point
/// This class contain all Lagrangian variables
/// that represents the domain an its properties 
class Particle {

public:

	Particle(); //!< default constructor
	Particle(Vector3d); //!< create a particle in a coordinate
	virtual ~Particle(); //!< default destructor

	void updateContributionNodes(Mesh &); //!< update the list of nodes that contributes

	// get methods
	int getId(); //!< returns the particle identification 
	double getMass(); //!< returns the mass of the particle
	Vector3d getPosition(); //!< returns the current particle position
	vector<Contribution>* getContributionNodes(); //<! returns the contribution list
	static int getTotalParticles(); //<! returns o number of particles created
	
	// set methods
	void setSize(Vector3d); //!< configures particle size in each direction
	void setMass(double); //!< configures particle mass

private:

	int id;	//!< particle id
	int materialId;	//!< material id
	int cellId; //!< cell id
	int bodyId; //!< body id

	double mass; //!< particle mass
	double density;	//!< particle density
	double volume; //!< particle volume
	double initialVolume; //!< initial particle volume
	double plasticStrain; //!< effective plastic strain
	double porosity; //!< particle porosity

	Vector3d position; //!< particle position
	Vector3d initialPosition; //!< initial particle position
	Vector3d velocity; //!< particle velocity
	Vector3d momentum; //!< particle momentum
	Vector3d externalForce;	//!< external force
	Vector3d size; //!< size in each direction
	
	Matrix3d stress; //!< particle stress
	Matrix3d strain; //!< particle strain
	Matrix3d deformationGradient; //!< particle deformation gradient
	Matrix3d deformationGradientIncrement; //!< particle deformation gradient increment
	Matrix3d velocityGradient; //!< particle deformation gradient

	vector<Contribution> contributionNodes; //!< id of nodes that the particle contributes

	Shape shape; //!< shape function representation

	void initializeValues(); //!< initialization of all data in particle
	
	static int totalParticles; //!< total particle in the model
};

#endif /* PARTICLE_H_ */
