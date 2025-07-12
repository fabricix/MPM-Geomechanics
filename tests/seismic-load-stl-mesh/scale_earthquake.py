import pandas as pd

# Cargar archivo original
df = pd.read_csv("base_acceleration.csv")

# Multiplicar aceleración por 10
df["ax"] *= 10
df["az"] = df["ax"]
df["ax"] *= 0.0
df["vx"] *= 0.0

# Guardar nuevo archivo
df.to_csv("base_acceleration_scaled.csv", index=False)
