/*
 * ElasticJaumann.h
 *
 *  Created on: 28 de mai de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#ifndef ELASTICJAUMANN_H_
#define ELASTICJAUMANN_H_

#include <Elastic.h>

/// \class ElasticLaumann
/// \brief Represents an elastic material with the Jaumann rate correction
class ElasticJaumann: public Elastic {

public:
	
	/// \brief Create a linear elastic material
	/// \param[in] material_id Material identification
	/// \param[in] material_density Material density \f$\rho\f$
	/// \param[in] young_modulus Young's modulus \f$E\f$
	/// \param[in] poisson_modulus Poisson's ratio \f$\nu\f$
	ElasticJaumann(int, double, double, double);

	/// \brief Default destructor
	///
	virtual ~ElasticJaumann();

	/// \brief Update the stress in the particle
	/// \param[in] *particle Particle pointer
	virtual void updateStress(Particle* particle);

};

inline ElasticJaumann::ElasticJaumann(int id, double density, double Young, double Poisson)
: Elastic(id, density, Young, Poisson){

}

ElasticJaumann::~ElasticJaumann() {

}

#endif /* ELASTICJAUMANN_H_ */
