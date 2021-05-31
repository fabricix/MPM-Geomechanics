/*
 * ElasticJaumann.h
 *
 *  Created on: 28 de mai de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#ifndef ELASTICJAUMANN_H_
#define ELASTICJAUMANN_H_

#include <Elastic.h>

/// \class ElasticJaumann
/// \brief Represents an elastic material with the Jaumann rate correction
class ElasticJaumann: public Elastic {

public:
	
	/// \brief Create a linear elastic material
	/// \param[in] id Material identification
	/// \param[in] density Material density \f$\rho\f$
	/// \param[in] young Young's modulus \f$E\f$
	/// \param[in] poisson Poisson's ratio \f$\nu\f$
	ElasticJaumann(int id, double density, double young, double poisson);

	/// \brief Default destructor
	///
	virtual ~ElasticJaumann();

	/// \brief Update the stress in the particle
	/// \param[in] particle Particle
	virtual void updateStress(Particle* particle) const;

};

inline ElasticJaumann::ElasticJaumann(int id, double density, double young, double poisson)
: Elastic(id, density, young, poisson){

}

ElasticJaumann::~ElasticJaumann() {

}

#endif /* ELASTICJAUMANN_H_ */