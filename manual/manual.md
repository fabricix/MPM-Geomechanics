\page program_theory_manual Theory manual

**Manual Theory under development**

## Material Point Method (MPM)

### Introduction to MPM

The Material Point Method, or MPM, is a hybrid Lagrangian-Eulerian method that allows for simulating continuum mechanics processes involving large deformations and displacements without issues related to computational mesh distortion.
In MPM, the material domain to be simulated is discretized into a set of material points that can move freely within a computational mesh, where the equations of motion are solved.
The material points store all variables of interest during the simulation, such as stress, pore pressure, temperature, etc., giving the method its Lagrangian characteristic.

![](/manual/mpm_discretization.png)
Figure 1: MPM Discretization [2]

In an MPM computational cycle, all variables stored in the material points are computed at the computational mesh nodes using interpolation functions, and then the equation of motion is solved at the nodes. The nodal solution obtained is interpolated back to the particles, whose positions are updated, and all nodal variables are discarded.

![](/manual/mpm_cycle.png)
Figure 2: MPM Cycle [3]

### MPM Formulation

MPM enables the numerical solution of the equation of motion in continuum mechanics by using the nodes of an Eulerian mesh for integration and Lagrangian material points to transfer and store the properties of the medium.

The equation of motion in continuum mechanics

\f[
\frac{\partial \sigma_{ij}}{\partial x_j} + \rho b_i = \rho a_i
\f]

where \f$ \sigma_{ij} \f$ is the Cauchy stress tensor, \f$ \rho \f$ is the density, \f$ b_i \f$ is
the body force (regarding its mass), and \f$ a_i \f$ is the acceleration of any point of the continuum.

Note that all equations are in tensor notation. So \f$ a_i \f$ is the acceleration vector with tree dimension in the space \f$ x, y, z \f$.

The MPM formulation is obtained from the weak form of the motion equation and using a Petrov–Galerkin discretization scheme. The weak form of motion equation is obtained by multiplying the equation by arbitrary weighting functions, integrating this product over the domain, using integration by parts to reduce the order of the stress term and introducing the boundary conditions

\f[
    -\int_{\Omega} \sigma_{i j} \delta u_{i, j} dV + \int_{\Gamma} t_i \delta u_i dA + \int_{\Omega} \rho b_i \delta u_i dV = \int_{\Omega} \rho a_i \delta u_i dV
\f]

where \f$ \delta u_i \f$ are virtual displacements, whose value in the boundary is \f$ \delta u_i |_{\Gamma} = 0 \f$ and \f$ t_i \f$ is an external traction acting on the boundary \f$ \Gamma \f$.

In the MPM context any field or property \f$ f(x_i) \f$ can approximated using the value stored in the particle \f$ f_p \f$:

\f[
    f(x_i) = \sum f_p \chi_p (x_i)
\f]

where \f$ \chi_p \f$ is the particle characteristic function that defines the volume occupied by the material point:

\f[ 
    V_p = \int_{\Omega_p \cap \Omega} \chi_p(x_i) dV
\f]

consequently, in the MPM context, the density, the acceleration and the stress fields can be approximated by the values stored in particles:

\f[
\rho(x_i) = \sum_p \frac{m_p}{V_{ip}} \chi_p(x_i)
\f]

\f[
\rho(x_i) a_i(x_i) = \sum_p \frac{\dot{p_{ip}}}{V_p} \chi_p(x_i)
\f]

\f[
\sigma_{i j}(x_i) = \sum_p \sigma_{i j p} \chi_p(x_i)
\f]

where \f$ \dot{p_{i p}} = m_p \dot{v_{ip}} = m_p a_{ip} \f$.

Replacing these fields in the weak form of the motion equation we have:

\f[
-\sum_p \int_{\Omega_p \cap \Omega} \sigma_{i j p} \chi_p \delta u_{i, j} dV + \int_{\Gamma} t_i \delta u_i dA + \sum_p \int_{\Omega_p \cap \Omega} \frac{m_p}{V_p} b_{i p} \chi_p \delta u_i dV = \sum_p \int_{\Omega_p \cap \Omega} \frac{\dot{p}_p}{V_p} \chi_p a_i dV
\f]

