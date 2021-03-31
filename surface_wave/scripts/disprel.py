#!/usr/bin/env python3

# Usage: ./disprel.py path/to/folder

import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from matplotlib import cm
import numpy as np
import sys
from os.path import join as pjoin
from scipy.constants import value as constants
from glob import glob
import re
from tqdm import tqdm

folder = sys.argv[1]
files = glob(pjoin(folder, '*'))
pat = re.compile('\d+')
files.sort(key=lambda x: int(pat.findall(x)[-1]))

# READ TEMPORAL GRID

with open(pjoin(folder, '..', 'timestep.txt')) as file:
    dt = float(file.readline())

n = np.array([int(pat.findall(a)[-1]) for a in files])
n += 1 # XOOPIC diagnostics is actually off by one
t = n*dt
Nt = len(t)

# READ SPATIAL GRID

data = np.loadtxt(files[0])

x, y = data[:,2:4].T
Ny = len(np.where(x==x[0])[0])
Nx = len(x)//Ny
x = x.reshape(Nx, Ny)
y = y.reshape(Nx, Ny)

# READ FIELD

f = np.zeros((Nt, Nx, Ny))

for i, file in enumerate(tqdm(files)):
    data = np.loadtxt(file)
    f[i] = data[:,-1].reshape(Nx, Ny)

# Remove y
f = np.average(f, axis=2)

# Compress axes
# ind = range(len(t)-128, len(t), 1)
ind = range(len(t)-128, len(t), 1)
ind = range(len(t))
t = t[ind]
f = f[ind]
x = x[:,0]

# FFT axes
dt = t[1]-t[0]
dx = x[1]-x[0]
Mt = len(t)
Mx = len(x)
omega = 2*np.pi*np.array(range(Mt))/(Mt*dt)
k     = 2*np.pi*np.array(range(Mx))/(Mx*dx)
Omega, K = np.meshgrid(omega, k)

F = np.fft.fft2(f).T

halflen = np.array(F.shape, dtype=int)//2
Omega = Omega[:halflen[0],:halflen[1]]
K = K[:halflen[0],:halflen[1]]
F = F[:halflen[0],:halflen[1]]


# Analytical ion-acoustic dispresion relation
ni = 1e12
eps0 = constants('electric constant')
kb = constants('Boltzmann constant')
mi = 40*constants('atomic mass constant')
e = constants('elementary charge')
gamma_e = 5./3
Te = 1160.4
wpi = np.sqrt(e**2*ni/(eps0*mi))
print("wpi={}".format(wpi))
cia = np.sqrt(gamma_e*kb*Te/mi)
ka = np.linspace(0, np.max(K), 100)
wa = np.sqrt((ka*cia)**2/(1+(ka*cia/wpi)**2))

omega /= wpi
Omega /= wpi
wa /= wpi

Z = np.log(np.abs(F))
# Z = np.abs(F)
# omega = x[:,0]
# X, T = np.meshgrid(x[:,0], t)
print(K.shape, Omega.shape, Z.shape)
# T = T.T
# X = X.T
# Z = Z.T

plt.pcolor(K, Omega, Z)

plt.plot(ka, wa, '--w')

plt.xlabel('k [1/m]')
plt.ylabel('$\omega/\omega_{pi}$')
plt.savefig(pjoin('disprel_prad_15.png'))
plt.show()

# fig = plt.figure()
# ax = plt.gca(projection='3d')
# plt.imshow(np.log(np.abs(F[-1024:,:])))
# surf = ax.plot_surface(T, X, Z, cmap=cm.coolwarm, linewidth=0, antialiased=False)
# fig.colorbar(surf, shrink=0.5, aspect=5)
# plt.show()

