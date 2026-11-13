// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#include "Loads.h"
#include "Model.h"
#include "Geometry.h"
#include "Input.h"
#include "Warning.h"
#include <cmath>
#include <algorithm>
#include <stdexcept>

namespace Loads 
{
	vector<PrescribedPorePressure> prescribedPorePressureParticlesList;
	NodalPointLoadData nodalPointLoadList;
    vector<PrescribedVelocity> prescribedVelocities;
}

vector<Loads::PrescribedVelocity>& Loads::getPrescribedVelocities() { return prescribedVelocities; }

void Loads::resetPrescribedVelocityReactions() {
    for (auto& condition : prescribedVelocities) {
        condition.nodalReactionForces.clear();
    }
}

void Loads::setPrescribedVelocity(vector<Body*>& bodies) {
    prescribedVelocities.clear();
    const auto& input = Input::getJson();
    if (!input.contains("prescribed_velocity")) return;
    if (ModelSetup::getTwoPhaseActive())
        throw std::runtime_error("prescribed_velocity currently supports single-phase USL/MUSL only");
    const auto& entries = input.at("prescribed_velocity");
    if (!entries.is_array()) throw std::runtime_error("prescribed_velocity must be an array");
    for (const auto& entry : entries) {
        PrescribedVelocity condition;
        const auto& velocity = entry.at("velocity");
        if (!velocity.is_array() || velocity.size() != 3)
            throw std::runtime_error("prescribed_velocity requires three velocity components (number or null)");
        bool any = false;
        for (int axis = 0; axis < 3; ++axis) {
            if (velocity[axis].is_null()) continue;
            if (!velocity[axis].is_number() || !std::isfinite(velocity[axis].get<double>()))
                throw std::runtime_error("Invalid prescribed velocity component");
            condition.constrained[axis] = true;
            condition.velocity[axis] = velocity[axis].get<double>();
            any = true;
        }
        const bool box = entry.contains("point_p1") || entry.contains("point_p2");
        if (!any || (!box && !entry.contains("body_id")))
            throw std::runtime_error("prescribed_velocity needs a body_id or box and a constrained component");
        Vector3d p1 = Vector3d::Zero(), p2 = Vector3d::Zero();
        if (box) {
            for (const auto* key : {"point_p1", "point_p2"})
                if (!entry.contains(key) || !entry[key].is_array() || entry[key].size() != 3)
                    throw std::runtime_error("prescribed_velocity box requires point_p1 and point_p2");
            for (int axis = 0; axis < 3; ++axis) {
                p1[axis] = entry["point_p1"][axis].get<double>();
                p2[axis] = entry["point_p2"][axis].get<double>();
                if (!std::isfinite(p1[axis]) || !std::isfinite(p2[axis]) || p1[axis] > p2[axis])
                    throw std::runtime_error("Invalid prescribed_velocity box bounds");
            }
        }
        for (auto* body : bodies) {
            if (entry.contains("body_id") && body->getId() != entry["body_id"].get<int>()) continue;
            for (auto* p : *body->getParticles()) {
                const auto& x = p->getInitialPosition();
                if (box && ((x.array() < p1.array()).any() || (x.array() > p2.array()).any())) continue;
                for (const auto& previous : prescribedVelocities) {
                    if (std::find(previous.particles.begin(), previous.particles.end(), p) == previous.particles.end()) continue;
                    for (int axis = 0; axis < 3; ++axis)
                        if (previous.constrained[axis] && condition.constrained[axis] &&
                            previous.velocity[axis] != condition.velocity[axis])
                            throw std::runtime_error("Conflicting prescribed velocities on a particle");
                }
                auto value = p->getVelocity();
                for (int axis = 0; axis < 3; ++axis)
                    if (condition.constrained[axis]) value[axis] = condition.velocity[axis];
                p->setVelocity(value);
                condition.particles.push_back(p);
            }
        }
        if (condition.particles.empty()) throw std::runtime_error("prescribed_velocity selects no particles");
        prescribedVelocities.push_back(condition);
    }
}

