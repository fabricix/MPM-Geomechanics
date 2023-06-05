import pandas as pd
import matplotlib.pyplot as plt

# Read the data from CSV files
data = pd.read_csv('MohrCoulombVerificationMPM.csv')
stress_mpm = data['stress']
strain_mpm = data['strain']

# Create the figure and axes
fig, ax = plt.subplots()

# Plot the measured stresses
ax.plot(strain_mpm, stress_mpm,'-b', label='MPM-Geomechanics',)

# Read the calculated stresses data from another CSV file
data = pd.read_csv('MohrCoulombAnalyticalSolutionConfinedCompression.csv')
stress_analy = data['stress']
strain_analy = data['strain']

# Plot the calculated stresses
ax.plot(strain_analy, stress_analy, '--r', label='Analytical solution')

# Configure the axes and title
ax.set_xlabel('Strain')
ax.set_ylabel('Stress')
ax.set_title('Mohr-Coulomb Verification')

# Add a legend
ax.legend()

# Save the figure in PDF format for publication use
plt.savefig('MhorCoulombVerification.pdf', format='pdf')

# Show the plot
plt.show()