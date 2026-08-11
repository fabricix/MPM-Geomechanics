// SPDX-License-Identifier: MIT
// Copyright (c) 2021-202 MPM-Geomechanics Development Team

#ifndef INC_MATERIALS_CAMClAY_H_
#define INC_MATERIALS_CAMClAY_H_

#include <Materials/ElasticJaumann.h>

/// \class CamClay
/// \brief Represents an elasto-plastic material using the CamClay failure criteria 
class CamClay: public ElasticJaumann {
public:

    /// \brief Create a CamClay elasto-plastic material
    
    /// \param[in] id Material identification
    
    /// \param[in] density Material density \f$\rho\f$
    /// \param[in] young Young's modulus \f$E\f$
    /// \param[in] poisson Poisson's ratio \f$\nu\f$
    
    /// \param[in] lamda, l,compresion index
    /// \param[in] kappa, k, sweelling index
    /// \param[in] Mc, slope of critical line (p-q) for compression
    /// \param[in] Me, slope of critical line (p-q) in extension 

    CamClay(int id, double density, double young, double poisson);

    /// \brief Update the stress in the particle
    /// \param[in] particle Particle
    virtual void updateStress(Particle* particle) const;

    /// \brief Default destructor
    /// 
	virtual ~CamClay();

protected:
    double lambda; //!< compression index
    double kappa; //!< sweelling index
    double Mc; //!< critital state line slope for compression
    double Me; //!< critical state line slope for extension
};

#endif /* INC_MATERIALS_CAMClAY_H_ */
