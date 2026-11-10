import numpy as np
import pandas as pd
from matplotlib.figure import Figure
from matplotlib.backends.backend_agg import FigureCanvasAgg as FigureCanvas
import matplotlib as plt

# problem parameters
g = 10.0  # gravity (m/s^2)
theta = np.radians(45)  # inclination angle (radians)
dt = 0.001  # time step (s)
t_max = 2  # total time (s)

# friction coefficients
mu_values = [0.0, 0.1, 0.3, 0.5]
markers =  ["o", "o", "o", "o", "o", "o"]
colors = plt.cm.tab10.colors[:len(markers)]

# initialize time vector
t_values = np.arange(0, t_max, dt)

# analytical solution for displacement in y direction
def analytical_solution_y(t, mu):
    return 0.5 * (g * np.sin(theta) - mu * g * np.cos(theta)) * t**2

# create figure and axis (OO style, sem pyplot)
fig = Figure(figsize=(8, 6))
canvas = FigureCanvas(fig)
ax = fig.add_subplot(111)

for mu in mu_values:
    
    # analytical solution for displacement in y direction
    u_y_analytical = [analytical_solution_y(t, mu) for t in t_values]

    # plot analytical
    ax.plot(t_values, u_y_analytical, linestyle="solid", label=f"Analytical (μ={mu})", color=colors[mu_values.index(mu)])

for mu in mu_values:
    
    # load MPM results
    filename = f"mpm_results_mu_{mu}_USL.csv"
    mpm_data = pd.read_csv(filename)

    # extract MPM results
    mpm_time = mpm_data["Time"]
    mpm_u_y = mpm_data["avg(Displacement (0))"]

    # plot MPM results
    ax.plot(mpm_time, mpm_u_y, linestyle="none", label=f"MPM-Geomechanics (μ={mu})-USL", marker="o", color=colors[mu_values.index(mu)],markerfacecolor='none')

for mu in mu_values:
    
    # load MPM results
    filename = f"mpm_results_mu_{mu}_MUSL.csv"
    mpm_data = pd.read_csv(filename)

    # extract MPM results
    mpm_time = mpm_data["Time"]
    mpm_u_y = mpm_data["avg(Displacement (0))"]

    # plot MPM results
    ax.plot(mpm_time, mpm_u_y, linestyle="none", label=f"MPM-Geomechanics (μ={mu})-MUSL", marker="x", color=colors[mu_values.index(mu)])

# configure plot
ax.set_xlabel("Time (s)")
ax.set_ylabel("Displacement $u$ (m)")
ax.set_title("Comparison of Analytical and MPM Solutions for $u$")
ax.legend()
ax.grid(True)

# render/save output
fig.savefig("mpm-analytical-comparison.png", dpi=300)
print("png saved...")
