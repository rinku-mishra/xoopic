#!/usr/bin/env python
import matplotlib.pyplot as plt
from matplotlib import cm
import numpy as np
from pylab import *
import cmath

POWER = 0.01
color=cm.plasma

DIR = "/home/rinku/Documents/Plasma_antenna/New_data/den_1e15"
DENSITY = 1e15
####### numerical data file #####
###### Ex data #######
fileName1=DIR+'/antenna_1e15_Ex.txt'
datax, datay, dataE = np.loadtxt(fileName1, unpack=True)

###### E vector data #######
fileName2=DIR+'/antenna_1e15_poynting_vector.txt'
datax, datay, dataEx, dataEy = np.loadtxt(fileName2, unpack=True)

Nx = 129
Ny = 65

X = np.reshape(datax, (Nx, Ny))
Y = np.reshape(datay, (Nx, Ny))
E = np.reshape(dataE,(Nx, Ny))
# E = E/np.max(E)
U = np.reshape(dataEx,(Nx, Ny))
V = np.reshape(dataEy,(Nx, Ny))


fig, ax = plt.subplots(1,1,figsize=(8,6))
cntr = ax.contourf(X, Y, E, 1000,cmap=color)
ax.set_xlabel("Axial Length (m)",fontsize=18)
ax.set_ylabel("Radial Length (m)",fontsize=18)
fig.colorbar(cntr, ax=ax)
# cntr2 = ax2.contour(X, Y, E, 20,colors='k')
ax.tick_params(axis='both', which='major', labelsize=18)


# fig, ax = plt.subplots(1,1,figsize=(7,5))
plt.rcParams["font.size"] = "25"
plt.rcParams["font.family"] = "Times"
ax.tick_params(axis="x", labelsize=18)
ax.tick_params(axis="y", labelsize=18)
quiv = ax.quiver(X, Y, U, V)
# ax.set_xlabel("Axial Length (m)",fontsize=14)
# ax.set_ylabel("Radial Length (m)",fontsize=14)
# ax.tick_params(axis='both', which='major', labelsize=12)
#plt.legend(['Density =%2.1e'%DENSITY])
fig.tight_layout()

# plt.title('Electric field along axial axis \n for power = %1.1e'%POWER+' W')

plt.show()
