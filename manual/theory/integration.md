# Explicit Time Integration {#mpm_integration}

## Central Difference Method
To advance the discrete equation of motion from time $t^n$ to $t^{n+1}$, the explicit scheme computes the updated state strictly from known quantities at step $t^n$.

Using central differences, nodal velocity at half-step $t^{n+1/2}$ and nodal acceleration at $t^n$ are approximated as:

$$\dot{u}^{n+1/2} = \frac{u^{n+1} - u^n}{\Delta t}, \quad \ddot{u}^n = \frac{\dot{u}^{n+1/2} - \dot{u}^{n-1/2}}{\Delta t}$$

From the equation of motion $\ddot{u}^n = f^n / m$, the velocity and displacement updates are:

$$\dot{u}^{n+1/2} = \dot{u}^{n-1/2} + \frac{f^n}{m} \Delta t$$

$$u^{n+1} = u^n + \dot{u}^{n+1/2} \Delta t$$

## Stability Requirement (CFL Condition)
Because the central difference scheme is conditionally stable, the time step $\Delta t$ must satisfy the Courant–Friedrichs–Lewy (CFL) condition so that a mechanical wave does not travel across more than one grid cell in a single step:

$$\Delta t_{cr} = \frac{l^e}{\max_p \left( c_p + |v_p| \right)}$$

where $l^e$ is the grid cell dimension, $|v_p|$ is the particle velocity magnitude, and $c_p$ is the elastic compression (P-wave) speed:

$$c = \sqrt{\frac{E(1-\nu)}{(1+\nu)(1-2\nu)\rho}}$$

---

## Explicit MPM Computational Cycle {#mpm_explicit_algorithm}
Within each time step $\Delta t$, the explicit algorithm executes the following sequence (Update Stress Last - USL):

1. **Reset Grid:** Clear nodal mass, momentum, and force arrays from the previous step.
2. **Particle-to-Grid (P2G):** Map particle masses and momenta to background grid nodes using GIMP weighting functions $S_{Ip}$.
3. **Nodal Forces:** Compute internal forces ($f_{iI}^{int}$), external forces ($f_{iI}^{ext}$), and damping forces ($f_{iI}^{dnplocal}$).
4. **Integrate Grid Momentum:** Update nodal momenta $\dot{p}_{iI}$ and apply essential boundary conditions and multi-body contact corrections.
5. **Grid-to-Particle (G2P):** Interpolate nodal accelerations and velocities back to update particle velocities $v_{ip}^{n+1}$ and positions $x_{ip}^{n+1}$.
6. **Constitutive Update:** Compute strain increments $\Delta \varepsilon_{ijp}$ at particles and update stresses $\sigma_{ijp}^{n+1}$ and state variables via the constitutive model.