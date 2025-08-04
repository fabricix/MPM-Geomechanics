import pandas as pd

# Cargar archivo original
df = pd.read_csv("base_acceleration.csv")

# eliminar velocidades
df.drop('vx', axis=1, inplace=True)
df.drop('vy', axis=1, inplace=True)
df.drop('vz', axis=1, inplace=True)

# Guardar nuevo archivo
df.to_csv("base_acceleration_comma.csv", index=False,sep=',')
df.to_csv("base_acceleration_tab.csv", index=False,sep='\t')
df.to_csv("base_acceleration_space.csv", index=False,sep=' ')
