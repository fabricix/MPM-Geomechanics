import json
import numpy as np
from shapely.geometry import Polygon, Point

# Load config
with open("config.json") as f:
    config = json.load(f)

polygon = Polygon(config["polygon"])
z_water = config["z_water"]
rho = config["density"]
g = config["gravity"]
dx = config["dx"]
dy = config["dy"]
dz = config["dz"]
y_min = config["y_min"]
y_max = config["y_max"]
output_file = config["output_file"]

# Define grid
min_x, min_z, max_x, max_z = polygon.bounds
x_vals = np.arange(min_x, max_x + dx, dx)
y_vals = np.arange(y_min, y_max + dy, dy)
z_vals = np.arange(min_z, max_z + dz, dz)

pressure_data = []

# Iterate over 3D grid
for x in x_vals:
    for z in z_vals:
        if polygon.contains(Point(x, z)):
            for y in y_vals:
                pressure = rho * g * max(0.0, z_water - z)
                pressure_data.append({
                    "position": [float(x), float(y), float(z)],
                    "pressure": pressure
                })

# Save output
with open(output_file, "w") as f:
    json.dump(pressure_data, f, indent=2)

print(f"Generated 3D pore pressure field to {output_file}")