In the generalized interpolation material point method (GIMP), the resolution of this equation is carried out using a Petrov–Galerkin scheme where the characteristic functions \f$ \chi_p(x_i) \f$ are the trial functions and the nodal interpolation functions \f$ N_I(x_i) \f$ are the test functions. 

To arrive at this scheme, the virtual displacements are expressed using the nodal interpolation functions:

\f[ \delta u_i=\sum_I N_{I p} \delta u_{i I} \f]

Note that the trial and test functions are such that:

\f[ \sum_I N_{I}(x_i) = 1 \f]

\f[ \sum_p \chi_p(x_i) = 1 \f]

Therefore, the resulting discrete form of the motion equation is:

\f[
f_{iI}^{int} + f_{iI}^{ext} = \dot{p}_{iI}
\f]

where 

\f$ p_{iI} = \sum_p S_{Ip} p_{Ip} \f$ is the nodal momentum,

\f$ f_{iI}^{int} = -\sum_p \sigma_{ijp} S_{Ip,j} V_p \f$ is the nodal internal force, and

\f$ f_{iI}^{ext} = \sum_p m_p S_{Ip} b_{ip} + \int_{\Gamma} t_i N_I(x_i) dA \f$ is the external force at node \f$ I \f$.

Note that the function \f$ S_{Ip} \f$ and its gradient \f$ S_{Ip,j} \f$ are the weighting functions of node \f$ I \f$ evaluated at the position of particle \f$ p \f$.

The GIMP shape functions are defined by 

\f[
S_{Ip} = \frac{1}{V_p} \int_{\Omega_p \cap \Omega} \chi_p(x_i) N_I(x_i) dV 
\f] 

and 

\f[ 
S_{Ip,j} = \frac{1}{V_p} \int_{\Omega_p \cap \Omega} \chi_p(x_i) N_{I,j}(x_i) dV
\f] 

Note that these functions is also a partition of the unity:

\f$ \sum_I S_{Ip} = 1 \f$

The the weighting function need to be integrated over
the particle domain by choosing different characteristic functions and interpolation functions in a Petrov–Galerkin scheme.

In the contiguous particle GIMP (cpGIMP) the characteristic function in defined as step function and the interpolation function is defined as linear function:

