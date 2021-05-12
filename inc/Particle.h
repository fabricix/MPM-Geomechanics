/*
 * Particle.h
 *
 *  Created on: 13 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#ifndef PARTICLE_H_
#define PARTICLE_H_

#include "Eigen/Core"
using Eigen::Matrix3d;
using Eigen::Vector3d;

#include "Contribution.h"
#include "Mesh.h"
#include "Model.h"
#include "Shape.h"
#include "Material.h"

#include <vector>

class Mesh;

/// \class Particle
/// \brief Represents a material point
/// This class contain all Lagrangian variables
/// that represents the domain an its properties 
class Particle {

public:

	Particle(); //!< default constructor
	Particle(Vector3d); //!< create a particle
	Particle(Vector3d, Material*); //!< create a particle
	virtual ~Particle(); //!< default destructor

	void updateContributionNodes(Mesh &); //!< update the list of nodes that contributes
	void updateStress();//!< update the stress

	int getId(); //!< returns the particle identification 
	int getMaterialId(); //!< returns the particle's material
	double getMass(); //!< returns the mass of the particle
	double getDensity(); //!< returns the current particle density
	Vector3d getMomentum(); //!< returns the momentum of the particle
	Vector3d getPosition(); //!< returns the current particle position
	Vector3d getExternalForce(); //!< returns the external force in particle
	Matrix3d getStress(); //!< returns the current particle stress tensor
	Matrix3d getStrainIncrement(); //!< returns the strain increment
	Matrix3d getVorticityIncrement(); //!< returns the vorticity increment
	vector<Contribution>* getContributionNodes(); //<! returns the contribution list

	void setId(int); //!< configures the particle id
	void setMass(double); //!< configures particle mass
	void setVolume(double); //!< configures particle volume
	void setDensity(double); //!< configures particle density
	void setInitialPosition(Vector3d); //!< configures the initial position
	void setPosition(Vector3d); //!< configures the current position
	void setSize(Vector3d); //!< configures particle size in each direction
	void setStrainIncrement(Matrix3d); //!< configures the strain increment
	void setVorticityIncrement(Matrix3d); //!< configures the vorticity increment
	void setStress(Matrix3d); //!< configures the current particle stress tensor
	void setMaterial(Material*); //!< configures the material in the particle
	void setShape(Shape*); //!< configures the shape function in the particle

	static int getTotalParticles(); //<! returns o number of particles created

private:

	int id;	//!< particle id
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
	Matrix3d strainIncrement; //!< particle strain increment
	Matrix3d vorticityIncrement; //!< particle vorticity increment
	Matrix3d deformationGradient; //!< particle deformation gradient
	Matrix3d deformationGradientIncrement; //!< particle deformation gradient increment
	Matrix3d velocityGradient; //!< particle deformation gradient

	vector<Contribution> contributionNodes; //!< id of nodes that the particle contributes
	Shape* shape; //!< shape function representation
	Material* material; //!< material pointer

	static int totalParticles; //!< total particle in the model
	
	void initializeValues(); //!< initialization of all data in particle
};

#endif /* PARTICLE_H_ */
