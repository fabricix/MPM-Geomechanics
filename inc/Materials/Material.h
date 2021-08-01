/*
 * Material.h
 *
 *  Created on: 13 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#ifndef MATERIAL_H_
#define MATERIAL_H_

#include "Eigen/Core"
using Eigen::Vector3d;

class Particle;

/// \class Material
/// \brief Represents a material.
class Material {

public:

	/// \enum MaterialType
	/// Describes the material's constitutive model type
	enum MaterialType{ NONE, ELASTIC, ELASTOPLASTIC };

	/// \brief Default constructor
	///
	Material(int id=-1,double density=0, MaterialType=MaterialType::NONE);
	
	/// \brief Default destructor
	///
	virtual ~Material();
	
	/// \brief Return the identification
	/// \return Material identification
	inline int getId() const { return this->id; }
	
	/// \brief Returns material density
	/// \return Material density
	inline double getDensity() const { return this->density; }

	/// \brief Returns fluid density on material
	/// \return Fluid density
	inline double getDensityFluid() const { return this->densityFluid; }

	/// \brief Returns material porosity
	/// \return Material porosity
	inline double getPorosity() const { return this->porosity; }

	/// \brief Returns Hydraulic conductivity
	/// \return Hydraulic Conductivity
	inline Vector3d getHydraulicConductivity() const { return this->hydraulicConductivity; }

	/// \brief Configure the material identification
	/// \param[in] material_id Material identification
	inline void setId(int material_id) { this->id=material_id; }
	
	/// \brief Configures the material density
	/// \param[in] material_density Material density 
	inline void setDensity(double material_density) { this->density=material_density; }
	
	/// \brief Configures the material porosity
	/// \param[in] material_porosity Material porosity
	inline void setPorosity(double material_porosity) { this->porosity=material_porosity; }

	/// \brief Configures the material hydraulic conductivity
	/// \param[in] hydraulic_conductivity Material hydraulic conductivity
	inline void setHydraulicConductivity(Vector3d hydraulic_conductivity) { this->hydraulicConductivity=hydraulic_conductivity; }

	/// \brief Configures the material type
	/// \param[in] material_type Material::MaterialType
	inline void setType(MaterialType material_type) { this->type=material_type; }
	
	/// \brief Returns the material type
	/// \return Material::MaterialType
	inline virtual MaterialType getType() const { return this->type; }

	/// \brief Update the stress tensor
	/// \param[in] particle Particle
	virtual void updateStress(Particle* particle) const = 0;

	/// \brief Get sound speed
	/// \param[out] sound_speed Sound speed
	virtual double getSoundSpeed( ) const = 0;

protected:
	
	int id; //!< material identification

	double density; //!< initial material density \f$\rho^{0}\f$ or initial solid density in two-phase calculations \f$\rho^{s,0}\f$

	double densityFluid; //!< initial density of the fluid phase in two-phase calculations \f$\rho^{f,0}\f$

	double porosity; //!< initial porosity \f$n^0\f$

	Vector3d hydraulicConductivity; //!< hydraulic conductivity of the fluid in the material \f$k_i\f$
	
	MaterialType type; //!< material type
};

inline Material::Material(int id, double density, MaterialType type) {
	
	this->density=density;
	this->id=id;
	this->type=type;
	this->porosity=0.0;
	this->densityFluid=0.0;
	this->hydraulicConductivity.setZero();
}

inline Material::~Material() { }

#endif /* MATERIAL_H_ */
