# Material Contact Verification {#material_contact}

## Block sliding on an inclined plane

**Setup**

A cuboid block (\f$10 \times 10 \times 10\text{ m}\f$) slides down a \f$45^\circ\f$ inclined plane simulated by tilting gravity to \f$ (7.07, 0.0, -7.07)\text{ m/s}^2 \f$. The material parameters are \f$E = 100\text{ MPa} \f$, \f$\rho = 2500\text{ kg/m}^3\f$, \f$\nu = 0.3\f$, cell size \f$1\text{ m}\f$, and friction coefficients \f$\mu \in \{0.0, 0.1, 0.3, 0.5\}\f$.

@image html model-stl-contact-verification.png "STL contact model" width=350px

@image html model-material-contact-verification.png "Material contact model" width=350px

**Analytical Solution**

The analytical solution for tangential displacement is:

\f[ u(t) = \frac{1}{2} \left( g \sin\theta - \mu g \cos\theta \right) t^2 \f]

**Results**

@image html results-contact-verification.png "Material contact verification" width=350px

## Multi-Block Sliding

**Setup**
  
Two blocks (\f$5 \times 10 \times 5\text{ m}\f$) slide simultaneously on the same \f$45^\circ\f$ inclined plane with different friction coefficients (\f$\mu = 0.2\f$ for Body 2 and \f$\mu = 0.0\f$ for Body 3).

**Results** 

@image html results-multi-material-contact-verification.png "Multi contact verification" width=350px

## Falling Blocks Impact for Real Distance Correction
**Setup**
 
Two blocks positioned \f$6\text{ m}\f$ and \f$2\text{ m}\f$ above a fixed base fall without gravity at constant initial velocities of \f$4\text{ m/s}\f$ and \f$2\text{ m/s}\f$, yielding exact analytical impact times of \f$t = 1.5\text{ s}\f$ and \f$t = 1.0\text{ s}\f$.

**Results**

Omitting the real-distance correction triggers premature contact when the gap equals the grid cell size (\f$1\text{ m}\f$). Activating `"real_distance_correction_coefficient": 0.5` accurately captures the exact impact times and velocity reversals.

@image html results-real-distance-correction-verification.png "Displacement comparison for interpenetration using real distance correction." width=350px