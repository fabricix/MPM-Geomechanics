import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

def compute_seismic_displacement(filename, output_file="seismic_displacement.csv"):
    # Leer archivo con encabezado
    df = pd.read_csv(filename)

    t = df["t"].values
    a = df["az"].values

    dt = np.mean(np.diff(t))

    v = np.cumsum(a) * dt
    u = np.cumsum(v) * dt

    # Guardar desplazamiento
    np.savetxt(output_file, np.column_stack((t, u)), delimiter=",", header="t,u", comments='')

    # Graficar
    plt.plot(t, a, label="a (input)", color="black")
    plt.plot(t, v, label="v (integrated)", color="purple")
    plt.plot(t, u, label="u (displacement)", color="blue")
    plt.xlabel("Time (s)")
    plt.ylabel("Response")
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    plt.show()

# Ejecutar
if __name__ == "__main__":
    compute_seismic_displacement("base_acceleration_scaled.csv")
