
#include <string>
#include "Particle/Particle.h"

#ifndef STATES_H_
#define STATES_H_

namespace States {

	void saveParticleStress(const std::string& filename, const std::vector<Particle*>& particles);

	void loadParticleStress(const std::string& filename, std::vector<Particle*>& particles);
}

#endif /* STATES_H_ */