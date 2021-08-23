/*
 * Loads.cpp
 *
 *  Created on: 13 de abr de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#include "Loads.h"
#include "Model.h"
#include "Geometry.h"

namespace Loads 
{
	vector<PressureParticleIndex> prescribedPorePressure;
}

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

			// compute the gravitational force
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
		double totalParticles = 0.0;
		for (size_t ibody = 0; ibody < bodies.size(); ++ibody){

			// get particles
			vector<Particle*>* particles = bodies.at(ibody)->getParticles();

			// for each particle 
			for (size_t i = 0; i < particles->size(); ++i) {

				if(Geometry::getInsideBox(loads.at(iload).pointP1, loads.at(iload).pointP2, particles->at(i)->getPosition()))
				{
					totalParticles++;
				}
			}
		}

		// add distributed load in particles
		for (size_t ibody = 0; ibody < bodies.size(); ++ibody){

			// get particles
			vector<Particle*>* particles = bodies.at(ibody)->getParticles();

			// for each particle 
			for (size_t i = 0; i < particles->size(); ++i) {

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
		// configures pressure in particles inside the box
		for (size_t ibody = 0; ibody < bodies.size(); ++ibody){

			// get particles
			vector<Particle*>* particles = bodies.at(ibody)->getParticles();

			// for each particle 
			for (size_t ipart = 0; ipart < particles->size(); ++ipart) {

				if(Geometry::getInsideBox(loads.at(iload).pointP1, loads.at(iload).pointP2, particles->at(ipart)->getPosition()))
				{
					Loads::prescribedPorePressure.push_back(Loads::PressureParticleIndex(ibody,ipart,loads.at(iload).pressure));
				}
			}
		}
	}
}

void Loads::updatePrescribedPorePressure(vector<Body*>& bodies) {

	for (size_t i = 0; i < Loads::prescribedPorePressure.size(); ++i)
	{
		const int ibody = Loads::prescribedPorePressure.at(i).bodyIndex;
		const int ipart = Loads::prescribedPorePressure.at(i).particleIndex;
		const double ipressure = Loads::prescribedPorePressure.at(i).pressure;

		bodies.at(ibody)->getParticles()->at(ipart)->setPressureFluid(ipressure);
	}
}

void Loads::setInitialPorePressureBox(vector<Body*>& bodies, vector<Loads::PressureBox> loads) {

	// for each pressure box
	for (size_t iload = 0; iload < loads.size(); ++iload)
	{
		// configures pressure in particles inside the box
		for (size_t ibody = 0; ibody < bodies.size(); ++ibody){

			// get particles
			vector<Particle*>* particles = bodies.at(ibody)->getParticles();

			// for each particle 
			for (size_t i = 0; i < particles->size(); ++i) {

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

		// set pressure in particles with material
		for (size_t ibody = 0; ibody < bodies.size(); ++ibody){

			// get particles
			vector<Particle*>* particles = bodies.at(ibody)->getParticles();

			// for each particle 
			for (size_t i = 0; i < particles->size(); ++i) {

				if (particles->at(i)->getMaterialId()==materialId){

					particles->at(i)->setPressureFluid(pressure);
				}
				
			}
		}
	}
}

void Loads::setPorePressureTraction(vector<Body*>& bodies) {

	for (size_t i = 0; i < Loads::prescribedPorePressure.size(); ++i)
	{
		// body index
		const int ibody = Loads::prescribedPorePressure.at(i).bodyIndex;
		
		// particle index
		const int iparticle = Loads::prescribedPorePressure.at(i).particleIndex;
		
		// pressure
		const double pressure = Loads::prescribedPorePressure.at(i).pressure;
		
		// particle size
		const Vector3d size = bodies.at(ibody)->getParticles()->at(iparticle)->getSize();

		// set external boudanry condition of fluid phase
		bodies.at(ibody)->getParticles()->at(iparticle)->addExternalForceFluid(-size*pressure);
	}
}
