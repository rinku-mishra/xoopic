#!/usr/bin/env python3
import numpy as np
import matplotlib.pyplot as plt
from pylab import *
import cmath
import plasmapy.dispersion.dispersionfunction as plasma
import os
from time import sleep
from tqdm import tqdm

#Use the data directory
mode          = "split"
run           = "E_25"
yLoc          =  11
dataDir       = "/scratch/data/outputs/"+run+"/"
processedDir  = "/scratch/data/outputs/dispDataProcessed/"
processedFile = "dispersed_data_"+run+"_%d"%yLoc

#####  Contour variables
vmax = 0.07
vmin = 0
nlevels = 10
cmap = 'viridis'
######################


fileBase = 'Ex'  #base name for E-filed data (e.g. if Ez_1, Ez_2 ... then Ez_)
NtStart  = 0
NtStop   = 100000  #Number of time data (number of files)
period   = 1  #period value set while dumping data E-data
dt       = 9.0e-12*period
splitNum = 4
fileRange = np.arange(NtStart, NtStop,period,dtype=int)

t1,t2,t3,t4 = np.array_split(fileRange, splitNum)

#Calculating Nx and Ny from data
fileName=dataDir+fileBase+'_%08d'%NtStart+'.dat'
index_x,index_y,x,y,Ex = np.loadtxt(fileName, unpack=True)
Nx = int(np.amax(index_x)+1)
Ny = int(np.amax(index_y)+1)
#Nx = 100
#Ny = 60

# Selecting omega and k
OmegaLow = 0
OmegaHigh = int(NtStop/100)  #4000
KLow = 0
KHigh = int(Nx/2)  #4000

if os.path.exists(processedDir):
	print("Data exists. Let's load the first data.")
	Exxt1 = np.loadtxt(processedDir+processedFile+"part_1.dat", unpack=True)
	print("Shape of loaded data Exxt1",Exxt1.shape)
	Exxt1 = np.array(Exxt1.T)
	print("Shape of final data Exxt1",Exxt1.shape)
	print("Data exists. Let's load the second data.")
	Exxt2 = np.loadtxt(processedDir+processedFile+"part_2.dat", unpack=True)
	print("Shape of loaded data Exxt2",Exxt2.shape)
	Exxt2 = np.array(Exxt2.T)
	print("Shape of final data Exxt2",Exxt2.shape)
	print("Data exists. Let's load the third data.")
	Exxt3 = np.loadtxt(processedDir+processedFile+"part_3.dat", unpack=True)
	print("Shape of loaded data Exxt3",Exxt3.shape)
	Exxt3 = np.array(Exxt3.T)
	print("Shape of final data Exxt3",Exxt3.shape)
	print("Data exists. Let's load the fourth data.")
	Exxt4 = np.loadtxt(processedDir+processedFile+"part_4.dat", unpack=True)
	print("Shape of loaded data Exxt4",Exxt4.shape)
	Exxt4 = np.array(Exxt4.T)
	print("Shape of final data Exxt4",Exxt4.shape)
	Exxt=np.vstack((Exxt1,Exxt2,Exxt3,Exxt4))
	print("Shape of final data Exxt",Exxt.shape)
	###################################
	print("Using PlasmaPy library. Wait ....")

# FFT using PlasmaPy library
F1 = plasma.plasma_dispersion_func(Exxt)
F2 = plasma.plasma_dispersion_func_deriv(Exxt)

#####################################

############### Plotting ################
fig,ax1 = plt.subplots(constrained_layout=True,dpi=150)
plt.rcParams["font.size"] = "12"
plt.rcParams["font.family"] = "serif"
im1 = ax1.contourf(F1[OmegaLow:OmegaHigh,KLow:KHigh,].real,cmap=cmap,levels=np.linspace(vmin,vmax,nlevels))
ax1.set_xlabel("$x/\\lambda_D$",fontsize=10)
#ax1.set_xlim([57,80])
#ax1.set_ylim([1000, 2000])
ax1.set_ylabel("$\\omega_{pe}t$",fontsize=10)
#plt.title("Real")
plt.colorbar(im1,ax=ax1)

#im2 = ax2.imshow(F1[OmegaLow:OmegaHigh,KLow:KHigh,].imag,cmap='viridis',vmin=vmin,vmax=vmax)
#ax2.set_xlabel("$x/\\lambda_D$",fontsize=10)
#ax2.set_ylabel("$\\omega_{pe}t$",fontsize=10)
#plt.title("Imaginary")
#plt.colorbar(im2,ax=ax2)


#fig2 = ax2.contourf(Kx,Omega,F1[OmegaLow:OmegaHigh,KLow:KHigh,].imag,100)
#ax2.set_xlabel("$k$",fontsize=10)
#ax2.set_ylabel("$\\omega$",fontsize=10)
#ax2.set_title("Imaginary")
#fig.colorbar(fig2,ax=ax2)

#fig.tight_layout()
plt.show()

# plt.savefig('I-V-PIC-%s'%object+'-%d'%height+'km.png')
