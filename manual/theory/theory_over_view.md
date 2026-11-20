# Theory and Formulation {#theory_overview}

This section describes the mathematical foundations, spatial discretization, and time-integration schemes implemented in **MPM-Geomechanics**.

* @ref mpm_intro
* @ref mpm_formulation
* @ref mpm_integration
* @ref standard_mpm_formulation
* @ref gimp_formulation
* @ref mpm_integration
* @ref mpm_damping
  
---
  
## Introduction to the Material Point Method (MPM) {#mpm_intro}

The Material Point Method (MPM) is a particle-based, meshfree numerical method well-suited for geomechanics problems involving large deformations, history-dependent materials, and multi-body contact.

In MPM, a continuum body is discretized into a set of Lagrangian material points (particles) that carry all physical properties—such as mass, volume, position, velocity, stress, strain, and internal state variables. Simultaneously, a background Eulerian computational grid is used to solve the governing equations of motion. At the end of each time step, the deformed grid is discarded and reset, eliminating the mesh entanglement and distortion issues typical of the Finite Element Method (FEM) under large deformations.

@image html mpm_cycle.png "MPM computational cycle" width=350px

A computational MPM cycle consists in a) interpolate values from particles to nodes, b) solve equations in nodes, d) interpolate solution from nodes to particles, and c) update particle's position.

## MPM Formulation {#mpm_formulation}

#### Motion Equation and Weak Form

The Material Point Method formulation is based on the 3D continuum mechanics equation of motion:

\f[ \frac{\partial \sigma_{ij}}{\partial x_j} + \rho b_i = \rho a_i \f]

where \f$\sigma_{ij}\f$ is the Cauchy stress tensor, \f$\rho\f$ is the mass density, \f$b_i\f$ is the body force, and \f$a_i\f$ is the acceleration. Multiplying by arbitrary virtual displacements \f$\delta u_i\f$ (\f$\delta u_i|_{\Gamma} = 0\f$) and integrating by parts yields the weak form:

\f[ -\int_{\Omega} \sigma_{ij} \delta u_{i,j} \, dV + \int_{\Gamma} t_i \delta u_i \, dA + \int_{\Omega} \rho b_i \delta u_i \, dV = \int_{\Omega} \rho a_i \delta u_i \, dV \f]

where \f$t_i\f$ represents external tractions acting on boundary \f$\Gamma\f$.

## Standard Linear MPM Formulation (sMPM) {#standard_mpm_formulation}

In the standard Material Point Method (sMPM), the continuum domain is discretized into a set of Lagrangian material points (particles) carrying mass \f$m_p\f$, volume \f$V_p\f$, velocity \f$\mathbf{v}_p\f$, and Cauchy stress \f$\boldsymbol{\sigma}_p\f$, while the equations of motion are solved on a background Eulerian grid.

### Dirac-Delta Particle Approximation
Unlike GIMP, standard MPM assumes each particle is concentrated at a single point \f$\mathbf{x}_p\f$ using Dirac delta functions \f$\delta(\mathbf{x} - \mathbf{x}_p)\f$:

\f[
\rho(\mathbf{x}) = \sum_p m_p \delta(\mathbf{x} - \mathbf{x}_p), \quad \boldsymbol{\sigma}(\mathbf{x}) = \sum_p \boldsymbol{\sigma}_p V_p \delta(\mathbf{x} - \mathbf{x}_p)
\f]

### Linear Nodal Shape Functions
The nodal weighting functions \f$S_{Ip}\f$ and their gradients \f$\nabla S_{Ip}\f$ reduce directly to the standard piecewise-linear finite element shape functions \f$N_I(\mathbf{x})\f$ evaluated at the particle position \f$\mathbf{x}_p\f$:

\f[
S_{Ip} = N_I(\mathbf{x}_p), \quad \nabla S_{Ip} = \nabla N_I(\mathbf{x}_p)
\f]

In 1D, for a regular grid spacing \f$L\f$ and relative coordinate \f$\xi = x_p - x_I\f$:

\f[
N_I(x_p) = 
\begin{cases} 
1 - \dfrac{|\xi|}{L}, & |\xi| < L \\ 
0, & |\xi| \ge L 
\end{cases}
\f]

In 3D, the shape function is the tensor product \f$N_I(\mathbf{x}_p) = N_I(x_p) N_I(y_p) N_I(z_p)\f$.

### Discrete Nodal Equations of Motion
Substituting the Dirac-delta fields and linear shape functions into the weak form yields the discrete momentum balance at grid node \f$I\f$:

\f[
\dot{p}_{iI} = f_{iI}^{int} + f_{iI}^{ext}
\f]

where:
* **Lumped nodal mass:** \f$m_I = \sum_p m_p N_I(\mathbf{x}_p)\f$
* **Nodal momentum:** \f$p_{iI} = \sum_p m_p v_{ip} N_I(\mathbf{x}_p)\f$
* **Internal nodal force:** \f$f_{iI}^{int} = -\sum_p \sigma_{ijp} N_{I,j}(\mathbf{x}_p) V_p\f$
* **External nodal force:** \f$f_{iI}^{ext} = \sum_p m_p b_{ip} N_I(\mathbf{x}_p) + \int_{\Gamma} t_i N_I(\mathbf{x}) \, dA\f$

