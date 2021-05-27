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
	void setYoung(double);
	
	/// \brief Configure the Poisson's ratio
	/// \param[in] poisson_modulus Poisson's ratio \f$\nu\f$
	void setPoisson(double);

	/// \brief Return the Shear modulus
	/// \param[out] shear_modulus Shear modulus \f$G=\frac{E}{2(1+\nu)}\f$
	double getShearModulus();
	
	/// \brief Return the Bulk modulus
	/// \param[out] shear_modulus Shear modulus \f$K=\frac{E}{3(1-2\nu)}\f$ 
	double getBulkModulus();
	
	/// \brief Update the stress in the particle
	/// \param[in] *particle Particle pointer
	virtual void updateStress(Particle* particle);
	
	/// \brief Return the material type
	/// \param[out] material_type Material type
	virtual Material::MaterialType getType();
	
private:
	
	double Young; //!< Young's modulus \f$E\f$
	
	double Poisson; //!< Poisson's ratio \f$\nu\f$
};

#endif /* ELASTIC_H_ */
