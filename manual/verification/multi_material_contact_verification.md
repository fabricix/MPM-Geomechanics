# Multi-Material Contact Verification {#multi_material_contact}

## Verification Benchmarks

### 1. Single Block Sliding on an Inclined Plane
* **Setup:** A cuboid block ($10 \times 10 \times 10\text{ m}$) slides down a $45^\circ$ inclined plane ($32 \times 14 \times 2\text{ m}$) simulated by tilting gravity to $(7.07, 0.0, -7.07)\text{ m/s}^2$. Elastic parameters: $E = 100\text{ MPa}$, $\rho = 2500\text{ kg/m}^3$, $\nu = 0.3$, cell size $1\text{ m}$, and friction coefficients $\mu \in \{0.0, 0.1, 0.2, 0.3, 0.4, 0.5\}$.
* **Analytical Solution:**
  $$d(t) = \frac{1}{2} \left( g \sin\theta - \mu g \cos\theta \right) t^2$$
* **Results:** Numerical displacements show close agreement with the analytical solution across all friction values.

@image html single_contact_verification.png "Verification of displacements obtained with MPM simulation (single contact)."

### 2. Multi-Block Sliding (Multi-Contact)
* **Setup:** Two blocks ($5 \times 10 \times 5\text{ m}$) slide simultaneously on the same $45^\circ$ inclined plane with different friction coefficients ($\mu = 0.2$ for Body 2 and $\mu = 0.0$ for Body 3).
* **Results:** Both blocks accurately reproduce their respective analytical displacement trajectories without cross-interference.

@image html multi_contact_verification.png "Verification of displacements obtained with MPM simulation (multi-contact)."

### 3. Falling Blocks Impact (Real Distance Correction)
* **Setup:** Two blocks positioned $6\text{ m}$ and $2\text{ m}$ above a fixed base fall without gravity at constant initial velocities of $4\text{ m/s}$ and $2\text{ m/s}$, yielding exact analytical impact times of $t = 1.5\text{ s}$ and $t = 1.0\text{ s}$.
* **Results:** Omitting the real-distance correction triggers premature contact when the gap equals the grid cell size ($1\text{ m}$). Activating `"real_distance_correction_coefficient": 0.5` accurately captures the exact impact times and velocity reversals.

@image html RealDistanceCorrection_Displacement.png "Displacement comparison with and without real distance correction."
@image html RealDistanceCorrection_Velocity.png "Velocity comparison with and without real distance correction."