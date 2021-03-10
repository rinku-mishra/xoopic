#!/usr/bin/env python
import matplotlib.pyplot as plt
from matplotlib import cm
import numpy as np
from pylab import *
import cmath


DIR="data"
POWER = 0.01
color=cm.plasma

DIR = "/home/rinku/XOOPIC/xoopic/Antenna"

####### numerical data file #####
###### Ex data #######
fileName1=DIR+'/antenna_test_Ey.txt'
datax, datay, dataE = np.loadtxt(fileName1, unpack=True)

###### E vector data #######
fileName2=DIR+'/antenna_test_E_vector.txt'
datax, datay, dataEx, dataEy = np.loadtxt(fileName2, unpack=True)

Nx = 65
Ny = 33

X = np.reshape(datax, (Nx, Ny))
Y = np.reshape(datay, (Nx, Ny))
E = np.reshape(dataE,(Nx, Ny))
# E = E/np.max(E)
U = np.reshape(dataEx,(Nx, Ny))
V = np.reshape(dataEy,(Nx, Ny))


fig, ax = plt.subplots(1,1,figsize=(8,6))
cntr = ax.contourf(X, Y, E, 1000,cmap=color)
ax.set_xlabel("Axial Length (m)",fontsize=14)
ax.set_ylabel("Radial Length (m)",fontsize=14)
fig.colorbar(cntr, ax=ax)
# cntr2 = ax2.contour(X, Y, E, 20,colors='k')
ax.tick_params(axis='both', which='major', labelsize=12)


# fig, ax = plt.subplots(1,1,figsize=(7,5))
plt.rcParams["font.size"] = "14"
plt.rcParams["font.family"] = "DejaVu Sans"
quiv = ax.quiver(X, Y, U, V)
# ax.set_xlabel("Axial Length (m)",fontsize=14)
# ax.set_ylabel("Radial Length (m)",fontsize=14)
# ax.tick_params(axis='both', which='major', labelsize=12)

fig.tight_layout()

# plt.title('Electric field along axial axis \n for power = %1.1e'%POWER+' W')

plt.show()
