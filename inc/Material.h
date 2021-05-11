/*
 * Material.h
 *
 *  Created on: 13 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#ifndef MATERIAL_H_
#define MATERIAL_H_

/// \class Material
/// \brief Represents a material.
class Material {

public:

	/// \enum MaterialType
	/// Describes the material's constitutive model type
	enum MaterialType{NONE,ELASTIC};

	Material(int id=-1,double density=0, MaterialType=MaterialType::NONE); //!< default constructor
	virtual ~Material(); //< default destructor
	
	int getId(); //!< returns the identification
	double getDensity(); //!< returns density

	void setId(int); //!< configures the material identification
	void setDensity(double); //!< configures the density
	void setType(MaterialType); //!< configures the type

	virtual void updateStress()=0; //!< update the stress tensor
	virtual MaterialType getType(); //!< returns the material type

private:
	
	int id; //!< material identification
	double density; //!< density
	MaterialType type; //!< material type
};

#endif /* MATERIAL_H_ */