#!/usr/bin/env python
import matplotlib.pyplot as plt
# from matplotlib import cm
import numpy as np
# from pylab import *
import cmath
# from mpl_toolkits.mplot3d import Axes3D


DIR="/home/rinku/XOOPIC/xoopic/Antenna/den_1e15"

# numerical data file
fileName=DIR+'/antenna_E1_vector.txt'

datax, datay, dataEx, dataEy = np.loadtxt(fileName, unpack=True)


Nx = 65
Ny = 33

X = np.reshape(datax, (Nx, Ny))
Y = np.reshape(datay, (Nx, Ny))
U = np.reshape(dataEx,(Nx, Ny))
V = np.reshape(dataEy,(Nx, Ny))
###########################

fig, ax = plt.subplots(1,1,figsize=(7,5))
plt.rcParams["font.size"] = "14"
plt.rcParams["font.family"] = "DejaVu Sans"

cntr1 = ax.quiver(X, Y, U, V)
ax.set_xlabel("Axial Length (m)",fontsize=14)
ax.set_ylabel("Radial Length (m)",fontsize=14)
# ax.set_xlim(min(X0)+0.05, max(X0)-0.05)
# ax.set_ylim(min(Y0)+0.05, max(Y0)-0.05)

ax.tick_params(axis='both', which='major', labelsize=12)
fig.tight_layout()

plt.show()
