import numpy as np
import matplotlib.pyplot as plt
from pylab import *
import cmath
#plasma formulary library
import plasmapy.dispersion.dispersionfunction as plasma
import os
from time import sleep
from tqdm import tqdm

dataDir  = "../data/antenna4/disp_data"
#Use the data directory 
fileBase = 'Ex_'  #base name for E-filed data (e.g. if Ez_1, Ez_2 ... then Ez_)
Lx       = 0.3
Nx       = 128
Ny       = 64
Nt       = 10000  #Number of time data (number of files)
period   = 5  #period value set while dumping data E-data
yLoc0     = 64 #
dt       = 2E-13*period
dx       = Lx/Nx

# Selecting omega and k
OmegaLow = 0
OmegaHigh = int(Nt/200) #TIMESTEP #4000
KLow =  0
KHigh = Nx #TIMESTEP #4000

# Setting Empty Array
X        = np.empty([Nt,Nx+1])
EX       = np.empty([Nt,Nx+1])

yLoc     = yLoc0

########## Data loading  #########
for i in tqdm(range(Nt)):
    sleep(0.001)
    fileName = dataDir+'/'+fileBase+'%d'%(i+1)+'.txt'
    x,y,Ex = np.loadtxt(fileName, unpack=True)
    for j in range(Nx+1):
        X[i,j]  = x[yLoc]
        EX[i,j] = Ex[yLoc]
        yLoc    = yLoc+(Ny+1)
    yLoc = yLoc0 #Reset yLoc for i
    np.delete(x,[1]) #Clear memory of x
    np.delete(y,[1]) #Clear memory of y
    np.delete(Ex,[1]) #Clear memory of Ex
###################################

# FFT using PlasmaPy library
F1 = plasma.plasma_dispersion_func(EX)
F2 = plasma.plasma_dispersion_func_deriv(EX)

#####################################
# Setting up space time mesh grid
t = np.arange(Nt)*dt
space = np.arange(Nx+1)*dx
nx = len(space)
nt = len(t)
sizex = nx*dx
sizet = nt*dt
omega = t/sizet # two sides frequency range
kx = np.linspace(0,Nx,num=Nx+1)
omega_norm = omega[range(round(OmegaLow),round(OmegaHigh))] # one side frequency range
omega_norm = (omega_norm*(1/dt))  #in KHz
Kx, Omega = np.meshgrid(kx[KLow:KHigh], omega_norm)


############### Plotting ################
#fig,(ax1,ax2) = plt.subplots(1,2,figsize=(6,3),dpi=150)
plt.rcParams["font.size"] = "12"
plt.rcParams["font.family"] = "Arial"
fig1 = plt.contourf(Kx,Omega,F1[OmegaLow:OmegaHigh,KLow:KHigh,].real,100)
plt.xlabel("$k$",fontsize=10)
plt.ylabel("$\\omega$",fontsize=10)
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
