import numpy as np
import csv

# Parámetros de la vibración
amplitud_x = 0.3  # Amplitud de la vibración en X (m/s^2)
amplitud_y = 0.3  # Amplitud de la vibración en Y (m/s^2)
amplitud_z = 0.0  # Amplitud de la vibración en Z (m/s^2)

frecuencia_x = 1.0  # Frecuencia en X (Hz)
frecuencia_y = 0.5  # Frecuencia en Y (Hz)
frecuencia_z = 0.0  # Frecuencia en Z (Hz)

fase_x = np.pi / 1  # Fase en X (radianes)
fase_y = np.pi / 1  # Fase en Y (radianes)
fase_z = np.pi / 1  # Fase en Z (radianes)

duracion = 2.0  # Duración de la vibración (segundos)
paso_tiempo = 1e-4/2.0  # Paso de tiempo (segundos)

# Generación de tiempo
tiempo = np.arange(0, duracion, paso_tiempo)

# Generación de velocidades sinusoidales
velocidad_x = amplitud_x * np.sin(2 * np.pi * frecuencia_x * tiempo + fase_x)
velocidad_y = amplitud_y * np.sin(2 * np.pi * frecuencia_y * tiempo + fase_y)
velocidad_z = amplitud_z * np.sin(2 * np.pi * frecuencia_z * tiempo + fase_z)

# Derivar la aceleración de la velocidad (aceleración es la derivada de la velocidad)
aceleracion_x = amplitud_x * 2 * np.pi * frecuencia_x * np.cos(2 * np.pi * frecuencia_x * tiempo + fase_x)
aceleracion_y = amplitud_y * 2 * np.pi * frecuencia_y * np.cos(2 * np.pi * frecuencia_y * tiempo + fase_y)
aceleracion_z = amplitud_z * 2 * np.pi * frecuencia_z * np.cos(2 * np.pi * frecuencia_z * tiempo + fase_z)

# Guardar en un archivo CSV para MPM
nombre_archivo = 'base_acceleration.csv'
with open(nombre_archivo, mode='w', newline='') as archivo_csv:
    escritor_csv = csv.writer(archivo_csv)
    escritor_csv.writerow(['t','ax','ay','az'])
    for t, ax, ay, az in zip(tiempo, aceleracion_x, aceleracion_y, aceleracion_z):
        escritor_csv.writerow([t, ax, ay, az])

# Guardar en un archivo CSV para verificación de velocidades
nombre_archivo = 'base_acceleration_velocity.csv'
with open(nombre_archivo, mode='w', newline='') as archivo_csv:
    escritor_csv = csv.writer(archivo_csv)
    escritor_csv.writerow(['t','ax','ay','az','vx','vy','vz'])
    for t, ax, ay, az, vx, vy, vz in zip(tiempo, aceleracion_x, aceleracion_y, aceleracion_z, velocidad_x, velocidad_y, velocidad_z):
        escritor_csv.writerow([t, ax, ay, az, vx, vy, vz])