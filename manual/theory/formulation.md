# MPM and GIMP Formulation {#mpm_formulation}

## Motion Equation and Weak Form
The Material Point Method formulation is based on the 3D continuum mechanics equation of motion:

$$\frac{\partial \sigma_{ij}}{\partial x_j} + \rho b_i = \rho a_i$$

where $\sigma_{ij}$ is the Cauchy stress tensor, $\rho$ is the mass density, $b_i$ is the body force, and $a_i$ is the acceleration. Multiplying by arbitrary virtual displacements $\delta u_i$ ($\delta u_i|_{\Gamma} = 0$) and integrating by parts yields the weak form:

$$-\int_{\Omega} \sigma_{ij} \delta u_{i,j} \, dV + \int_{\Gamma} t_i \delta u_i \, dA + \int_{\Omega} \rho b_i \delta u_i \, dV = \int_{\Omega} \rho a_i \delta u_i \, dV$$

where $t_i$ represents external tractions acting on boundary $\Gamma$.

## GIMP Spatial Discretization
Any spatial field $f(x_i)$ is approximated using the values stored at the particles $f_p$ and the particle characteristic function $\chi_p(x_i)$:

$$f(x_i) = \sum_p f_p \chi_p(x_i), \quad V_p = \int_{\Omega_p \cap \Omega} \chi_p(x_i) \, dV$$

Using a Petrov–Galerkin scheme with nodal test functions $N_I(x_i)$, the discrete equation of motion at grid node $I$ becomes:

$$f_{iI}^{int} + f_{iI}^{ext} = \dot{p}_{iI}$$

where:
* **Nodal momentum:** $p_{iI} = \sum_p S_{Ip} p_{ip}$
* **Nodal internal force:** $f_{iI}^{int} = -\sum_p \sigma_{ijp} S_{Ip,j} V_p$
* **Nodal external force:** $f_{iI}^{ext} = \sum_p m_p S_{Ip} b_{ip} + \int_{\Gamma} t_i N_I(x_i) \, dA$

The Generalized Interpolation Material Point (GIMP) weighting functions $S_{Ip}$ and their gradients $S_{Ip,j}$ are defined as:

$$S_{Ip} = \frac{1}{V_p} \int_{\Omega_p \cap \Omega} \chi_p(x_i) N_I(x_i) \, dV, \quad S_{Ip,j} = \frac{1}{V_p} \int_{\Omega_p \cap \Omega} \chi_p(x_i) N_{I,j}(x_i) \, dV$$

In the contiguous particle GIMP (**cpGIMP**) approach, $\chi_p$ is a step function over the particle domain ($2l_p$) and $N_I$ is a piecewise linear hat function over cell size $L$. In 3D, the weighting function is constructed as the tensor product of three 1D functions:

$$S_{Ip}(x_{ip}) = S_{Ip}(\xi) S_{Ip}(\eta) S_{Ip}(\zeta)$$

with relative coordinates $\xi = x_p - x_I$, $\eta = y_p - y_I$, and $\zeta = z_p - z_I$.

---

## Damping Formulations {#mpm_damping}
To dissipate non-physical numerical oscillations or achieve static equilibrium in dynamic simulations, **MPM-Geomechanics** incorporates numerical damping mechanisms (such as local non-viscous damping and kinetic damping) applied directly to the nodal equations of motion.

---

## Quasi-Static Solution with Local Damping {#mpm_quasi_static}
Local damping is used to obtain a quasi-static solution of the dynamic system by applying a viscous nodal force proportional and opposite to the nodal velocity at each time step:

$$f_{iI}^{dnplocal} = -\alpha \left| f_{iI}^{unb} \right| \hat{v}_{iI}$$

where $\alpha$ is the local damping factor, $\hat{v}_{iI}$ is the unit nodal velocity vector, and the unbalanced nodal force is:

$$f_{iI}^{unb} = f_{iI}^{int} + f_{iI}^{ext}$$

The discrete equation of motion with local viscous damping is therefore:

$$\dot{p}_{iI} = f_{iI}^{int} + f_{iI}^{ext} + f_{iI}^{dnplocal}$$