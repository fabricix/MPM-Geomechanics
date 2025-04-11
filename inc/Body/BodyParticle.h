// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#ifndef INC_BODY_BODYPARTICLE_H_
#define INC_BODY_BODYPARTICLE_H_

#include <Body/Body.h>

/// \class BodyParticle
/// \brief Is a Body created by a list of particles.
///
/// ## Introduction
///
/// This class allows to create a 3D body in the space by a list of particles.
///

class BodyParticle: public Body {

public:

    /// \brief Default constructor
    ///
    BodyParticle();

    /// \brief Default destruction
    ///
    ~BodyParticle();

    /// \brief Configure the material id
    /// \param[in] material_id Material identification
    inline virtual void setMaterialId(int material_id) { this->materialId = material_id; }
    
    /// \brief Return the material identification
    /// \return Material identification
    inline virtual int getMaterialId() const { return this->materialId; }

    /// \brief Configure the particles in the body
    /// \param[in] particle_list A vector containing pointers to the particle
    inline void setParticles(const vector<Particle*>& particle_list) { Body::insertParticles(particle_list); }

    /// \brief Add particles to the current particle list
    /// \param[in] particle_list A vector containing pointers to particles
    inline void insertParticles(const vector<Particle*>& particle_list) { Body::insertParticles(particle_list); }
    
    /// \brief Return the particles forming the body
    /// \return A reference to a vector containing Particle pointers
    inline vector<Particle*>* getParticles() { return Body::getParticles(); }
    
    /// \brief Create a body in space using particles
    /// \param[in] &mesh Mesh reference
    /// \param[in] *material Material pointer 
    virtual void create(Mesh& mesh, Material* material);

private:

    int materialId; //!< material identification
};

#endif /* INC_BODY_BODYPARTICLE_H_ */
