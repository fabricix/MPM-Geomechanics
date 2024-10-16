/*
 * Loads.cpp
 *
 *  Created on: 13 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include "Loads.h"
#include "Model.h"
#include "Geometry.h"
#include "Input.h"

namespace Loads 
{
	// list of particles with prescribed pore pressure
	vector<PrescribedPorePressure> prescribedPorePressureParticlesList;

	// seismic data
	SeismicData seismicRecord;
}

Loads::SeismicData& Loads::getSeismicData(){ return seismicRecord ;}

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

void Loads::setSeismicData()
{
	if(!ModelSetup::getSeismicAnalysis()) return;

	// set seismic data
	seismicRecord = Input::readSeismicData(ModelSetup::getSeismicFileName(),false);
}