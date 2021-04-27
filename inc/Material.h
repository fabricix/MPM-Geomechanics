/*
 * Material.h
 *
 *  Created on: 13 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#ifndef MATERIAL_H_
#define MATERIAL_H_

/// \class Material
/// \brief Represents a material constitutive model.
class Material {
public:
	Material(); //!< default constructor
	virtual ~Material(); //< default destructor
};

#endif /* MATERIAL_H_ */
