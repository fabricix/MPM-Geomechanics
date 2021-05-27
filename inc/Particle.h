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
	Particle(Vector3d);

	/// \brief Create a particle
	/// \param[in] position Particle position
	/// \param[in] *material Material pointer
	Particle(Vector3d, Material*);
	
	/// \brief Create a particle
	/// \param[in] position Particle position
	/// \param[in] *material Material pointer
	/// \param[in] size Particle size in each direction
	Particle(Vector3d, Material*, Vector3d);
	
	/// \brief Default destructor
	virtual ~Particle();

	/// \brief Update the list of nodes that the particle contributes
	/// \param[in] mesh Mesh reference
	void updateContributionNodes(Mesh &);

	/// \brief Update the particle stress
	///  
	void updateStress();

	/// \brief Returns the particle identification 
	/// \param[out] particle_id Particle identification
	int getId() const;

	/// \brief Returns the particle's material
	/// \param[out] material_id Material index
	int getMaterialId() const; 

	/// \brief Returns the mass of the particle
	/// \param[out] mass Particle mass
	double getMass() const; 

	/// \brief Returns the current particle density
	/// \param[out] density Particle density
	double getDensity() const; 

	/// \brief Returns the momentum of the particle
	/// \param[out] momentum Particle momentum
	Vector3d getMomentum() const; 

	/// \brief Returns the current particle position
	/// \param[out] position Particle current position
	Vector3d getPosition() const; 

	/// \brief Returns the external force in particle
	/// \param[out] external_force Particle external force
	Vector3d getExternalForce() const; 

	/// \brief Returns particle velocity
	/// \param[out] velocity Particle current velocity
	Vector3d getVelocity() const; 

	/// \brief Returns the current particle stress tensor
	/// \param[out] stress Particle stress
	Matrix3d getStress() const; 

	/// \brief Returns the strain increment
	/// \param[out] delta_strain Particle strain increment
	Matrix3d getStrainIncrement() const; 

	/// \brief Returns the vorticity increment
	/// \param[out] delta_vorticity Particle vorticity (spin) increment 
	Matrix3d getVorticityIncrement() const; 

	/// \brief Returns the contribution list
	/// \param[out] contribution Contribution node structure
	vector<Contribution>* getContributionNodes(); 

	/// \brief Configures the particle id
	/// \param[in] particle_id Particle identification
	void setId(int); 

	/// \brief Configures particle mass
	/// \param[in] particle_mass Particle mass
	void setMass(double); 

	/// \brief Configures particle volume
	/// \param[in] particle_volume Current particle volume
	void setVolume(double); 

	/// \brief Configures particle density
	/// \param[in] particle_density Current particle density
	void setDensity(double); 

	/// \brief Configures the current particle position
	/// \param[in] particle_position Current particle position
	void setPosition(Vector3d); 

	/// \brief Configures particle size in each direction
	/// \param[in] particle_size Current particle size
	void setSize(Vector3d); 

	/// \brief Configures particle velocity
	/// \param[in] particle_velocity Current particle velocity
	void setVelocity(Vector3d); 

	/// \brief Configures the external force in particle
	/// \param[in] external_force Particle external force
	void setExternalForce(Vector3d); 

	/// \brief Configures the strain increment
	/// \param[in] delta_strain Particle strain increment
	void setStrainIncrement(Matrix3d); 

	/// \brief Configures the vorticity increment
	/// \param[in] delta_vorticity Particle vorticity (spin) increment 
	void setVorticityIncrement(Matrix3d); 

	/// \brief Configures the current particle stress tensor
	/// \param[in] stress Particle stress
	void setStress(Matrix3d); 

	/// \brief Configures the material in the particle
	/// \param[in] *material Material pointer
	void setMaterial(Material*); 

	/// \brief Configures the shape function in the particle
	/// \param[in] *shape Shape pointer
	void setShape(Shape*); 

	/// \brief Adds a external force increment
	/// \param[in] delta_external_force External force increment
	void addExternalForce(Vector3d); 

	/// \brief Returns o number of particles created
	/// \param[out] total_particles Total created particles
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

#endif /* PARTICLE_H_ */
