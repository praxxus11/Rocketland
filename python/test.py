import numpy as np
import matplotlib.pyplot as plt
import matplotlib
import time

x = []
vel = []
pos = []
legend_good = False
while (True):
    with open("data.txt", "r") as f:
        for (i, l) in enumerate(f):
            sp = l.split()
            x.append(float(sp[0]))
            vel.append(float(sp[1]))
            pos.append(float(sp[2]))
    open("data.txt", 'w').close()
    plt.plot(x, vel, color='red', label="Velocity y")
    plt.plot(x, pos, color="green", label="Position y")
    plt.xlabel("Time")
    plt.ylabel("Velocity (m/s)")
    if not legend_good:
        plt.legend()
        legend_good = True
    plt.pause(0.5)