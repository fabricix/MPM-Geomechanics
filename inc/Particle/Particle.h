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
#include "Mesh/Mesh.h"
#include "Model.h"
#include "Shape/Shape.h"
#include "Materials/Material.h"

#include <vector>

class Mesh;

/// \class Particle
/// \brief Represents a Lagrangian material point
/// This class contain all Lagrangian variables
/// that represents the domain an its properties 
class Particle {

public:

	/// \brief Create a particle
	/// \param[in] position Particle position
	/// \param[in] material Material
	/// \param[in] size Particle size in each direction
	Particle(const Vector3d& position, Material* material, const Vector3d& size);
	
	/// \brief Default destructor
	virtual ~Particle();

	/// \brief Update the list of nodes that the particle contributes
	/// \param[in] mesh Mesh reference
	void updateContributionNodes(Mesh* mesh);

	/// \brief Update the particle stress
	///  
	inline void updateStress() { material->updateStress(this); }

	/// \brief Update the particle pressure
	///  
	virtual inline void updatePressure(double dt) { return; }

	/// \brief Update the particle density
	///  
	virtual void updateDensity();
	
	/// \brief Update the particle porosity
	///
	virtual inline void updatePorosity() { return; }

	/// \brief Returns the particle active status
	/// \return Particle active status
	inline bool getActive() const { return this->active; }

	/// \brief Returns the particle identification 
	/// \return Particle identification
	inline int getId() const { return this->id; }

	/// \brief Returns the particle's material
	/// \return Material identification
	inline int getMaterialId() const { return this->material!=0?this->material->getId():-1; } 

	/// \brief Returns the mass of the particle
	/// \return Particle mass
	inline double getMass() const { return this->mass; }

    /// \brief Returns fluid mass of the particle
    /// \return Particle fluid mass
    virtual inline double getMassFluid() const { return 0.0; }

	/// \brief Returns the current particle density
	/// \return Particle density
	inline double getDensity() const { return this->density; }

	/// \brief Returns the current particle density of fluid
	/// \return Particle density of fluid
	virtual inline double getDensityFluid() const { return 0.0; }

	/// \brief Returns the current particle position
	/// \return Particle current position
	inline const Vector3d& getPosition() const { return this->position; } 

	/// \brief Returns the initial particle position
	/// \return Particle initial position
	inline const Vector3d& getInitialPosition() const { return this->initialPosition; } 

	/// \brief Returns the external force in particle
	/// \return Particle external force
	inline const Vector3d& getExternalForce() const { return this->externalForce; } 

	/// \brief Returns the external force of fluid in particle
	/// \return Particle external force of fluid
	virtual inline const Vector3d* getExternalForceFluid() const { return NULL; }

	/// \brief Returns particle velocity
	/// \return Current velocity
	inline const Vector3d& getVelocity() const { return this->velocity; }

	/// \brief Returns velocity of fluid
	/// \return Current velocity of fluid
	virtual inline const Vector3d* getVelocityFluid() const { return NULL; }

	/// \brief Returns the current particle stress tensor
	/// \return Particle stress
	inline const Matrix3d& getStress() const { return this->stress; }

	/// \brief Returns the strain increment
	/// \return Particle strain increment
	inline const Matrix3d& getStrainIncrement() const { return this->strainIncrement; }

	/// \brief Returns the strain increment of fluid
	/// \return Particle strain increment of fluid
	virtual inline const Matrix3d* getStrainIncrementFluid() const { return NULL; }

	/// \brief Returns the strain
	/// \return Total particle strain 
	inline const Matrix3d& getStrain() const { return this->strain; }

	/// \brief Returns the vorticity increment
	/// \return Particle vorticity (spin) increment 
	inline const Matrix3d& getVorticityIncrement() const { return this->vorticityIncrement; }

	/// \brief Returns the contribution list
	/// \return Contribution node structure
	inline vector<Contribution>* getContributionNodes() { return &(this->contributionNodes); }

	/// \brief Configures the particle active status
	/// \param[in] particle_active Particle active status
	inline void setActive(bool particle_active) { this->active=particle_active; }

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
	inline void setPosition(const Vector3d& particle_position) { this->position=particle_position; } 

	/// \brief Configures the initial particle position
	/// \param[in] particle_initial_position Particle initial position
	inline void setInitialPosition(const Vector3d& particle_initial_position) { this->initialPosition=particle_initial_position; } 

	/// \brief Configures particle size in each direction
	/// \param[in] particle_size Current particle size
	inline void setSize(const Vector3d& particle_size) { this->size=particle_size; } 

	/// \brief Return particle size in each direction
	/// \return particle_size Current particle size
	inline const Vector3d& getSize( ) { return this->size; } 

	/// \brief Configures particle velocity
	/// \param[in] particle_velocity Current particle velocity
	inline void setVelocity(const Vector3d& particle_velocity) { this->velocity = particle_velocity; }

	/// \brief Configures particle velocity of fluid phase
	/// \param[in] particle_velocity_fluid Current particle velocity of fluid phase
	virtual inline void setVelocityFluid(const Vector3d& particle_velocity_fluid) { return; }

