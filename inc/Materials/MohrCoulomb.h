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

    // softening struct
    class Softening {

    public :

        enum SofteningType { NONE, EXPONENTIAL };

        // softening type
        SofteningType softening_type;

        // exponential softening
        double exponential_shape_factor;

        // residual values
        bool friction_residual;
        bool cohesion_residual;
        bool tensile_residual;

        // parameters to be affected by the softening
        bool friction_softening_active;
        bool cohesion_softening_active;
        bool tensile_softening_active;

        /// \brief This function provide an exponential law used for softening, a reduction of the material strength parameters. This function has this form: \f$ y(x) = y_f-(y_i-y_f)e^{-\eta x}\f$.
        /// \param[in] x Independent variable \f$x\f$
        /// \param[in] eta Shape parameter \f$\eta\f$
        /// \param[in] y_i Initial value \f$y_i\f$
        /// \param[in] y_f Final value \f$y_f\f$
        /// \param[out] y_current Current value \f$y(x)\f$
        inline double exponentialSoftening (double x, double eta, double y_i, double y_f) const {return y_f+(y_i-y_f)*std::exp(-eta*x);};

        Softening() {

            // softening type
            this->softening_type = MohrCoulomb::Softening::SofteningType::NONE;

            // exponential softening
            this->exponential_shape_factor = 0.0;

            // residual values
            this->friction_residual = false;
            this->cohesion_residual = false;
            this->tensile_residual = false;

            // parameters to be affected by the softening
            this->friction_softening_active = false;
            this->cohesion_softening_active = false;
            this->tensile_softening_active = false;
        }
    } ;

    /// \brief Create a Mohr-Coulomb elasto-plastic material
    /// \param[in] id Material identification
    /// \param[in] density Material density \f$\rho\f$
    /// \param[in] young Young's modulus \f$E\f$
    /// \param[in] poisson Poisson's ratio \f$\nu\f$
    /// \param[in] friction Internal frictional angle in degree \f$\phi\f$
    /// \param[in] cohesion Cohesion parameter \f$c\f$
    /// \param[in] dilation Dilatant angle in degree \f$\psi\f$
    /// \param[in] tensile Tensile strength \f$\sigma_t\f$
    /// \param[in] softening; //!< softening type
    MohrCoulomb(int id, double density, double young, double poisson, double friction, double cohesion, double dilation, double tensile, MohrCoulomb::Softening softening);

    /// \brief Update the stress in the particle
    /// \param[in] particle Particle
    virtual void updateStress(Particle* particle) const;

    /// \brief Default destructor
    /// 
	virtual ~MohrCoulomb();

protected:

    double friction; //!< internal friction angle in degree, \f$\phi\f$
    double cohesion; //!< cohesion parameter, \f$c\f$
    double dilation; //!< dilation angle in degree, \f$\psi\f$
    double tensile; //!< tensile strength, \f$\sigma_t\f$
    MohrCoulomb::Softening softening; 
};

#endif /* INC_MATERIALS_MOHRCOULOMB_H_ */
