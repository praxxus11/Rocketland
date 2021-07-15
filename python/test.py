import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from pylab import *
plt.rcParams["figure.figsize"] = [5,6]


fig, axs = plt.subplots(3)
fig.tight_layout()
hi = get_current_fig_manager()
hi.window.wm_geometry("+0+0")


x = []
vel = []
pos = []
velx = []
gs = []
def animate(i):
    with open("python/datas.txt", "r") as f:
        x = []
        vel = []
        pos = []
        velx = []
        for (i, l) in enumerate(f):
            sp = l.split()
            x.append(float(sp[0]))
            pos.append(float(sp[1]))
            vel.append(float(sp[2]))
            velx.append(float(sp[3]))
    for ax in axs:
        ax.cla()
    axs[0].plot(x, pos, color='black', label="Position y (m)")
    axs[0].plot(x, vel, color='red', label="Velocity y (m/s)")
    axs[0].title.set_text("Y stuff")
    axs[0].set_xlabel("Time")
    axs[0].set_ylabel("Meters")
    axs[0].legend()

    axs[1].plot(x, velx, color='green', label="Velocity x (m/s)")
    axs[1].title.set_text("Horizontal stuff")
    axs[1].legend()


ani = FuncAnimation(plt.gcf(), animate, interval=1000)
plt.tight_layout()
plt.show()
open("python/datas.txt", 'w').close() # clear the file