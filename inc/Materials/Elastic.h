/*
 * Elastic.h
 *
 *  Created on: 4 de mai de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#ifndef ELASTIC_H_
#define ELASTIC_H_

#include "Materials/Material.h"
#include "Particle/Particle.h"

/// \class Elastic
/// \brief Represents an elastic material.
class Elastic: public Material {

public:

	/// \brief Create a linear elastic material
	/// \param[in] id Material identification
	/// \param[in] density Material density \f$\rho\f$
	/// \param[in] young Young's modulus \f$E\f$
	/// \param[in] poisson Poisson's ratio \f$\nu\f$
	Elastic(int id, double density, double mu, double young, double poisson);
	
	/// \brief Default destructor
	///
	virtual ~Elastic();
	
	/// \brief Configure the Young's modulus
	/// \param[in] young Young's modulus \f$E\f$
	inline void setYoung(double young) { this->Young=young; }
	
	/// \brief Configure the Poisson's ratio
	/// \param[in] poisson Poisson's ratio \f$\nu\f$
	inline void setPoisson(double poisson) { this->Poisson=poisson; }

	/// \brief Return the Shear modulus
	/// \return Shear modulus \f$G=\frac{E}{2(1+\nu)}\f$
	inline double getShearModulus() const { return this->Young/2.0/(1.0+this->Poisson); }
	
	/// \brief Return the Bulk modulus
	/// \return Bulk modulus \f$K=\frac{E}{3(1-2\nu)}\f$ 
	inline double getBulkModulus() const { return this->Young/3.0/(1.0-2.0*this->Poisson); }
	
	/// \brief Update the stress in the particle
	/// \param[in] particle Particle
	virtual void updateStress(Particle* particle) const;
	
	/// \brief Return the material type
	/// \return Material type Material::MaterialType
	inline virtual Material::MaterialType getType() const { return Material::getType(); }

	/// \brief Get sound speed
	/// \return sound_speed Sound speed
	virtual double getSoundSpeed( ) const;
	
protected:
	
	double Young; //!< Young's modulus \f$E\f$
	
	double Poisson; //!< Poisson's ratio \f$\nu\f$
};

inline Elastic::Elastic(int id, double density, double mu, double young, double poisson)
: Material(id, density, mu, Material::MaterialType::ELASTIC)
{
	this->Young=young;
	this->Poisson=poisson;
}

inline Elastic::~Elastic() { }

#endif /* ELASTIC_H_ */
