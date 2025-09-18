import numpy as np
import json

# Parámetros del cubo
origin = np.array([4.0, 3.0, 5.0])  # esquina inferior del cubo
size = 1.0
spacing = 0.5

# Número de puntos en cada dirección
n = int(size / spacing) + 1

# Generar malla regular dentro del cubo
x, y, z = np.meshgrid(
    np.linspace(origin[0], origin[0]+size, n),
    np.linspace(origin[1], origin[1]+size, n),
    np.linspace(origin[2], origin[2]+size, n)
)

# Aplanar y combinar las coordenadas
positions = np.vstack([x.ravel(), y.ravel(), z.ravel()]).T
volume = spacing/2**3  # volumen por partícula

# Crear estructura JSON
particles = [
    {
        "id": i+1,
        "position": pos.tolist(),
        "volume": volume
    }
    for i, pos in enumerate(positions)
]

# Guardar en archivo
with open("particle_list.json", "w") as f:
    json.dump(particles, f, indent=4)

print(f"Archivo generado con {len(particles)} partículas.")