	/// \brief Configures the strain increment
	/// \param[in] strain_increment Particle strain increment
	inline void setStrainIncrement(const Matrix3d& strain_increment) { this->strainIncrement=strain_increment; this->strain+=strain_increment; }

	/// \brief Configures the strain increment of fluid phase
	/// \param[in] strain_increment Particle strain increment of fluid phase
	virtual inline void setStrainIncrementFluid(const Matrix3d& strain_increment) { return; }

	/// \brief Configures the vorticity increment
	/// \param[in] vorticity_increment Particle vorticity (spin) increment 
	inline void setVorticityIncrement(const Matrix3d& vorticity_increment) { this->vorticityIncrement=vorticity_increment; }

	/// \brief Configures the current particle stress tensor
	/// \param[in] particle_stress Particle stress
	inline void setStress(const Matrix3d& particle_stress) { this->stress=particle_stress; } 

	/// \brief Configures the material in the particle
	/// \param[in] material Material
	inline void setMaterial(Material* material) { this->material=material; }

	/// \brief Configures the shape function in the particle
	/// \param[in] shape Shape
	inline void setShape(Shape* shape) { this->shape=shape; }

	/// \brief Adds a external force increment
	/// \param[in] delta_external_force External force increment
	inline void addExternalForce(const Vector3d& delta_external_force) { this->externalForce+=delta_external_force; }

	/// \brief Adds a external fluid force increment
	/// \param[in] delta_external_fluid_force External fluid force increment
	virtual inline void addExternalForceFluid(const Vector3d& delta_external_fluid_force) { return; }

	/// \brief Returns o number of particles created
	/// \return Total created particles
	static int getTotalParticles();

	/// \brief Add a plastic strain increment
	/// \param[in] deltaPlasticStrain Plastic strain increment
	inline void addPlasticStrain(double deltaPlasticStrain) { this->plasticStrain+=deltaPlasticStrain; }

	/// \brief Returns pressure of fluid
    /// \return Current pore pressure of fluid
    virtual inline double getPressureFluid() const { return 0.0; }

	/// \brief Configure the pressure of fluid
    /// \param[in] pressure Pressure of the fluid
    virtual inline void setPressureFluid(double pressure) { return; }

    /// \brief Returns current porosity
    /// \return Current porosity of mixture
    virtual inline double getPorosity() const { return 0.0; }

    /// \brief Returns Saturation of fluid in void in mixture
    /// \return saturation
    virtual inline double getSaturation() const { return 0.0; }

    /// \brief Returns the drag force of fluid in particle
    /// \return Particle drag force of fluid
    virtual inline Vector3d getDragForceFluid() const { return Vector3d::Zero(); }

	/// \brief Configures defomation gradient
    /// \param[in] deformation_gradient Particle deformation gradient
    inline void setDeformationGradient(const Matrix3d& deformation_gradient) { this->deformationGradient=deformation_gradient; }

    /// \brief Returns deformation gradient
    /// \return Particle deforamtion gradient
    inline const Matrix3d& getDeformationGradient() const { return this->deformationGradient; }

protected:

	bool active; //!< is particle active
	int id;	//!< particle id
	int bodyId; //!< body id

	double mass; //!< particle mass: \f$m_p\f$
	double density;	//!< current particle density: \f$\rho_p\f$
	double plasticStrain; //!< current effective plastic strain: \f$\epsilon_p^{pleff}=\sqrt{2/3\epsilon_{pij}^{pl}\epsilon_{pij}^{pl}}\f$

	Vector3d position; //!< current particle position: \f$x_{ip}\f$
	Vector3d initialPosition; //!< particle initial position: \f$x_{ip}^{0}\f$
	Vector3d velocity; //!< current particle velocity: \f$v_{ip}\f$
	Vector3d externalForce;	//!< particle external force: \f$f_{ip}^ext\f$
	Vector3d size; //!< current size in each direction: \f$\Omega_{ip}\f$
	
	Matrix3d stress; //!< current particle stress: \f$\sigma_{ijp}\f$
	Matrix3d strain; //!< current particle strain: \f$\epsilon_{ijp}\f$
	Matrix3d strainIncrement; //!< current particle strain increment: \f$\Delta \epsilon_{ijp}\f$
	Matrix3d vorticityIncrement; //!< particle vorticity increment: \f$\Delta \Omega _{ijp}\f$
	Matrix3d deformationGradient; //!< particle deformation gradient: \f$ F_{ijp}=\partial x_i / \partial X_j \f$

	vector<Contribution> contributionNodes; //!< id of nodes that the particle contributes
	Shape* shape; //!< shape functions values (see class Shape)
	Material* material; //!< particle material (see class Material)

	static int totalParticles; //!< total particle in the model
};

inline Particle::~Particle() {
	
	totalParticles--;
}

inline int Particle::getTotalParticles() {

	return totalParticles;
}

#endif /* PARTICLE_H_ */
