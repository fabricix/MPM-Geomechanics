// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#ifndef INC_ENERGY_H_
#define INC_ENERGY_H_

#include "Body/Body.h"

class Energy
{

  private:
  double currentKineticEnergy = 0.0;
  double lastKineticEnergy = 0.0;

  Energy() = default; 
  Energy(const Energy&) = delete;
  Energy& operator=(const Energy&) = delete;

  public:
  static Energy& inst()
  {
    static Energy instance;
    return instance;
  }

  inline double getCurrentKineticEnergy() { return currentKineticEnergy; }
  inline void setCurrentKineticEnergy(double energy) { currentKineticEnergy = energy; }

  inline double getLastKineticEnergy() { return lastKineticEnergy; }
  inline void setLastKineticEnergy(double energy) { lastKineticEnergy = energy; }

  void setLastKineticEnergyAsCurrent()
  {
    lastKineticEnergy = currentKineticEnergy;
  }

  void computeKineticEnergy(vector<Body*>* bodies);

};

#endif /* INC_ENERGY_H_ */