\f[ 
    \chi_p(x)=\left\{\begin{array}{cc}1 & x \in \Omega_p \\ 0 & x \notin \Omega_p\end{array}\right.
\f]

\f[
    N_I(x)=\left\{\begin{array}{c}0 \quad\left|x-x_I\right| \geq L \\ 1+\left(x-x_I\right) / L \quad-L<x-x_I \leq 0 \\ 1-\left(x-x_I\right) / L \quad 0<x-x_I<L\end{array}\right.$
\f]

Where the integration is performed analytically within the particle domain.

\f[
S_{I p}=\left\{\begin{array}{ll}0 & |\xi| \geq L+l_p \\ 
\left(L+l_p+\xi\right)^2 / 4 L l_p & -L-l_p<\xi \leq-L+l_p \\ 
1+\xi / L & -L+l_p<\xi \leq-l_p \\ 
1-\left(\xi^2+l_p^2\right) / 2 L l_p & \quad-l_p<\xi \leq l_p \\ 
1-\xi / L & l_p<\xi \leq L-l_p \\ 
\left(L+l_p-\xi\right)^2 / 4 L l_p & L-l_p<\xi \leq L+l_p\end{array}\right.
\f]

and

\f[
\nabla S_{I p}= \begin{cases}0 & \left|x_p-x_I\right| \geq L+l_p, \\ 
\frac{L+l_p+\left(x_p-x_I\right)}{2 L l_p} & -L-l_p<x_p-x_I \leq-L+l_p, \\ 
\frac{1}{L} & -L+l_p<x_p-x_I \leq-l_p, \\ 
-\frac{x_p-x_I}{L l_p} & -l_p<x_p-x_I \leq l_p, \\ 
-\frac{1}{L} & l_p<x_p-x_I \leq L-l_p, \\ 
-\frac{L+l_p-\left(x_p-x_I\right)}{2 L l_p} & L-l_p<x_p-x_I \leq L+l_p .
\end{cases}
\f]

In which \f$ 2lp \f$ is the particle domain, \f$ L \f$ is the mesh size in 1D, and  is the relative particle position to node.

Weighting functions in 3D are obtained by the product of three one-dimensional weighting functions:

\f$
S_{I p}(x_{i p}) = S_{I p}(\xi) S_{I p} (\eta) S_{I p} (\zeta)
\f$

where \f$ \xi=x_p-x_I, \eta=y_p-y_I \f$ and \f$ \zeta=z_p-z \f$.

### Explicit MPM integration

The discrete form of the motion equation \f$
f_{iI}^{int} + f_{iI}^{ext} = \dot{p}_{iI} \f$, is a second order ordinary differential equation in terms of displacement with respect to time, and can be solved by integration, using an explicit or implicit integration scheme.

### Central difference Method  

The displacement, the velocity and the acceleration at time \f$ t = 0, t^1, t^2, ... , t^n \f$ are knows, and the solution at time \f$ t^{n+1} \f$ is required.

In central difference method, the velocity at \f$ t^{n+1/2} \f$ can be approximated as:

\f[ 
    \dot{u}^{n+1/2} = ( u^{n+1} - u^{n} ) \Delta t\ 
\f]

and, the acceleration in \f$ t^{n} \f$ can be approximated as:

\f[
    \ddot{u}^{n} =  (\dot{u}^{n+1/2} - \dot u ^ {n-1/2})/\Delta t
\f]

and therefore, the required displacement at \f$ t^{n+1} \f$ can be calculated as:

\f[

u^{n+1} = u^{n} + \dot u ^ {n+1/2} \, \Delta t

\f]

, where

\f[
\dot u ^ {n+1/2} = \dot u ^ {n-1/2} + \ddot u ^ {n}  \Delta t
\f]

The motion equation in \f$ t^{n} \f$ is:

\f[
m \, \ddot u ^{n} = f ^{n}
\f]

So,

\f[
\ddot u ^{n} = f ^{n} / m
\f]

and
\f[
\dot u ^ {n+1/2} = \dot u ^ {n-1/2} + f ^{n} / m \, \Delta t
\f]

### Numerical implementation  

1. Calculate force \f$ f_n \f$
2. Calculate acceleration \f$ \ddot u ^{n} = f ^{n} / m \f$
3. Impose essential boundary conditions in terms of \f$ \ddot u ^{n} \f$
4. Update velocity \f$ \dot u ^ {n+1/2} = \dot u ^ {n-1/2} + \ddot u ^{n} \, \Delta t \f$
5. Update position \f$ u^{n+1} = u^{n} + \dot u ^ {n+1/2} \, \Delta t \f$
6. Let \f$ n = n + 1 \f$

### Stability Requirement
The central difference method is conditionally stable, so the time step must be less that a certain value. For linear systems this critical time step value depends on the natural period of the system, in particular for undamped linear systems the critical time step is:

\f[ \Delta t_{cr} = T_n \f]

\f$ T_n \f$ is the smallest natural period of the system. For finite element method the critical time step of the central difference method can be expressed as:

\f[ \Delta t_{cr} = min_e [ l^e / c ] \f]

, where \f$ l^e \f$ is the characteristic length of the element and \f$ c \f$ is the sound speed. This time step restriction implies that time step has to be limited such that a disturbance can travel across the smallest characteristic element length withing a single time step. This condition is known as CFL condition, or Courant-Friedrichs-Lewy condition. For linear elastic material the sound speed is:

\f[ c = \sqrt{ \frac {E (1-\nu)} {(1+\nu)(1-2\nu)\rho}}  \f]

In the MPM context the particles can has velocities in the begin ing of any time step, so the critical time speed can be written as:

\f[ \Delta t_{cr} = l^e / max_p (c_p + |u_p| ) ]\f

In a structured regular mesh, \f[l^e]\f is the grid cell dimension.

# References
- Zhang, X., Chen, Z., & Liu, Y. (2017). The material point method : a continuum-based particle method for extreme loading cases (First edition). Elsevier. http://site.ebrary.com/id/11285709

- Bathe, K.-J., & Bathe, K.-J. (1996). Finite element procedures. Prentice Hall.