Loads::NodalPointLoadData& Loads::getNodalPointList() { return nodalPointLoadList; }

void Loads::setGravity(vector<Body*>& bodies) {

	// return if gravity is not activated
	if (!ModelSetup::getGravityActive()) { return; }

	// get gravity
	Vector3d gravity = ModelSetup::getGravity();

	// is two-phase calculations 
	bool isTwoPhase = ModelSetup::getTwoPhaseActive();

	// for each body
	for (size_t ibody = 0; ibody < bodies.size(); ++ibody){

		// get particles
		vector<Particle*>* particles = bodies.at(ibody)->getParticles();

		// for each particle 
		for (size_t i = 0; i < particles->size(); ++i) {

			// get particle handle
			Particle* particle = particles->at(i);

			// compute the gravitational force of solid
			particle->addExternalForce(particle->getMass()*gravity);

			if (isTwoPhase)
			{
				// body force of fluid to solid phase
				particle->addExternalForce(particle->getMassFluid()*gravity);

				// body force of fluid in fluid phase
				particle->addExternalForceFluid(particle->getMassFluid()*gravity);
			}
		}
	}
}

void Loads::setLoadDistributedBox(vector<Body*>& bodies, vector<Loads::LoadDistributedBox> loads) {

	// for each load box
	for (size_t iload = 0; iload < loads.size(); ++iload)
	{
		// get total particle in the box 
		int totalParticles = 0;
		for (size_t ibody = 0; ibody < bodies.size(); ++ibody){

			// get particles
			vector<Particle*>* particles = bodies.at(ibody)->getParticles();

			// for each particle 
			for (size_t i = 0; i < particles->size(); ++i) {

				// verify if the particle is inside the box and count total particles
				if(Geometry::getInsideBox(loads.at(iload).pointP1, loads.at(iload).pointP2, particles->at(i)->getPosition())){

					totalParticles++;
				}
			}
		}

		// distributed load between  particles
		for (size_t ibody = 0; ibody < bodies.size(); ++ibody){

			// get particles
			vector<Particle*>* particles = bodies.at(ibody)->getParticles();

			// for each particle 
			for (size_t i = 0; i < particles->size(); ++i) {

				// distributed the load in particles inside the box
				if(Geometry::getInsideBox(loads.at(iload).pointP1, loads.at(iload).pointP2, particles->at(i)->getPosition()))
				{
					particles->at(i)->addExternalForce(loads.at(iload).load/totalParticles);
				}
			}
		}
	}
}

void Loads::setPrescribedPorePressureBox(vector<Body*>& bodies, vector<Loads::PressureBox> loads) {

	// for each pressure box
	for (size_t iload = 0; iload < loads.size(); ++iload)
	{
		// for each body
		for (size_t ibody = 0; ibody < bodies.size(); ++ibody){

			// get particles
			vector<Particle*>* particles = bodies.at(ibody)->getParticles();

			// for each particle 
			for (size_t ipart = 0; ipart < particles->size(); ++ipart) {

				// verify if particle is inside the box and store its index for set up the pressure during simulation
				if(Geometry::getInsideBox(loads.at(iload).pointP1, loads.at(iload).pointP2, particles->at(ipart)->getPosition()))
				{
					// create the particle list
					Loads::prescribedPorePressureParticlesList.push_back(Loads::PrescribedPorePressure(static_cast<unsigned int>(ibody), static_cast<unsigned int>(ipart),loads.at(iload).pressure));

					// set the pressure in particle
					particles->at(ipart)->setPressureFluid(loads.at(iload).pressure);
				}
			}
		}
	}
}

void Loads::setPrescribedPorePressureBoundaryForceBox(vector<Body*>& bodies, vector<Loads::PressureBoundaryForceBox> loads) {

	// for each pressure force box
	for (size_t iload = 0; iload < loads.size(); ++iload)
	{
		// for each body
		for (size_t ibody = 0; ibody < bodies.size(); ++ibody){

			// get particles
			vector<Particle*>* particles = bodies.at(ibody)->getParticles();

			// for each particle 
			for (size_t ipart = 0; ipart < particles->size(); ++ipart) {

				// verify is particle is inside the box and configure the pressure force
				if(Geometry::getInsideBox(loads.at(iload).pointP1, loads.at(iload).pointP2, particles->at(ipart)->getPosition()))
				{
					// set external boundary condition of fluid phase
					bodies.at(ibody)->getParticles()->at(ipart)->addExternalForceFluid(-loads.at(iload).pressureForce);
				}
			}
		}
	}
}

