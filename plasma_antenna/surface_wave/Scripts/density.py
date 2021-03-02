#!/usr/bin/env python
import matplotlib.pyplot as plt
from matplotlib import cm
import numpy as np
from pylab import *
import cmath
from mpl_toolkits.mplot3d import Axes3D
#import matplotlib.mlab as mlab
#import matplotlib.gridspec as gridspec

DIR="../data/antenna4"
#POWER = 0.01
color=cm.plasma
# numerical data file
fileName=DIR+'/Ex.txt'

#Choose Axis
dataAxis = "x"
#Exponetial Moving Average
EMA = True

datax, datay, dataE = np.loadtxt(fileName, unpack=True)

Nx = 129
Ny = 65

###########################################
######## Exponetial Moving Average ########

def ema(s, n):
    """
    returns an n period exponential moving average for
    the time series s

    s is a list ordered from oldest (index 0) to most
    recent (index -1)
    n is an integer

    returns a numeric array of the exponential
    moving average
    """
    s = array(s)
    ema = []
    j = 1

    #get n sma first and calculate the next n period ema
    sma = sum(s[:n]) / n
    multiplier = 2 / float(1 + n)
    ema.append(sma)

    #EMA(current) = ( (Price(current) - EMA(prev) ) x Multiplier) + EMA(prev)
    ema.append(( (s[n] - sma) * multiplier) + sma)

    #now calculate the rest of the values
    for i in s[n+1:]:
        tmp = ( (i - ema[j]) * multiplier) + ema[j]
        j = j + 1
        ema.append(tmp)

    return ema





X = np.reshape(datax, (Nx, Ny))
Y = np.reshape(datay, (Nx, Ny))
E = np.reshape(dataE,(Nx, Ny))
E = E/np.max(E)
# print(Y[0,:])
###########################
###### 1D Density #########
if dataAxis == "x":
    E1D = np.zeros(Nx)
    m = int(Ny/8)+1
    pointY = Y[0,m]
    for i in range(Nx):
        E1D[i] = dataE[m]
        m = m+Ny
elif dataAxis == "y":
    E1D = np.zeros(Ny)
    m = int(Nx/8)+1
    pointX = X[m,0]
    for i in range(Ny):
        E1D[i] = dataE[m]
        m = m+1
else:
    raise Exception("Choice of axis not found")
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

fig, ax = plt.subplots(1,1,figsize=(8,6))
cntr1 = ax.contourf(X, Y, E, 1000,cmap=color)
ax.set_xlabel("Axial Length (m)")
ax.set_ylabel("Radial Length (m)")
fig.colorbar(cntr1, ax=ax)
#ax.set_title('Electric field for power = %1.1e'%POWER+' W')
cntr2 = ax.contour(X, Y, E, 20,colors='k')
if dataAxis == "x":
    ax.axhline(pointY,0,1,color="white",linestyle='--')
elif dataAxis == "y":
    ax.axvline(pointX,0,1,color="white",linestyle='--')
else:
    raise Exception("Choice of axis not found")
fig.tight_layout()
#plt.savefig("figures/disp_%3d"%TIMESTEP+"_%3d"%OMEGA_HIGHEND+"_khighend%3d"%K_HIGHEND1+'log10_def.png')
#print("First image done")
if EMA == True:
    E1D = ema(E1D,2)

fig = plt.figure()
if dataAxis == "x":
    plt.plot(X[:len(E1D),0], E1D,'o-',linewidth=1)
    plt.xlabel("Axial Length (m)",fontsize=12)
    plt.ylabel("Electric field ($E_x$)",fontsize=12)
elif dataAxis == "y":
    plt.plot(Y[0,:len(E1D)], E1D,'o-',linewidth=1)
    plt.xlabel("Radial Length (m)",fontsize=12)
    plt.ylabel("Electric field ($E_y$)",fontsize=12)
else:
    raise Exception("Choice of axis not found")
plt.xticks(fontsize=12)
plt.yticks(fontsize=12)


#plt.tight_layout()
#plt.title('Electric field along axial axis \n for power = %1.1e'%POWER+' W')

plt.show()
