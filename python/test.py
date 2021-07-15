import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from pylab import *
import matplotlib
plt.rcParams["figure.figsize"] = [5,4]

hi = get_current_fig_manager()
hi.window.wm_geometry("+0+0")

x = []
vel = []
pos = []
legend_good = False
def animate(i):
    global legend_good
    with open("python/datas.txt", "r") as f:
        x = []
        vel = []
        pos = []
        for (i, l) in enumerate(f):
            sp = l.split()
            x.append(float(sp[0]))
            pos.append(float(sp[1]))
            vel.append(float(sp[2]))
    plt.cla()
    plt.plot(x, pos, color='black', label="Position y")
    plt.plot(x, vel, color='red', label="Velocity y")
    plt.xlabel("Time")
    plt.ylabel("Velocity (m/s)")
    plt.legend()

ani = FuncAnimation(plt.gcf(), animate, interval=1000)
plt.tight_layout()
plt.show()
open("python/datas.txt", 'w').close()