/*
 * MohrCoulomb.h
 *
 *  Created on: 1 de jul de 2021
 *      Author: Fabricio Fernandez <fabricio.hmf@gmail.com>
 */

#ifndef INC_MATERIALS_MOHRCOULOMB_H_
#define INC_MATERIALS_MOHRCOULOMB_H_

#include <Materials/ElasticJaumann.h>

/// \class MohrCoulomb
/// \brief Represents an elasto-plastic material using the Mohr-Coulomb failure criteria 
class MohrCoulomb: public ElasticJaumann {
public:

    /// \brief Create a Mohr-Coulomb elasto-plastic material
    /// \param[in] id Material identification
    /// \param[in] density Material density \f$\rho\f$
    /// \param[in] young Young's modulus \f$E\f$
    /// \param[in] poisson Poisson's ratio \f$\nu\f$
    /// \param[in] friction Internal frictional angle in degree \f$\phi\f$
    /// \param[in] cohesion Cohesion parameter \f$c\f$
    /// \param[in] dilation Dilatancy angle in degree \f$\psi\f$
    /// \param[in] tensile Tensile strength \f$\sigma_t\f$
	MohrCoulomb(int id, double density, double young, double poisson, double friction, double cohesion, double dilation, double tensile);

    /// \brief Update the stress in the particle
    /// \param[in] particle Particle
    virtual void updateStress(Particle* particle) const;

    /// \brief Default destructor
    ///
	virtual ~MohrCoulomb();

    /// \brief exponential law used for softening \f$ y(x) = y_f-(y_i-y_f)e^{\eta x}\f$
    /// \param[in] x Independent variable \f$x\f$
    /// \param[in] eta Shape parameter \f$\eta\f$
    /// \param[in] y_initial Peak value \f$y_i\f$
    /// \param[in] y_final Residual value \f$y_f\f$
    /// \param[out] y_current Current value \f$y(x)\f$
    double exponentialLaw(double x, double eta, double y_initial, double y_final) const;

protected:

    double friction; //!< internal friction angle in degree, \f$\phi\f$
    double cohesion; //!< cohesion parameter, \f$c\f$
    double dilation; //!< dilation angle in degree, \f$\psi\f$
    double tensile; //!< tensile strength, \f$\sigma_t\f$
};

#endif /* INC_MATERIALS_MOHRCOULOMB_H_ */