void Loads::updatePrescribedPorePressure(vector<Body*>* bodies) {

	// for each particle in prescribed pressure list
	for (size_t i = 0; i < Loads::prescribedPorePressureParticlesList.size(); ++i)
	{	
		// get bodies and particles indexes and pressure value
		const int ibody = Loads::prescribedPorePressureParticlesList.at(i).bodyIndex;
		const int ipart = Loads::prescribedPorePressureParticlesList.at(i).particleIndex;
		const double ipressure = Loads::prescribedPorePressureParticlesList.at(i).pressure;

		// setup pressure at particle
		bodies->at(ibody)->getParticles()->at(ipart)->setPressureFluid(ipressure);
	}
}

void Loads::configureNodalPointLoads(Mesh* mesh)
{
	Loads::NodalPointLoadData loadlist = Input::readNodalPointLoads();

	for (auto& ipoint : loadlist.points)
	{	
		double min_distance = 1e10;
		int node_id = -1;
		size_t ipoint_indx = 0;

		// for each load point calculate the min distance inside the nodes
		for (const auto& inode : mesh->getNodesInCell(ipoint))
		{	
			Vector3d xI = mesh->getNodes()->at(inode)->getCoordinates();
			
			double new_distance = (xI - ipoint).norm();

			if (new_distance <= min_distance)
			{
				min_distance = new_distance;
				node_id = mesh->getNodes()->at(inode)->getId();
			}
		}

		if (node_id==-1)
			Warning::printMessage("Bad point load node id configuration");
		else
			loadlist.nodal_ids.at(ipoint_indx)=node_id;
	}
	// set nodal-id load vector
	nodalPointLoadList = loadlist;
}

void Loads::setInitialPorePressureBox(vector<Body*>& bodies, vector<Loads::PressureBox> loads) {

	// for each pressure box
	for (size_t iload = 0; iload < loads.size(); ++iload)
	{
		// for each body
		for (size_t ibody = 0; ibody < bodies.size(); ++ibody){

			// get particles
			vector<Particle*>* particles = bodies.at(ibody)->getParticles();

			// for each particle 
			for (size_t i = 0; i < particles->size(); ++i) {

				// verify is the particle is inside the box and configure the pressure
				if(Geometry::getInsideBox(loads.at(iload).pointP1, loads.at(iload).pointP2, particles->at(i)->getPosition()))
				{
					particles->at(i)->setPressureFluid(loads.at(iload).pressure);
				}
			}
		}
	}
}

void Loads::setInitialPorePressureMaterial(vector<Body*>& bodies, vector<Loads::PressureMaterial> loads) {

	// for each load box
	for (size_t iload = 0; iload < loads.size(); ++iload)
	{
		// get material
		int materialId = loads.at(iload).materialId;
		
		// get pressure
		double pressure = loads.at(iload).pressure;

		// for each material 
		for (size_t ibody = 0; ibody < bodies.size(); ++ibody){

			// get particles
			vector<Particle*>* particles = bodies.at(ibody)->getParticles();

			// verify the material of each particle and setup the pressure 
			for (size_t i = 0; i < particles->size(); ++i) {

				if (particles->at(i)->getMaterialId()==materialId){

					particles->at(i)->setPressureFluid(pressure);
				}
				
			}
		}
	}
}

void Loads::setInitialVelocity(vector<Body*>& bodies) {

	// for each body
	for (auto& body : bodies) {

		// get particles
		auto* particles = body->getParticles();

		// apply initial velocity
		for (auto& particle : *particles) {
			particle->setVelocity(body->getInitialVelocity());
		}
	}
}
