import numpy as np
import csv

# Parámetros de la vibración
amplitud_x = 1.0  # Amplitud de la vibración en X (m/s^2)
frecuencia_x = 10.0  # Frecuencia en X (Hz)
amplitud_y = 0.0  # Amplitud de la vibración en Y (m/s^2)
frecuencia_y = 2.0  # Frecuencia en Y (Hz)
amplitud_z = 0.0  # Amplitud de la vibración en Z (m/s^2)
frecuencia_z = 1.5  # Frecuencia en Z (Hz)

fase_x = 0.0  # Fase en X (radianes)
fase_y = np.pi / 4  # Fase en Y (radianes)
fase_z = np.pi / 2  # Fase en Z (radianes)

duracion = 1.0  # Duración de la vibración (segundos)
paso_tiempo = 0.001  # Paso de tiempo (segundos)

# Generación de tiempo
tiempo = np.arange(0, duracion, paso_tiempo)

# Generación de aceleraciones sinusoidales
aceleracion_x = amplitud_x * np.sin(2 * np.pi * frecuencia_x * tiempo + fase_x)
aceleracion_y = amplitud_y * np.sin(2 * np.pi * frecuencia_y * tiempo + fase_y)
aceleracion_z = amplitud_z * np.sin(2 * np.pi * frecuencia_z * tiempo + fase_z)

# Guardar en un archivo CSV
nombre_archivo = 'base_acceleration.csv'

with open(nombre_archivo, mode='w', newline='') as archivo_csv:
    escritor_csv = csv.writer(archivo_csv)
    #escritor_csv.writerow(['Tiempo', 'Aceleración_X', 'Aceleración_Y', 'Aceleración_Z'])
    for t, ax, ay, az in zip(tiempo, aceleracion_x, aceleracion_y, aceleracion_z):
        escritor_csv.writerow([t, ax, ay, az])

print(f"Archivo CSV guardado como {nombre_archivo}")
