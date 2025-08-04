dt = 0.001
step = 0.1
energy = 0.0
t = 0.0
g = 9.81
m = 64000

print("time,energy")

while(t <= 2.0):
	if t == 0.0:
		energy = 0.0
	else:
		energy = 1/2 * m * (g ** 2) * ((t - dt/2) ** 2)
	print(f"{t},{energy:.5e}")
	t += step
	t = round(t, 2)