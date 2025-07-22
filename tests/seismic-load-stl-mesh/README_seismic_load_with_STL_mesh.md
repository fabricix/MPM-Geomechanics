# Seismic Loading with STL Contact – Validation Example

This example validates the implementation of seismic loading applied through **STL contact surfaces** within the **MPM-Geomechanics** simulator. The model setup, input parameters, and results are described below.

---

## 1. Model Description

The simulation consists of a **column resting on a terrain surface** represented by an STL mesh. The seismic motion is introduced by applying a **velocity field to marked seismic nodes**, which is derived from an external acceleration record.

---

## 2. JSON Configuration

The key parameters in the configuration file `seismic-column-stl-mesh.json` are:

- **Stress Update Scheme**:  
  `"stress_scheme_update": "MUSL"`

- **Time and Time Step**:  
  `"time": 10` (10 seconds of simulation),  
  `"time_step_multiplier": 0.2` (critical time step scaling).

- **Gravity**:  
  `"gravity": [0.0, 0.0, -9.81]` (m/s2).

- **Mesh**:  
  A regular Eulerian background grid with dimensions:  
  `"cells_number": [20, 10, 15]`  
  `"cells_dimension": [1, 1, 1]` (m).

- **Seismic Loading**:  
  ```
  "earthquake": {
    "active": true,
    "file": "base_acceleration_scaled.csv",
    "header": true
  }
  ```  
  The acceleration record (`base_acceleration_scaled.csv`) is integrated to compute nodal velocities for seismic nodes.

- **Terrain Contact**:  
  ```
  "terrain_contact": {
    "active": true,
    "stl_mesh": "terrain_mesh.stl",
    "friction": 0.1,
    "distance_threshold": 2.0
  }
  ```  
  - `friction`: Coulomb friction coefficient between column and terrain.
  - `distance_threshold`: Cell dimension scaling factor for detecting particle contact with the STL mesh.

- **Material Properties**:  
  ```
  "material": {
    "elastic_1": {
      "type": "elastic",
      "young": 10e6,
      "density": 2500,
      "poisson": 0.25
    }
  }
  ```

- **Body Definition**:  
  A cuboid column from `[8, 4, 5]` to `[10, 6, 8]` using `material_id: 1`.

---

## 3. Validation Results

The figure `seismic-load-with-STL-contact-verification.png` shows the **horizontal displacement (X)** of the column's base compared with the displacement directly integrated from the input seismic record.

### Key Observations:
- For **low friction coefficients** the column does not follow the terrain movement due to the **relative sliding**.
- As friction increases the **relative sliding** is reduced and the column's motion is close to the input record.
- The implementation accurately reproduces the expected behavior, where friction controls the transmission of seismic motion to the body.