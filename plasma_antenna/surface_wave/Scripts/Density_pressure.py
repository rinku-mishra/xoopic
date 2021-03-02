#!/usr/bin/env python
import matplotlib.pyplot as plt
from matplotlib import cm
import numpy as np
from pylab import *
import cmath
from mpl_toolkits.mplot3d import Axes3D
#import matplotlib.mlab as mlab
#import matplotlib.gridspec as gridspec

DIR="/home/rinku/XOOPIC/xoopic/Plasma_Antenna/New2/Press_var_v2/Press_0-005"

PRESS = 0.005
DATA = "Electron"
# numerical data file

if DATA=="Ion":
    filename1=DIR+'/Ion_density.txt'
elif DATA=="Electron":
    filename1=DIR+'/Electron_density.txt'
else:
    raise Exception("Error in input DATA TYPE")


data_act1 = np.loadtxt(filename1, delimiter="\t", skiprows=2, usecols=[0,2,4])
#data_act1 = np.loadtxt(filename1, delimiter="\t", usecols=[0,2,4])

datax = data_act1[:,0]
datay = data_act1[:,1]
datanAr = data_act1[:,2]

#disp(datax.shape)
#disp(datay.shape)
#disp(datanAr.shape)
Nx = 129
Ny = 65

DENAR = np.zeros([Nx,Ny])
X0 = np.empty(Nx)
Y0 = np.empty(Ny)
DEN1D = np.empty(Nx)

#for i in range(1,Nx+1,1):
#    X[i] = datax[k]
#    k = k+Nx+1
k=0
l=0
for i in range(len(X0)):
    X0[i] = datax[k]
    k = k+Ny


for j in range(len(Y0)):
    Y0[j] = datay[l]
    l = l+1

X, Y = np.meshgrid(X0, Y0)
DENAR = np.reshape(datanAr, (Nx, Ny))

###########################
###### 1D Density #########
m = 33
for i in range(len(X0)):
    DEN1D[i] = datanAr[m]
    m = m+Ny

#disp(X0.shape)
#disp(Y0.shape)
######################################
fig = plt.figure(figsize=(8,6))
ax = fig.gca(projection='3d')
surf = ax.plot_surface(X, Y, DENAR.T, cmap=cm.viridis,edgecolor='none',linewidth=0, antialiased=False)
fig.colorbar(surf, shrink=0.5, aspect=5)
plt.xlabel("Axial Length (m)",fontsize=15)
plt.ylabel("Radial Length (m)",fontsize=15)
#fig.tight_layout()
ax.set_title(DATA+' density for pressure = 0.01 Torr',fontsize=15)
#plt.zlabel("Density")
#plt.title("Ion Density")
######################################
fig, (ax1,ax2) = plt.subplots(2,1)
#cntr = ax.contourf(X, Y, DENAR.T, 1000,cmap=cm.coolwarm)
cntr1 = ax1.contourf(X, Y, DENAR.T, 1000,cmap=cm.viridis)
ax1.set_xlabel("Axial Length (m)")
ax1.set_ylabel("Radial Length (m)")
fig.colorbar(cntr1, ax=ax1)
ax1.set_title(DATA+' density for pressure = %3.1e'%PRESS+' Torr')

cntr2 = ax2.contour(X, Y, DENAR.T, 100,cmap=cm.viridis)
ax2.set_xlabel("Axial Length (m)")
ax2.set_ylabel("Radial Length (m)")
fig.colorbar(cntr1, ax=ax2)
fig.tight_layout()
#plt.savefig("figures/disp_%3d"%TIMESTEP+"_%3d"%OMEGA_HIGHEND+"_khighend%3d"%K_HIGHEND1+'log10_def.png')
#print("First image done")
fig = plt.figure()
plt.plot(X0, DEN1D,linewidth=1)
plt.xlabel("Axial Length (m)")
if DATA=="Ion":
    plt.ylabel("Density ($N_{i}$) ($m^{-3}$)")
elif DATA=="Electron":
    plt.ylabel("Density ($N_{e}$) ($m^{-3}$)")
else:
    raise Exception("Error in input DATA TYPE")
    
#plt.tight_layout()
plt.title(DATA+' density along axial axis \n for pressure = %3.1e'%PRESS+' Torr')

plt.show()
