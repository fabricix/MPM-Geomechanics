import numpy as np
import matplotlib.pyplot as plt
import pandas as pd

# problem parameters
g = 10.0  # gravity (m/s^2)
theta = np.radians(45)  # inclination angle (radians)
dt = 0.001  # time step (s)
t_max = 2  # total time (s)

# friction coefficients
mu_values = [0.0, 0.1, 0.2, 0.3, 0.4, 0.5]
markers =  ["o", "s", "D", "*", "+", "^"]

# initialize time vector
t_values = np.arange(0, t_max, dt)

# analytical solution for displacement in y direction
def analytical_solution_y(t, mu):
    return 0.5 * ( g * np.sin(theta) - mu * g * np.cos(theta) )* t**2 

# plot analytical and MPM solutions
plt.figure(figsize=(8, 6))

for mu in mu_values:
    # analytical solution for displacement in y direction
    u_y_analytical = [analytical_solution_y(t, mu) for t in t_values]

    # load MPM results
    filename = f"mpm_results_mu_{mu}.csv"
    mpm_data = pd.read_csv(filename)

    # extract MPM results
    mpm_time = mpm_data["Time"]
    mpm_u_y = mpm_data["Displacement_0"]

    # plot two solutions
    plt.plot(t_values, u_y_analytical, linestyle="solid", label=f"Analytical (μ={mu})")
    plt.plot(mpm_time, mpm_u_y, linestyle="none", label=f"MPM-Geomechanics (μ={mu})", marker=markers[mu_values.index(mu)])

# configure plot
plt.xlabel("Time (s)")
plt.ylabel("Displacement $u$ (m)")
plt.title("Comparison of Analytical and MPM Solutions for $u$")
plt.legend()
plt.grid()
plt.show()
