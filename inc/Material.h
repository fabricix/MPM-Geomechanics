/*
 * Material.h
 *
 *  Created on: 13 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#ifndef MATERIAL_H_
#define MATERIAL_H_

class Particle;

/// \class Material
/// \brief Represents a material.
class Material {

public:

	/// \enum MaterialType
	/// Describes the material's constitutive model type
	enum MaterialType{NONE,ELASTIC};

	/// \brief Default constructor
	///
	Material(int id=-1,double density=0, MaterialType=MaterialType::NONE);
	
	/// \brief Default destructor
	///
	virtual ~Material();
	
	/// \brief Return the identification
	/// \param[out] material_id Material identification
	int getId();
	
	/// \brief Returns material density
	/// \param[out] density Material density
	double getDensity();

	/// \brief Configure the material identification
	/// \param[in] material_id Material identification
	void setId(int);
	
	/// \brief Configures the material density
	/// \param[in] material_density Material density 
	void setDensity(double);
	
	/// \brief Configures the material type
	/// \param[in] material_type Material::MaterialType
	void setType(MaterialType);

	/// \brief Update the stress tensor
	/// \param[in] particle* Particle pointer
	virtual void updateStress(Particle*)=0;
	
	/// \brief Returns the material type
	/// \param[out] material_type Material::MaterialType
	virtual MaterialType getType();

private:
	
	int id; //!< material identification

	double density; //!< density \f$\rho\f$
	
	MaterialType type; //!< material type
};

#endif /* MATERIAL_H_ */
