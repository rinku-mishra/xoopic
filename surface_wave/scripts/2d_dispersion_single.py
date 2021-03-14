#!/usr/bin/env python3
import numpy as np
import matplotlib.pyplot as plt
from pylab import *
import cmath
import plasmapy.dispersion.dispersionfunction as plasma
import os
from time import sleep
from tqdm import tqdm

#run = "E_15/"
#yLoc = 50
dataDir      = "/scratch/data/outputs/E_15/"
processedDir = "/scratch/data/outputs/dispDataProcessed/"
processedFile = "dispersed_data_E_15_yloc_50.dat"

#Use the data directory
fileBase = 'Ex'  #base name for E-filed data (e.g. if Ez_1, Ez_2 ... then Ez_)
NtStart  = 100
NtStop   = 1000000  #Number of time data (number of files)
period   = 20  #period value set while dumping data E-data
dt       = 9.0E-12*period
yLoc = 4
vmax = 0.07
vmin = 0
nlevels = 10
cmap = 'viridis'


# Calculating Nx and Ny from data
fileName=dataDir+fileBase+'_%08d'%NtStart+'.dat'
index_x,index_y,x,y,Ex = np.loadtxt(fileName, unpack=True)
Nx = int(np.amax(index_x)+1)
Ny = int(np.amax(index_y)+1)
#Nx = 100
#Ny = 60

# Selecting omega and k
OmegaLow = 0
OmegaHigh = 5000  #int(NtStop/100)  #4000
KLow =  50
KHigh = 80  #4000

if os.path.exists(processedDir+processedFile):
	print("Data exists. Let's load the data.")
	Exxt = np.loadtxt(processedDir+processedFile, unpack=True)
	print("Shape of loaded data Exxt",Exxt.shape)
	Exxt = np.array(Exxt.T)
	print("Shape of final data Exxt",Exxt.shape)
	print("Using PlasmaPy library. Wait ....")
else:
	print("Data does not exist, Processing \n")
	Exxt = [] # Setting Empty Array
	########## Data loading  #########
	for i in tqdm(range(NtStart,NtStop,period)):
    		fileName=dataDir+fileBase+'_%08d'%i+'.dat'
    		index_x,index_y,x,y,Ex = np.loadtxt(fileName, unpack=True)
    		Exxy = np.reshape(Ex,(Nx, Ny))
    		Exxt.append(Exxy[:,yLoc])
	###################################
	Exxt = np.array(Exxt)
	print("Shape of final data Exxt",Exxt.shape)
	if os.path.exists(processedDir):
		np.savetxt(processedDir+processedFile,Exxt,fmt='%.8f')
		print("Processed data saved at "+processedDir+" with the name: "+processedFile)
	else:
		print("Processed Data directory does not exists. Creating directory ...")
		os.system('mkdir '+processedDir)
		np.savetxt(processedDir+processedFile,Exxt,fmt='%.8f')
		print("Processed data saved at "+processedDir+" with the name:  "+processedFile)
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

