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

	Elastic(int id, double density, double Young, double Poisson); //!< create a linear material
	virtual ~Elastic();
	
	void setYoung(double); //!< configures the Young's modulus
	void setPoisson(double); //!< configures Poisson's ratio

	double getShearModulus(); //!< returns th shear modulus
	double getBulkModulus(); //!< returns the bulk modulus
	
	virtual void updateStress(Particle* particle); //!< update the stress in the particle
	virtual Material::MaterialType getType(); //!< returns the material type
	
private:
	
	double Young; //!< Young's modulus
	double Poisson; //!< Poisson's ratio
};

#endif /* ELASTIC_H_ */