### Grid-to-Particle Kinematics & Strain Rate
Once nodal accelerations \f$a_{iI} = \dot{p}_{iI} / m_I\f$ and updated velocities \f$v_{iI}^{n+1}\f$ are computed on the grid, particle kinematics and strain increments are interpolated back using the same linear functions:

\f[
v_{ip}^{n+1} = v_{ip}^n + \Delta t \sum_I a_{iI}^n N_I(\mathbf{x}_p^n), \quad \mathbf{x}_p^{n+1} = \mathbf{x}_p^n + \Delta t \sum_I \mathbf{v}_I^{n+1} N_I(\mathbf{x}_p^n)
\f]

\f[
\Delta \varepsilon_{ijp} = \frac{\Delta t}{2} \sum_I \left( v_{iI}^{n+1} N_{I,j}(\mathbf{x}_p^n) + v_{jI}^{n+1} N_{I,i}(\mathbf{x}_p^n) \right)
\f]

## Generalized Interpolation Material Point Method -- GIMP {#gimp_formulation}

In the GIMP formulation any spatial field \f$f(x_i)\f$ is approximated using the values stored at the particles \f$f_p\f$ and the particle characteristic function \f$\chi_p(x_i)\f$:

\f[ f(x_i) = \sum_p f_p \chi_p(x_i), \quad V_p = \int_{\Omega_p \cap \Omega} \chi_p(x_i) \, dV \f]

Using a Petrov–Galerkin scheme with nodal test functions \f$N_I(x_i)\f$, the discrete equation of motion at grid node \f$I\f$ becomes:

\f[ f_{iI}^{int} + f_{iI}^{ext} = \dot{p}_{iI} \f]

where:
* **Nodal momentum:** \f$p_{iI} = \sum_p S_{Ip} p_{ip}\f$
* **Nodal internal force:** \f$f_{iI}^{int} = -\sum_p \sigma_{ijp} S_{Ip,j} V_p\f$
* **Nodal external force:** \f$f_{iI}^{ext} = \sum_p m_p S_{Ip} b_{ip} + \int_{\Gamma} t_i N_I(x_i) \, dA\f$

The Generalized Interpolation Material Point (GIMP) weighting functions \f$S_{Ip}\f$ and their gradients \f$S_{Ip,j}\f$ are defined as:

\f[ S_{Ip} = \frac{1}{V_p} \int_{\Omega_p \cap \Omega} \chi_p(x_i) N_I(x_i) \, dV, \quad S_{Ip,j} = \frac{1}{V_p} \int_{\Omega_p \cap \Omega} \chi_p(x_i) N_{I,j}(x_i) \, dV \f]

In the contiguous particle GIMP (**cpGIMP**) approach, \f$\chi_p\f$ is a step function over the particle domain (\f$2l_p\f$) and \f$N_I\f$ is a piecewise linear hat function over cell size \f$L\f$. In 3D, the weighting function is constructed as the tensor product of three 1D functions:

\f[ S_{Ip}(x_{ip}) = S_{Ip}(\xi) S_{Ip}(\eta) S_{Ip}(\zeta) \f]

with relative coordinates \f$\xi = x_p - x_I\f$, \f$\eta = y_p - y_I\f$, and \f$\zeta = z_p - z_I\f$.

## Time Integration {#mpm_integration}

This section describes explicit time integration for motion equation.

### Explicit Central Difference Method

To advance the discrete equation of motion from time \f$t^n\f$ to \f$t^{n+1}\f$, the explicit scheme computes the updated state strictly from known quantities at step \f$t^n\f$.

Using central differences, nodal velocity at half-step \f$t^{n+1/2}\f$ and nodal acceleration at \f$t^n\f$ are approximated as:

\f[ \dot{u}^{n+1/2} = \frac{u^{n+1} - u^n}{\Delta t}, \quad \ddot{u}^n = \frac{\dot{u}^{n+1/2} - \dot{u}^{n-1/2}}{\Delta t}\f]

From the equation of motion \f$\ddot{u}^n = f^n / m\f$, the velocity and displacement updates are:

\f[ \dot{u}^{n+1/2} = \dot{u}^{n-1/2} + \frac{f^n}{m} \Delta t \f]

\f[ u^{n+1} = u^n + \dot{u}^{n+1/2} \Delta t\f]


#### Stability Requirement (CFL Condition)
Because the central difference scheme is conditionally stable, the time step \f$\Delta t\f$ must satisfy the Courant–Friedrichs–Lewy (CFL) condition so that a mechanical wave does not travel across more than one grid cell in a single step:

\f[ \Delta t_{cr} = \frac{l^e}{\max_p \left( c_p + |v_p| \right)}\f]

where \f$l^e\f$ is the grid cell dimension, \f$|v_p|\f$ is the particle velocity magnitude, and \f$c_p\f$ is the elastic compression (P-wave) speed:

\f[ c = \sqrt{\frac{E(1-\nu)}{(1+\nu)(1-2\nu)\rho}}\f]

