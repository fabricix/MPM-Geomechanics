import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

def compute_seismic_displacement(filename, output_file="seismic_displacement.csv"):
    # Leer archivo con encabezado
    df = pd.read_csv(filename)

    t = df["t"].values
    ax = df["ax"].values

    dt = np.mean(np.diff(t))

    vx = np.cumsum(ax) * dt
    ux = np.cumsum(vx) * dt

    # Guardar desplazamiento
    np.savetxt(output_file, np.column_stack((t, ux)), delimiter=",", header="t,ux", comments='')

    # Graficar
    plt.plot(t, ax, label="ax (input)", color="gray", alpha=0.5)
    plt.plot(t, vx, label="vx (integrated)", color="purple")
    plt.plot(t, ux, label="ux (displacement)", color="blue")
    plt.xlabel("Time (s)")
    plt.ylabel("Response")
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    plt.show()

# Ejecutar
if __name__ == "__main__":
    compute_seismic_displacement("base_acceleration_scaled.csv")
