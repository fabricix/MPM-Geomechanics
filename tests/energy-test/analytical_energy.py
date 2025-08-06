import csv

# Constants
g = 9.81       # gravity
m = 64000      # mass
dt = 0.001     # time step offset

# Read time values from time-energy.csv
with open("time-energy.csv", "r") as infile:
    reader = csv.reader(infile)
    next(reader)  # Skip header
    times = [float(row[0]) for row in reader]

# Compute analytical energy and write to analytical_energy.csv
with open("analytical_energy.csv", "w", newline="") as outfile:
    writer = csv.writer(outfile)
    writer.writerow(["time", "energy"])
    for t in times:
        if t == 0.0:
            energy = 0.0
        else:
            energy = 0.5 * m * (g ** 2) * ((t - dt / 2) ** 2)
        writer.writerow([f"{t:.6f}", f"{energy:.5e}"])
