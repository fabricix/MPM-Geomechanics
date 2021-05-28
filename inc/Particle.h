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
/// \brief Represents a Lagrangian material point
/// This class contain all Lagrangian variables
/// that represents the domain an its properties 
class Particle {

public:

	/// Default constructor
	/// 
	Particle(); 

	/// \brief Create a particle
	/// \param[in] position Particle position
	Particle(Vector3d position);

	/// \brief Create a particle
	/// \param[in] position Particle position
	/// \param[in] material Material
	Particle(Vector3d position, Material* material);
	
	/// \brief Create a particle
	/// \param[in] position Particle position
	/// \param[in] material Material
	/// \param[in] size Particle size in each direction
	Particle(Vector3d position, Material* material, Vector3d size);
	
	/// \brief Default destructor
	virtual ~Particle();

	/// \brief Update the list of nodes that the particle contributes
	/// \param[in] mesh Mesh reference
	void updateContributionNodes(Mesh & mesh);

	/// \brief Update the particle stress
	///  
	inline void updateStress() { material->updateStress(this); }

	/// \brief Returns the particle identification 
	/// \return Particle identification
	inline int getId() const { return this->id; }

	/// \brief Returns the particle's material
	/// \return Material identification
	inline int getMaterialId() const { return this->material!=0?this->material->getId():-1; } 

	/// \brief Returns the mass of the particle
	/// \return Particle mass
	inline double getMass() const { return this->mass; }

	/// \brief Returns the current particle density
	/// \return Particle density
	inline double getDensity() const { return this->density; }

	/// \brief Returns the current particle position
	/// \return Particle current position
	inline Vector3d getPosition() const { return this->position; } 

	/// \brief Returns the initial particle position
	/// \return Particle initial position
	inline Vector3d getInitialPosition() const { return this->initialPosition; } 

	/// \brief Returns the external force in particle
	/// \return Particle external force
	inline Vector3d getExternalForce() const { return this->externalForce; } 

	/// \brief Returns particle velocity
	/// \return Particle current velocity
	inline Vector3d getVelocity() const { return this->velocity; }

	/// \brief Returns the current particle stress tensor
	/// \return Particle stress
	inline Matrix3d getStress() const { return this->stress; }

	/// \brief Returns the strain increment
	/// \return Particle strain increment
	inline Matrix3d getStrainIncrement() const { return this->strainIncrement; }

	/// \brief Returns the vorticity increment
	/// \return Particle vorticity (spin) increment 
	inline Matrix3d getVorticityIncrement() const { return this->vorticityIncrement; }

	/// \brief Returns the contribution list
	/// \return Contribution node structure
	inline vector<Contribution>* getContributionNodes() { return &(this->contributionNodes); }

	/// \brief Configures the particle id
	/// \param[in] particle_id Particle identification
	inline void setId(int particle_id) { this->id=particle_id; }

	/// \brief Configures particle mass
	/// \param[in] particle_mass Particle mass
	inline void setMass(double particle_mass) { this->mass=particle_mass; } 

	/// \brief Configures particle density
	/// \param[in] particle_density Current particle density
	inline void setDensity(double particle_density) { this->density=particle_density; } 

	/// \brief Configures the current particle position
	/// \param[in] particle_position Current particle position
	inline void setPosition(Vector3d particle_position) { this->position=particle_position; } 

	/// \brief Configures the initial particle position
	/// \param[in] particle_initial_position Particle initial position
	inline void setInitialPosition(Vector3d particle_initial_position) { this->initialPosition=particle_initial_position; } 

	/// \brief Configures particle size in each direction
	/// \param[in] particle_size Current particle size
	inline void setSize(Vector3d particle_size) { this->size=particle_size; } 

	/// \brief Configures particle velocity
	/// \param[in] particle_velocity Current particle velocity
	inline void setVelocity(Vector3d particle_velocity) { this->velocity = particle_velocity; }

	/// \brief Configures the external force in particle
	/// \param[in] external_force Particle external force
	inline void setExternalForce(Vector3d external_force) { this->externalForce=external_force; }

	/// \brief Configures the strain increment
	/// \param[in] strain_increment Particle strain increment
	inline void setStrainIncrement(Matrix3d strain_increment) { this->strainIncrement=strain_increment; }

	/// \brief Configures the vorticity increment
	/// \param[in] vorticity_increment Particle vorticity (spin) increment 
	inline void setVorticityIncrement(Matrix3d vorticity_increment) { this->vorticityIncrement=vorticity_increment; }

	/// \brief Configures the current particle stress tensor
	/// \param[in] particle_stress Particle stress
	inline void setStress(Matrix3d particle_stress) { this->stress=particle_stress; } 

	/// \brief Configures the material in the particle
	/// \param[in] material Material
	inline void setMaterial(Material* material) { this->material=material; } 

	/// \brief Configures the shape function in the particle
	/// \param[in] shape Shape
	inline void setShape(Shape* shape) { this->shape=shape; }

	/// \brief Adds a external force increment
	/// \param[in] delta_external_force External force increment
	inline void addExternalForce(Vector3d delta_external_force) { this->externalForce+=delta_external_force; }

	/// \brief Returns o number of particles created
	/// \return Total created particles
	static int getTotalParticles(); 

private:

	int id;	//!< particle id
	int bodyId; //!< body id

	double mass; //!< particle mass
	double density;	//!< particle density
	double volume; //!< particle volume
	double initialVolume; //!< initial particle volume
	double plasticStrain; //!< effective plastic strain
	double porosity; //!< particle porosity

	Vector3d position; //!< particle position
	Vector3d initialPosition; //!< particle initial position
	Vector3d velocity; //!< particle velocity
	Vector3d externalForce;	//!< external force
	Vector3d size; //!< size in each direction
	
	Matrix3d stress; //!< particle stress
	Matrix3d strain; //!< particle strain
	Matrix3d strainIncrement; //!< particle strain increment
	Matrix3d vorticityIncrement; //!< particle vorticity increment
	Matrix3d deformationGradientIncrement; //!< particle deformation gradient increment
	Matrix3d velocityGradient; //!< particle deformation gradient

	vector<Contribution> contributionNodes; //!< id of nodes that the particle contributes
	Shape* shape; //!< shape function representation
	Material* material; //!< material pointer

	static int totalParticles; //!< total particle in the model
	
	/// Initialize all attributes in the particles
	///
	void initializeValues();
};

inline Particle::~Particle() {
	
	totalParticles--;
}

inline int Particle::getTotalParticles() {

	return totalParticles;
}

#endif /* PARTICLE_H_ */
