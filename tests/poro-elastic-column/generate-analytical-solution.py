import numpy as np
import pandas as pd
import os

#
# This file generates the analytical solution of the poro-elastic column for the oedometer test problem.
# In this implementation the compresibility of the fluid was considered using the Skempton equivalent modulus. 
#

# ==========================================
# Parameters
# ==========================================
H = 10.0          # Soil height [m]
P0 = 1000.0       # Initial pore pressure [Pa]

# For deriving Cv
E = 50e6          # Elasticity modulus [Pa]
nu = 0.3          # Poisson ratio
k = 0.005         # Permeability [m/s]
rho_w = 1000.0    # water density [kg/m3]
g = 9.81          # Gravity [m/s2]
Kf = 500e6        # Fluid bulk modulus
n = 0.3           # Porosity

# Compression modulus Es and Consolidation coefficient Cv
Es = E * (1 - nu) / ((1 + nu) * (1 - 2 * nu))
#Cv = (k * Es) / (rho_w * g)   # for incompressible fluid

# Equivalent modulus for compressible fluid
Eeq = 1/( 1/Es + n/Kf )
Cv = (k * Eeq) / (rho_w * g) 

# Time
tiempos_segundos = np.linspace(0, 3.5, 36)

dir_veusz = './analytical_solution/'
os.makedirs(dir_veusz, exist_ok=True)

# ==========================================
# Analityc solution
# ==========================================
def calcular_presion_analitica_tiempo_real(Z_array, t, terminos=50):
    """
    Fourier serie solution for t
    """
    if t == 0.0:
        return np.full_like(Z_array, P0)
        
    Tv = (Cv * t) / (H**2)
    
    P = np.zeros_like(Z_array)
    for k_idx in range(terminos):
        m = 2 * k_idx + 1  
        termino_seno = np.sin((m * np.pi * (H - Z_array)) / (2.0 * H))
        termino_exp = np.exp(-((m**2 * np.pi**2) / 4.0) * Tv)
        P += (1.0 / m) * termino_seno * termino_exp
        
    return (4.0 * P0 / np.pi) * P

# ==========================================
# csv file generation
# ==========================================
Z_teorico = np.linspace(0, H, 100)
count = 0
for t in tiempos_segundos:
    P_teorico = calcular_presion_analitica_tiempo_real(Z_teorico, t)
    
    df = pd.DataFrame({
        'Altura_Z_m': Z_teorico,
        'Presion_Teoria_Pa': P_teorico
    })
    
    out_file = os.path.join(dir_veusz, f'solucion_analitica_t_{t}s_step{count}.csv')
    df.to_csv(out_file, index=False, sep=',')
    print(f"Exportado correctamente: {out_file}")
    count +=1