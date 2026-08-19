import numpy as np
import pandas as pd

# -----------------------------
# Parámetros del problema
# -----------------------------
g = 10.0                         # gravedad (m/s^2)
theta = np.radians(45.0)         # ángulo del plano (rad)
dt = 0.001                       # paso de tiempo (s)
t_max = 2.0                      # tiempo total (s)

# Coeficientes de fricción
mu_values = [0.0, 0.3, 0.5]

# Vector de tiempo (idéntico al script original)
t_values = np.arange(0.0, t_max, dt)

# -----------------------------
# Solución analítica
# u(t) = 0.5 * a * t^2
# a = g sin(theta) - mu g cos(theta)
# -----------------------------
def analytical_displacement(t: np.ndarray, mu: float) -> np.ndarray:
    a = g * np.sin(theta) - mu * g * np.cos(theta)
    return 0.5 * a * t**2

# -----------------------------
# Escritura de CSV (uno por mu)
# -----------------------------
for mu in mu_values:
    u = analytical_displacement(t_values, mu)

    df = pd.DataFrame({
        "Time": t_values,
        "Displacement_0": u,   # mismo nombre de columna que usas al leer MPM
    })

    out_name = f"analytical_results_mu_{mu}.csv"
    df.to_csv(out_name, index=False, float_format="%.10g")
    print(f"Escrito: {out_name}")