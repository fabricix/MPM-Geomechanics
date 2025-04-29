// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#ifndef SOLVER_H_
#define SOLVER_H_

#include <vector>
#include "BoundingBox.h"
using std::vector;

#include "TerrainContact.h"

class Body;
class Mesh;
class Particle;

/// \class Solver
/// \brief Represents the operations to solve the equations in time.
class Solver {

public:

	/// Particles per thread
	vector<vector<Particle*>*>* particlesPerThread;

	/// Interface Particles per thread
	vector<vector<Particle*>*>* interfaceParticlesPerThread;

	/// \brief Default constructor
	///
	Solver();

	/// \brief Default destructor
	///
	virtual ~Solver();
	
	/// \brief Solve the problem in time
	///
	virtual void Solve()=0;

	/// \brief Returns the mesh pointer
	///
	Mesh* getMesh() { return this->mesh; };

	/// \brief Register the mesh in the solver
	///
	inline void registerMesh(Mesh* mesh){ this->mesh=mesh; }
	
	/// \brief Register the bodies in the solver
	///
	inline void registerBodies(vector<Body*>* bodies){ this->bodies=bodies; }

	/// \brief Register the particles in the solver
	inline void registerParticles(vector<Particle*>* p){ this->particles=p; }

	/// \brief Register the terrain contact in the solver
	inline void registerTerrainContact(TerrainContact* terrainContact){ this->terrainContact=terrainContact; }

protected:

	Mesh* mesh; //!< pointer to mesh
	vector<Body*>* bodies; //!< pointer to bodies
	vector<Particle*>* particles; //!< pointer to particles
	TerrainContact* terrainContact; //!< pointer to terrain contact
	BoundingBox* boundingBox = new BoundingBox(); //particles bounding box
};

inline Solver::Solver():mesh(0),bodies(0),particles(0),terrainContact(0) { }
inline Solver::~Solver() { }

#endif /* SOLVER_H_ */