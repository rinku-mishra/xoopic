#!/usr/bin/env python3
import numpy as np
import matplotlib.pyplot as plt
from pylab import *
import cmath
import plasmapy.dispersion.dispersionfunction as plasma
import os
from time import sleep
from tqdm import tqdm

dataDir  = "/scratch/data/outputs/E_5/"
#Use the data directory 
fileBase = 'Ex'  #base name for E-filed data (e.g. if Ez_1, Ez_2 ... then Ez_)
Lx       = 0.235
#Nx       = 100
#Ny       = 60
NtStart  = 100
NtStop   = 10000000  #Number of time data (number of files)
period   = 10  #period value set while dumping data E-data
dt       = 9.05e-11*period

fileName=dataDir+fileBase+'_%08d'%NtStart+'.dat'
index_x,index_y,x,y,Ex = np.loadtxt(fileName, unpack=True)
Nx = int(np.amax(index_x)+1)
Ny = int(np.amax(index_y)+1)
yLoc = int(11)
dx   = Lx/Nx

# Selecting omega and k
OmegaLow = 0
OmegaHigh = 1000 //int(NtStop/1000)  #4000
KLow =  0
KHigh = Nx  #4000

# Setting Empty Array
X        = np.empty([NtStop,Nx])
#Exxt       = np.empty([NtStop,Nx])
Exxt = []
########## Data loading  #########
for i in tqdm(range(NtStart,NtStop,period)):
    sleep(0.0001)
    fileName=dataDir+fileBase+'_%08d'%i+'.dat'
    index_x,index_y,x,y,Ex = np.loadtxt(fileName, unpack=True)
    Exxy = np.reshape(Ex,(Nx, Ny))
#    Exx = np.zeros(Nx)
    Exxt.append(Exxy[:,yLoc])
###################################
Exxt = np.array(Exxt)
# FFT using PlasmaPy library
F1 = plasma.plasma_dispersion_func(Exxt)
F2 = plasma.plasma_dispersion_func_deriv(Exxt)

#####################################
# Setting up space time mesh grid
t = np.arange(NtStop)*dt
space = np.arange(Nx)*dx
nx = len(space)
nt = len(t)
sizex = nx*dx
sizet = nt*dt
omega = t/sizet # two sides frequency range
kx = np.linspace(0,Nx,num=Nx)
omega_norm = omega[range(round(OmegaLow),round(OmegaHigh))] # one side frequency range
omega_norm = (omega_norm*(1/dt))  #in KHz
Kx, Omega = np.meshgrid(kx[KLow:KHigh], omega_norm)


############### Plotting ################
#fig,(ax1,ax2) = plt.subplots(1,2,figsize=(6,3),dpi=150)
plt.rcParams["font.size"] = "12"
plt.rcParams["font.family"] = "Arial"
fig1 = plt.contourf(F1[OmegaLow:OmegaHigh,KLow:KHigh,].real,100)
plt.xlabel("$x/\\lambda_D$",fontsize=10)
plt.ylabel("$\\omega_{pe}t$",fontsize=10)
#plt.title("Real")
plt.colorbar(fig1)

#fig2 = ax2.contourf(Kx,Omega,F1[OmegaLow:OmegaHigh,KLow:KHigh,].imag,100)
#ax2.set_xlabel("$k$",fontsize=10)
#ax2.set_ylabel("$\\omega$",fontsize=10)
#ax2.set_title("Imaginary")
#fig.colorbar(fig2,ax=ax2)

#fig.tight_layout()
plt.show()

# plt.savefig('I-V-PIC-%s'%object+'-%d'%height+'km.png')
