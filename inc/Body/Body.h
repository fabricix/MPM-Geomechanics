// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#ifndef BODY_H_
#define BODY_H_

#include<vector>
using std::vector;

#include "Particle/Particle.h"
#include "Materials/Material.h"

/// \class Body
/// \brief Represents a body in the space forming by a group of materials points and identified by a number.
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
	virtual void create(Mesh& mesh, Material* material) = 0;
	
	/// \brief Configure the material id
	/// \param[in] material_id Material identification
	virtual void setMaterialId(int material_id) = 0;
	
	/// \brief Return the material id
	/// \return Material identification
	virtual int getMaterialId() const = 0;

	/// \brief Return the body identification
	/// \return Body identification
	inline int getId() const { return this->id; }

	/// \brief Return the initial velocity of the body
	/// \return Initial body velocity
	inline Vector3d getInitialVelocity() const { return this->initial_velocity; }

	/// \brief Return the particles forming the body
	/// \return A reference to a vector containing Particle pointers
	inline vector<Particle*>* getParticles() { return &(this->particles); }

	/// \brief Return the body friction Coefficient
	/// \return Body friction Coefficient
	//inline double getFrictionCoefficient() const { return this->friction; }
	
	/// \brief Configure the id of the body
	/// \param[in] body_id Body identification 
	inline void setId(int body_id) { this->id=body_id; }

	/// \brief Configure the initial velocity of the body
	/// \param[in] initial_velocity Body identification 
	inline void setInitialVelocity(Vector3d initial_velocity) { this->initial_velocity=initial_velocity; }

	/// \brief Configure the particles in the body
	/// \param[in] particle_list A vector containing pointers to the particle
	inline void setParticles(const vector<Particle*>& particle_list) { this->particles=particle_list; }

	/// \brief Configure the friction coefficient of the body
	/// \param[in] body_mu Body friction coefficient 
	//inline void setFrictionCoefficient(double _friction) { this->friction = _friction; }

	/// \brief Add particles to the current particle list
	/// \param[in] particle_list A vector containing pointers to particles
	inline void insertParticles(const vector<Particle*>& particle_list) { this->particles.insert(this->particles.end(),particle_list.begin(),particle_list.end()); }
	
protected:
	
	int id; //!< body identification
	Vector3d initial_velocity; //!< initial velocity of the body
	vector<Particle*> particles; //!< material points forming the body
};

inline Body::Body():id(-1),initial_velocity(0,0,0) { }
inline Body::~Body() { }

#endif /* BODY_H_ */
