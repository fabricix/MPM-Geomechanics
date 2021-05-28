/*
 * Elastic.h
 *
 *  Created on: 4 de mai de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#ifndef ELASTIC_H_
#define ELASTIC_H_

#include <Material.h>
#include "Particle.h"

/// \class Elastic
/// \brief Represents an elastic material.
class Elastic: public Material {

public:

	/// \brief Create a linear elastic material
	/// \param[in] material_id Material identification
	/// \param[in] material_density Material density \f$\rho\f$
	/// \param[in] young_modulus Young's modulus \f$E\f$
	/// \param[in] poisson_modulus Poisson's ratio \f$\nu\f$
	Elastic(int, double, double, double);
	
	/// \brief Default destructor
	///
	virtual ~Elastic();
	
	/// \brief Configure the Young's modulus
	/// \param[in] young_modulus Young's modulus \f$E\f$
	inline void setYoung(double young_modulus) { this->Young=young_modulus; }
	
	/// \brief Configure the Poisson's ratio
	/// \param[in] poisson_modulus Poisson's ratio \f$\nu\f$
	inline void setPoisson(double poisson_modulus) { this->Poisson=poisson_modulus; }

	/// \brief Configure the Jaumann rate active
	/// \param[in] jaumann_active Jaumann active
	inline void setJaumann(bool jaumann_active=false) { this->jaumann=jaumann_active; }

	/// \brief Return the Shear modulus
	/// \param[out] shear_modulus Shear modulus \f$G=\frac{E}{2(1+\nu)}\f$
	inline double getShearModulus() const { return this->Young/2.0/(1.0+this->Poisson); }
	
	/// \brief Return the Bulk modulus
	/// \param[out] shear_modulus Shear modulus \f$K=\frac{E}{3(1-2\nu)}\f$ 
	inline double getBulkModulus() const { return this->Young/3.0/(1.0-2.0*this->Poisson); }
	
	/// \brief Update the stress in the particle
	/// \param[in] *particle Particle pointer
	virtual void updateStress(Particle* particle);
	
	/// \brief Return the material type
	/// \param[out] material_type Material type
	inline virtual Material::MaterialType getType() { return Material::getType(); }
	
private:
	
	double Young; //!< Young's modulus \f$E\f$
	
	double Poisson; //!< Poisson's ratio \f$\nu\f$

	bool jaumann; //!< Jaumann rate activated
};

inline Elastic::Elastic(int id, double density, double Young, double Poisson)
: Material(id, density, Material::MaterialType::ELASTIC)
{
	this->setYoung(Young);
	this->setPoisson(Poisson);
	this->setJaumann(true);
}

inline Elastic::~Elastic() {
	
}

#endif /* ELASTIC_H_ */