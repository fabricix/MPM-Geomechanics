/*
 * Body.h
 *
 *  Created on: 13 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#ifndef BODY_H_
#define BODY_H_

#include<vector>
using std::vector;

#include "Particle.h"
#include "Material.h"

/// \class Body
/// \brief Represents a body
class Body {

public:

	/// \brief Default constructor
	///
	Body();

	/// \brief Default destructor
	///
	virtual ~Body();
	
	/// \brief Create a body with particles and material
	/// \param[in] mesh Computational Mesh reference
	/// \param[in] *material Pointer to a Material
	virtual void create(Mesh& mesh, Material* material)=0;
	
	/// \brief Configure the material id
	/// \param[in] mat_id Material identification
	virtual void setMaterialId(int)=0;
	
	/// \brief Return the material id
	/// \param[out] mat_id
	virtual int getMaterialId() const=0;

	/// \brief Return the body identification
	/// \param[out] body_id Body identification
	int getId() const;

	/// \brief Return the particles forming the body
	/// \param[out] particles A reference to a vector containing Particle pointers
	vector<Particle*>& getParticles();

	/// \brief Return the total bodies in the model
	/// \param[out] total_bodies Total created bodies
	static int getTotalBodies();
	
	/// \brief Configure the id of the body
	/// \param[in] body_id Body identification 
	void setId(int);

	/// \brief Configure the particles in the body
	/// \param[in] particle_vector A vector containing pointers to the particle
	void setParticles(vector<Particle*>);

	/// \brief Add particles to the current particle list
	/// \param[in] particle_vectr A vector containing pointers to particles
	void insertParticles(vector<Particle*>);
	
private:
	
	int id; //!< body identification

	vector<Particle*> particles; //!< material points forming the body  
	
	static int totalBodies; //!< number of bodies
};

#endif /* BODY_H_ */
