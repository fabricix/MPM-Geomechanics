import numpy as np
import pandas as pd
from matplotlib.figure import Figure
from matplotlib.backends.backend_agg import FigureCanvasAgg as FigureCanvas



# create figure and axis (OO style, sem pyplot)
fig = Figure(figsize=(8, 6))
canvas = FigureCanvas(fig)
ax = fig.add_subplot(111)

# load MPM results
filename = f"block1.csv"
mpm_data = pd.read_csv(filename)

# extract MPM results
mpm_time = mpm_data["Time"]
mpm_u_y = mpm_data["avg(Z)"]

# plot MPM results
ax.plot(mpm_time, mpm_u_y, linestyle="dashed", label=f"block 1 - no correction")

# load MPM results
filename = f"block1-correction.csv"
mpm_data = pd.read_csv(filename)

# extract MPM results
mpm_time = mpm_data["Time"]
mpm_u_y = mpm_data["avg(Z)"]

# plot MPM results
ax.plot(mpm_time, mpm_u_y, linestyle="solid", label=f"block 1 - real distance correction")

# load MPM results
filename = f"block2.csv"
mpm_data = pd.read_csv(filename)

# extract MPM results
mpm_time = mpm_data["Time"]
mpm_u_y = mpm_data["avg(Z)"]

# plot MPM results
ax.plot(mpm_time, mpm_u_y, linestyle="dashed", label=f"block 2 - no correction")

# load MPM results
filename = f"block2-correction.csv"
mpm_data = pd.read_csv(filename)

# extract MPM results
mpm_time = mpm_data["Time"]
mpm_u_y = mpm_data["avg(Z)"]

# plot MPM results
ax.plot(mpm_time, mpm_u_y, linestyle="solid", label=f"block 2 - real distance correction")


# configure plot
ax.set_xlabel("Time (s)")
ax.set_ylabel("Block base vertical position $z$ (m)")
ax.set_title("Real distance correction contribution")
ax.legend()
ax.grid(True)

# render/save output
fig.savefig("comparison.png", dpi=150)


# create figure and axis (OO style, sem pyplot)
fig = Figure(figsize=(8, 6))
canvas = FigureCanvas(fig)
ax = fig.add_subplot(111)

# load MPM results
filename = f"block1.csv"
mpm_data = pd.read_csv(filename)

# extract MPM results
mpm_time = mpm_data["Time"]
mpm_u_y = mpm_data["avg(Velocity (2))"]

# plot MPM results
ax.plot(mpm_time, mpm_u_y, linestyle="dashed", label=f"block 1 - no correction")

# load MPM results
filename = f"block1-correction.csv"
mpm_data = pd.read_csv(filename)

# extract MPM results
mpm_time = mpm_data["Time"]
mpm_u_y = mpm_data["avg(Velocity (2))"]

# plot MPM results
ax.plot(mpm_time, mpm_u_y, linestyle="solid", label=f"block 1 - real distance correction")

# load MPM results
filename = f"block2.csv"
mpm_data = pd.read_csv(filename)

# extract MPM results
mpm_time = mpm_data["Time"]
mpm_u_y = mpm_data["avg(Velocity (2))"]

# plot MPM results
ax.plot(mpm_time, mpm_u_y, linestyle="dashed", label=f"block 2 - no correction")

# load MPM results
filename = f"block2-correction.csv"
mpm_data = pd.read_csv(filename)

# extract MPM results
mpm_time = mpm_data["Time"]
mpm_u_y = mpm_data["avg(Velocity (2))"]

# plot MPM results
ax.plot(mpm_time, mpm_u_y, linestyle="solid", label=f"block 2 - real distance correction")


# configure plot
ax.set_xlabel("Time (s)")
ax.set_ylabel("Block vertical velocity $z$ (m/s)")
ax.set_title("Real distance correction contribution")
ax.legend()
ax.grid(True)

# render/save output
fig.savefig("comparison-velocity.png", dpi=150)
print("Gráfico salvo em 'comparison.png'")
