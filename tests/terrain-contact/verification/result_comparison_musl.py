import numpy as np
import matplotlib.pyplot as plt
import pandas as pd

# problem parameters
g = 10.0  # gravity (m/s^2)
theta = np.radians(45)  # inclination angle (radians)
dt = 0.001  # time step (s)
t_max = 2  # total time (s)

# friction coefficients
mu_values = [0.0, 0.3, 0.5]
markers =  ["o", "s", "D", "*", "+", "^"]

# initialize time vector
t_values = np.arange(0, t_max, dt)

# analytical solution for displacement in y direction
def analytical_solution_y(t, mu):
    return 0.5 * ( g * np.sin(theta) - mu * g * np.cos(theta) )* t**2 

# plot analytical and MPM solutions
plt.figure(figsize=(10, 6))

for mu in mu_values:
    # analytical solution
    u_y_analytical = [analytical_solution_y(t, mu) for t in t_values]

    # load MPM USL results
    filename_usl = f"mpm_results_mu_{mu}.csv"
    mpm_data_usl = pd.read_csv(filename_usl)
    mpm_time_usl = mpm_data_usl["Time"]
    mpm_u_y_usl = mpm_data_usl["Displacement_0"]

    # load MPM MUSL results
    filename_musl = f"mpm_results_mu_{mu}_musl.csv"
    mpm_data_musl = pd.read_csv(filename_musl)
    mpm_time_musl = mpm_data_musl["Time"]
    mpm_u_y_musl = mpm_data_musl["Displacement_0"]

    # plot
    idx = mu_values.index(mu)
    plt.plot(t_values, u_y_analytical, linestyle="solid", label=f"Analytical (μ={mu})")
    plt.plot(mpm_time_usl, mpm_u_y_usl, linestyle="none", marker=markers[idx], label=f"MPM-USL (μ={mu})")
    plt.plot(mpm_time_musl, mpm_u_y_musl, linestyle="none", marker=markers[idx], label=f"MPM-MUSL (μ={mu})")

# configure plot
plt.xlabel("Time (s)")
plt.ylabel("Displacement $u$ (m)")
plt.title("Comparison of Analytical, USL, and MUSL MPM Solutions for $u$")
plt.legend()
plt.grid()
plt.tight_layout()
plt.show()
