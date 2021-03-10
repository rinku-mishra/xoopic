#!/usr/bin/env python
import matplotlib.pyplot as plt
from matplotlib import cm
import numpy as np
from pylab import *
import cmath
from mpl_toolkits.mplot3d import Axes3D
#import matplotlib.mlab as mlab
#import matplotlib.gridspec as gridspec

DIR="data"
POWER = 0.01
color=cm.plasma
# numerical data file

fileName=DIR+'/antenna_Ey.txt'

datax, datay, dataE = np.loadtxt(fileName, unpack=True)

Nx = 65
Ny = 33

X = np.reshape(datax, (Nx, Ny))
Y = np.reshape(datay, (Nx, Ny))
E = np.reshape(dataE,(Nx, Ny))
E = E/np.max(E)
# print(X[:,0])
###########################
###### 1D Density #########
E1D = np.zeros(Nx)
m = 17
for i in range(Nx):
    E1D[i] = dataE[m]
    m = m+Ny
######################################
# fig = plt.figure(figsize=(8,6))
# ax = fig.gca(projection='3d')
# surf = ax.plot_surface(X, Y, E, cmap=color,edgecolor='none',linewidth=0, antialiased=False)
# fig.colorbar(surf, shrink=0.5, aspect=5)
# plt.xlabel("Axial Length (m)",fontsize=15)
# plt.ylabel("Radial Length (m)",fontsize=15)
# plt.xticks(fontsize=10)
# plt.yticks(fontsize=10)
# #fig.tight_layout()
# ax.set_title('Electric field for power = 0.01 W',fontsize=12)
#plt.zlabel("Density")
#plt.title("Ion Density")
######################################

fig, ax2 = plt.subplots(1,1,figsize=(8,6))
cntr1 = ax2.contourf(X, Y, E, 1000,cmap=color)
ax2.set_xlabel("Axial Length (m)")
ax2.set_ylabel("Radial Length (m)")
fig.colorbar(cntr1, ax=ax2)
ax2.set_title('Electric field for power = %1.1e'%POWER+' W')

cntr2 = ax2.contour(X, Y, E, 20,colors='k')

fig.tight_layout()
#plt.savefig("figures/disp_%3d"%TIMESTEP+"_%3d"%OMEGA_HIGHEND+"_khighend%3d"%K_HIGHEND1+'log10_def.png')
#print("First image done")


fig = plt.figure()
plt.plot(X[:,0], E1D,linewidth=1)
plt.xlabel("Axial Length (m)",fontsize=12)
plt.ylabel("Electric field ($E_x$)",fontsize=12)
plt.xticks(fontsize=12)
plt.yticks(fontsize=12)


#plt.tight_layout()
plt.title('Electric field along axial axis \n for power = %1.1e'%POWER+' W')

plt.show()
