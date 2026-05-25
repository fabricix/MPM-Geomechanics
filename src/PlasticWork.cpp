// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#include "PlasticWork.h"

namespace PlasticWork
{
    double currentPlasticWork = 0.0; //!< Current plastic work

    const double getPlasticWork() { return currentPlasticWork; } //!< Get current plastic work
    void setPlasticWork(double wp) { currentPlasticWork+=wp; } //!< Set current plastic work

    void computePlasticWork(vector <Particle*>* particles)
    {
        double pwork = 0.0;
        const int np = static_cast<int>(particles->size());

#ifdef _OPENMP
        #pragma omp parallel for shared(particles) reduction(+:pwork) 
#endif
        for (int i = 0; i < np; ++i) 
        {
            Particle* p = (*particles)[i];

            if (!p->getActive()) continue;

            pwork += p->getPlasticWorkIncrement();
        }
        setPlasticWork(pwork);
    }
}