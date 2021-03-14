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
KHigh = NtStop #int(Nx/2)  #4000

if os.path.exists(processedDir+processedFile+"part_1.dat"):
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
	for i in tqdm(t1):
    		fileName=dataDir+fileBase+'_%08d'%i+'.dat'
    		index_x,index_y,x,y,Ex = np.loadtxt(fileName, unpack=True)
    		Exxy = np.reshape(Ex,(Nx, Ny))
    		Exxt.append(Exxy[:,yLoc])
	###################################
	Exxt = np.array(Exxt)
	print("Shape of final data Exxt",Exxt.shape)
	if os.path.exists(processedDir):
		np.savetxt(processedDir+processedFile+"part_1.dat",Exxt,fmt='%.8f')
		print("Processed data saved at "+processedDir+" with the name: "+processedFile+"part_1.dat")
	else:
		print("Processed Data directory does not exists. Creating directory ...")
		os.system('mkdir '+processedDir)
		np.savetxt(processedDir+processedFile+"part_1.dat",Exxt,fmt='%.8f')
		print("Processed data saved at "+processedDir+" with the name:  "+processedFile+"part_1.dat")


	######### Part -2 ############
	Exxt = []   #reseting array

	########## Data loading  #########
	for i in tqdm(t2):
    		fileName=dataDir+fileBase+'_%08d'%i+'.dat'
    		index_x,index_y,x,y,Ex = np.loadtxt(fileName, unpack=True)
    		Exxy = np.reshape(Ex,(Nx, Ny))
    		Exxt.append(Exxy[:,yLoc])
	###################################
	Exxt = np.array(Exxt)
	print("Shape of final data Exxt",Exxt.shape)
	if os.path.exists(processedDir):
		np.savetxt(processedDir+processedFile+"part_2.dat",Exxt,fmt='%.8f')
		print("Processed data saved at "+processedDir+" with the name: "+processedFile+"part_2.dat")
	else:
		print("Processed Data directory does not exists. Creating directory ...")
		os.system('mkdir '+processedDir)
		np.savetxt(processedDir+processedFile+"part_2.dat",Exxt,fmt='%.8f')
		print("Processed data saved at "+processedDir+" with the name:  "+processedFile+"part_2.dat")

    ########## Part-3 ################

	Exxt = []  #resetting array

	########## Data loading  #########
	for i in tqdm(t3):
    		fileName=dataDir+fileBase+'_%08d'%i+'.dat'
    		index_x,index_y,x,y,Ex = np.loadtxt(fileName, unpack=True)
    		Exxy = np.reshape(Ex,(Nx, Ny))
    		Exxt.append(Exxy[:,yLoc])
	###################################
	Exxt = np.array(Exxt)
	print("Shape of final data Exxt",Exxt.shape)
	if os.path.exists(processedDir):
		np.savetxt(processedDir+processedFile+"part_3.dat",Exxt,fmt='%.8f')
		print("Processed data saved at "+processedDir+" with the name: "+processedFile+"part_3.dat")
	else:
		print("Processed Data directory does not exists. Creating directory ...")
		os.system('mkdir '+processedDir)
		np.savetxt(processedDir+processedFile+"part_3.dat",Exxt,fmt='%.8f')
		print("Processed data saved at "+processedDir+" with the name:  "+processedFile+"part_3.dat")

	########## Part-4 ################

	Exxt = []  #resetting array

	########## Data loading  #########
	for i in tqdm(t3):
    		fileName=dataDir+fileBase+'_%08d'%i+'.dat'
    		index_x,index_y,x,y,Ex = np.loadtxt(fileName, unpack=True)
    		Exxy = np.reshape(Ex,(Nx, Ny))
    		Exxt.append(Exxy[:,yLoc])
	###################################
	Exxt = np.array(Exxt)
	print("Shape of final data Exxt",Exxt.shape)
	if os.path.exists(processedDir):
		np.savetxt(processedDir+processedFile+"part_4.dat",Exxt,fmt='%.8f')
		print("Processed data saved at "+processedDir+" with the name: "+processedFile+"part_4.dat")
	else:
		print("Processed Data directory does not exists. Creating directory ...")
		os.system('mkdir '+processedDir)
		np.savetxt(processedDir+processedFile+"part_4.dat",Exxt,fmt='%.8f')
		print("Processed data saved at "+processedDir+" with the name:  "+processedFile+"part_4.dat")

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
#plt.show()

# plt.savefig('I-V-PIC-%s'%object+'-%d'%height+'km.png')
