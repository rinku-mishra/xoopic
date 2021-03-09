#!/usr/bin/env python
import matplotlib.pyplot as plt
from matplotlib import cm
import numpy as np
from pylab import *
import cmath
from mpl_toolkits.mplot3d import Axes3D
#import matplotlib.mlab as mlab
#import matplotlib.gridspec as gridspec

DIR1="/home/rinku/XOOPIC/xoopic/Plasma_Antenna/Antenna_rf/Plasma_antenna_new/Press_var/Press_005"
DIR2="/home/rinku/XOOPIC/xoopic/Plasma_Antenna/Antenna_rf/Plasma_antenna_new/Press_var/Press_01"
DIR3="/home/rinku/XOOPIC/xoopic/Plasma_Antenna/Antenna_rf/Plasma_antenna_new/Press_var/Press_05"
#PRESS = 0.005
# numerical data file

filename1=DIR1+'/antenna_Ex_005.txt'
filename2=DIR2+'/antenna_Ex_01.txt'
filename3=DIR3+'/antenna_Ex_05.txt'

data_act1 = np.loadtxt(filename1, delimiter="\t", skiprows=2, usecols=[0,2,4])
data_act2 = np.loadtxt(filename2, delimiter="\t", skiprows=2, usecols=[0,2,4])
data_act3 = np.loadtxt(filename3, delimiter="\t", skiprows=2, usecols=[0,2,4])
#data_act1 = np.loadtxt(filename1, delimiter="\t", usecols=[0,2,4])

datax1 = data_act1[:,0]
datay1 = data_act1[:,1]
dataphi1 = data_act1[:,2]

datax2 = data_act2[:,0]
datay2 = data_act2[:,1]
dataphi2 = data_act2[:,2]

datax3 = data_act3[:,0]
datay3 = data_act3[:,1]
dataphi3 = data_act3[:,2]

#disp(datax.shape)
#disp(datay.shape)
#disp(dataphi.shape)
Nx = 65
Ny = 33

PHI = np.zeros([Nx,Ny])
X0 = np.empty(Nx)
Y0 = np.empty(Ny)
DEN1D1 = np.empty(Nx)
DEN1D2 = np.empty(Nx)
DEN1D3 = np.empty(Nx)
#for i in range(1,Nx+1,1):
#    X[i] = datax[k]
#    k = k+Nx+1
k1=0
l1=0
k2=0
l2=0
k3=0
l3=0
for i in range(len(X0)):
    X0[i] = datax1[k1]
    k1 = k1+Ny


for j in range(len(Y0)):
    Y0[j] = datay1[l1]
    l1 = l1+1

X1, Y1 = np.meshgrid(X0, Y0)
PHI1 = np.reshape(dataphi1, (Nx, Ny))

#########
for i in range(len(X0)):
    X0[i] = datax2[k2]
    k2 = k2+Ny


for j in range(len(Y0)):
    Y0[j] = datay2[l2]
    l2 = l2+1

X2, Y2 = np.meshgrid(X0, Y0)
PHI2 = np.reshape(dataphi2, (Nx, Ny))

############
for i in range(len(X0)):
    X0[i] = datax3[k3]
    k3 = k3+Ny


for j in range(len(Y0)):
    Y0[j] = datay3[l3]
    l3 = l3+1

X3, Y3 = np.meshgrid(X0, Y0)
PHI3 = np.reshape(dataphi3, (Nx, Ny))


###########################
###### 1D Density #########
m1 = 17
m2 = 17
m3 = 17
for i in range(len(Y0)):
    DEN1D1[i] = dataphi1[m1]
    m1 = m1+Nx
    
for i in range(len(Y0)):
    DEN1D2[i] = dataphi2[m2]
    m2 = m2+Nx
    
for i in range(len(Y0)):
    DEN1D3[i] = dataphi3[m3]
    m3 = m3+Nx
#disp(X0.shape)
#disp(Y0.shape)
######################################
fig = plt.figure(figsize=(8,6))
ax = fig.gca(projection='3d')
surf = ax.plot_surface(X1, Y1, PHI1.T, cmap=cm.viridis,edgecolor='none',linewidth=0, antialiased=False)
fig.colorbar(surf, shrink=0.5, aspect=5)
plt.xlabel("Axial Length (m)",fontsize = 12)
plt.ylabel("Radial Length (m)",fontsize = 12)
#fig.tight_layout()
#ax.set_title('Electric field for pressure = %1.1e'%PRESS+' Torr',fontsize = 12)
ax.set_title('Electric field for pressure = 0.01 Torr',fontsize = 12)
#plt.zlabel("Density")
#plt.title("Ion Density")
plt.xticks(fontsize=10)
plt.yticks(fontsize=10) 
######################################
fig, (ax1,ax2) = plt.subplots(2,1)
#cntr = ax.contourf(X, Y, PHI.T, 1000,cmap=cm.coolwarm)
cntr1 = ax1.contourf(X1, Y1, PHI1.T, 1000,cmap=cm.viridis)
ax1.set_xlabel("Axial Length (m)",fontsize = 15)
ax1.set_ylabel("Radial Length (m)",fontsize = 15)
fig.colorbar(cntr1, ax=ax1)
#ax1.set_title('Electric field for pressure = %1.1e'%PRESS+' Torr',fontsize = 15)

cntr2 = ax2.contour(X1, Y1, PHI1.T, 50,cmap=cm.viridis)
ax2.set_xlabel("Axial Length (m)")
ax2.set_ylabel("Radial Length (m)")
fig.colorbar(cntr1, ax=ax2)
fig.tight_layout()
plt.xticks(fontsize=12)
plt.yticks(fontsize=12) 
#plt.savefig("figures/disp_%3d"%TIMESTEP+"_%3d"%OMEGA_HIGHEND+"_khighend%3d"%K_HIGHEND1+'log10_def.png')
#print("First image done")
fig = plt.figure(figsize=(6,4))
#plt.plot(X0, DEN1D1,linewidth=1)
plt.plot(X0, DEN1D2,linewidth=1)
plt.plot(X0, DEN1D3,linewidth=1)
plt.xlabel("Axial Length (m)",fontsize = 12)
plt.ylabel("Electric field ($E_x$)",fontsize = 12)
plt.xticks(fontsize=12)
plt.yticks(fontsize=12) 
    
#plt.tight_layout()
#plt.title('Electric field along axial axis \n for pressure = %1.1e'%PRESS+' Torr')

plt.show()
