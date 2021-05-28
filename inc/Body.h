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
	/// \param[in] material Material
	virtual void create(Mesh& mesh, Material* material)=0;
	
	/// \brief Configure the material id
	/// \param[in] material_id Material identification
	virtual void setMaterialId(int material_id)=0;
	
	/// \brief Return the material id
	/// \param[out] mat_id
	virtual int getMaterialId() const=0;

	/// \brief Return the body identification
	/// \param[out] body_id Body identification
	inline int getId() const { return this->id; }

	/// \brief Return the particles forming the body
	/// \param[out] particles A reference to a vector containing Particle pointers
	inline vector<Particle*>* getParticles() { return &(this->particles); }
	
	/// \brief Configure the id of the body
	/// \param[in] body_id Body identification 
	inline void setId(int body_id) { this->id=body_id; }

	/// \brief Configure the particles in the body
	/// \param[in] particle_list A vector containing pointers to the particle
	inline void setParticles(vector<Particle*> particle_list) { this->particles=particle_list; }

	/// \brief Add particles to the current particle list
	/// \param[in] particle_list A vector containing pointers to particles
	inline void insertParticles(vector<Particle*> particle_list) { this->particles.insert(this->particles.end(),particle_list.begin(),particle_list.end()); }
	
private:
	
	int id; //!< body identification

	vector<Particle*> particles; //!< material points forming the body  
};


inline Body::Body():id(-1) {
	
}

inline Body::~Body() {
	
}

#endif /* BODY_H_ */
