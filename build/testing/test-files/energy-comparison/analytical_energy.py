dt = 0.001
step = 0.1
energy = 0.0
t = 0.0
g = 9.81
m = 64000

print("")
print("- Calulo de energía cinética de un cuerpo en caída libre.")
print("- Se aproxima a 2 decimales.")
print("- Resultados: ")
print("")

print("time,energy")
while(t <= 2.0):
	energy = 1/2 * m * (g ** 2) * ((t - dt/2) ** 2)
	print(f"{round(t, 2)},{round(energy,2)}")
	t += step
	t = round(t, 2)