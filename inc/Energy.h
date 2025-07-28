// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#ifndef INC_ENERGY_H_
#define INC_ENERGY_H_

#include "Body/Body.h"

class Energy
{
public:

    static Energy& inst()
    {
        static Energy instance;
        return instance;
    }
    
    inline double getCurrentKineticEnergy() { return currentKineticEnergy; } //!< Get current kinetic energy
    inline void setCurrentKineticEnergy(double energy) { currentKineticEnergy = energy; } //<!< Set current kinetic energy
    inline double getLastKineticEnergy() { return lastKineticEnergy; } //!< Get last kinetic energy
    inline void setLastKineticEnergy(double energy) { lastKineticEnergy = energy; } //<!< Set last kinetic energy
    inline void setLastKineticEnergyAsCurrent() { lastKineticEnergy = currentKineticEnergy; } //<!< Set last kinetic energy as current
    
    void computeKineticEnergy(vector<Body*>* bodies); //!< Compute the kinetic energy of the bodies
    
private:

    double currentKineticEnergy = 0.0; //!< Current kinetic energy
    double lastKineticEnergy = 0.0; //!< Last kinetic energy

    Energy() = default; 
    Energy(const Energy&) = delete;
    Energy& operator=(const Energy&) = delete;

};

#endif /* INC_ENERGY_H_ */