#### Explicit MPM Computational Cycle {#mpm_explicit_algorithm}
Within each time step \f$\Delta t\f$, the explicit algorithm executes the following sequence (Update Stress Last - USL):

1. **Reset Grid:** Clear nodal mass, momentum, and force arrays from the previous step.
2. **Particle-to-Grid (P2G):** Map particle masses and momenta to background grid nodes using GIMP weighting functions \f$S_{Ip}\f$.
3. **Nodal Forces:** Compute internal forces (\f$f_{iI}^{int}\f$), external forces (\f$f_{iI}^{ext}\f$), and damping forces (\f$f_{iI}^{dnplocal}\f$).
4. **Integrate Grid Momentum:** Update nodal momenta \f$\dot{p}_{iI}\f$ and apply essential boundary conditions and multi-body contact corrections.
5. **Grid-to-Particle (G2P):** Interpolate nodal accelerations and velocities back to update particle velocities \f$v_{ip}^{n+1}\f$ and positions \f$x_{ip}^{n+1}\f$.
6. **Constitutive Update:** Compute strain increments \f$\Delta \varepsilon_{ijp}\f$ at particles and update stresses \f$\sigma_{ijp}^{n+1}\f$ and state variables via the constitutive model.

## Numerical Damping {#mpm_damping}

To dissipate non-physical numerical oscillations or achieve static equilibrium in dynamic simulations, **MPM-Geomechanics** incorporates numerical damping mechanisms (such as local non-viscous damping and kinetic damping) applied directly to the nodal equations of motion.

### Local Damping {#mpm_quasi_static}

Local damping is used to obtain a quasi-static solution of the dynamic system by applying a viscous nodal force proportional and opposite to the nodal velocity at each time step:

\f[ f_{iI}^{dnplocal} = -\alpha \left| f_{iI}^{unb} \right| \hat{v}_{iI} \f]

where \f$\alpha\f$ is the local damping factor, \f$\hat{v}_{iI}\f$ is the unit nodal velocity vector, and the unbalanced nodal force is:

\f[ f_{iI}^{unb} = f_{iI}^{int} + f_{iI}^{ext} \f]

The discrete equation of motion with local viscous damping is therefore:

\f[ \dot{p}_{iI} = f_{iI}^{int} + f_{iI}^{ext} + f_{iI}^{dnplocal} \f]

### Kinetic Damping (Dynamic Relaxation) {#kinetic_damping}

Kinetic damping (also known as kinetic dynamic relaxation) is an energy-based dissipation technique used in explicit dynamic MPM simulations to obtain quasi-static equilibrium solutions without introducing artificial viscous damping coefficients.

The total kinetic energy \f$E_k^n\f$ of the discrete system at time step \f$t^n\f$ is computed by summing the contributions of all active material points \f$p\f$:

\f[ E_k^n = \frac{1}{2} \sum_{p \in \Omega_{\text{act}}} m_p \left\| \mathbf{v}_p^n \right\|^2 \f]

where \f$m_p\f$ is the particle mass and \f$\mathbf{v}_p^n = (v_{xp}^n, v_{yp}^n, v_{zp}^n)\f$ is the particle velocity vector at step \f$n\f$.

During the dynamic oscillation of an elastic or elastoplastic body toward static equilibrium, the system passes through its minimum potential energy configuration precisely when the total kinetic energy reaches a local maximum. At each time step, the kinetic energy increment is evaluated as:

\f[ \Delta E_k^n = E_k^n - E_k^{n-1} \f]

A local peak in kinetic energy is detected when the increment becomes negative:

\f[ \Delta E_k^n < 0 \f]

When this condition is triggered, all kinetic energy is instantaneously removed from the system by resetting the velocity of every active material point to zero:

\f[ \mathbf{v}_p^{n} = \mathbf{0} \quad \forall \, p \in \Omega_{\text{act}}, \qquad E_k^n \leftarrow 0 \f]

The simulation then continues from the current deformed configuration and stress state, repeating this peak-detection and velocity-reset cycle until residual kinetic energy peaks become negligible and static equilibrium is achieved.

---

#### Numerical Implementation Algorithm

Within each time step \f$t^n\f$, the `DynamicRelaxation::setStaticSolution` routine executes the following steps:

1. **Compute System Kinetic Energy:** Evaluate \f$E_k^n\f$ across all active particles and compute the increment \f$\Delta E_k^n = E_k^n - E_k^{n-1}\f$.
2. **Peak Detection:** Check whether \f$\Delta E_k^n < 0\f$.
3. **Velocity Reset:** If a peak is detected:
   * Set \f$\mathbf{v}_p = (0, 0, 0)\f$ for all active particles in parallel (`#pragma omp parallel for`).
   * Reset the current kinetic energy tracker \f$E_k^n = 0\f$.
4. **State History Update:** Store \f$E_k^{n-1} \leftarrow E_k^n\f$ for the next time step evaluation.

---

#### JSON Input Configuration

To activate kinetic dynamic relaxation in a simulation input file, specify the damping type in the solver setup:

```json
"damping": {
  "type": "kinetic"
}
```