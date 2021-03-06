import os
import time
import numpy as np
import matplotlib.pyplot as plt
from ionmd import Simulation, Status

sim = Simulation()

params = sim.params
params.filename = "trajectories.bin"
sim.params = params
print(sim.params)

print("Adding ions...")

n_ions = 2

z = np.linspace(-5, 5, n_ions)

for n in range(n_ions):
    sim.add_ion(40, 1, [0, 0, z[n]])

print("Running simulation...")
t_start = time.time()
sim.start()
time.sleep(0.2)

while sim.status != Status.FINISHED:
    print("Still running... Elapsed time: {:.3f} s".format(time.time() - t_start))
    time.sleep(0.5)

print("Done in {:.3f} s".format(time.time() - t_start))

shape = (n_ions*3, params.num_steps)
# shape = (params.num_steps, n_ions*3)
data = np.fromfile(params.filename, dtype=np.double).reshape(shape).T

if n_ions <= 10:
    fig, ax = plt.subplots(3, n_ions)
    t = np.linspace(0, params.num_steps * params.dt, params.num_steps)
    lim = -1
    for n in range(n_ions):
        for k in range(3):
            ax[k, n].plot(t[:lim], data[:lim, k])
    plt.show()
