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
run           = "E_te08"
yLoc          =  11
dataDir       = "/scratch/data/outputs/"+run+"/"
processedDir  = "/scratch/data/outputs/dispDataProcessed/"
processedFile = "dispersed_data_"+run+"_%d"%yLoc

#####  Contour variables
vmax = 0.7
vmin = -0.7
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

t = [t1,t2,t3,t4]
#Calculating Nx and Ny from data
fileName=dataDir+fileBase+'_%08d'%NtStart+'.dat'
index_x,index_y,x,y,Ex = np.loadtxt(fileName, unpack=True)
Nx = int(np.amax(index_x)+1)
Ny = int(np.amax(index_y)+1)
#Nx = 100
#Ny = 60

# Selecting omega and k
OmegaLow = 0
OmegaHigh = 100 # int(NtStop/100)  #4000
KLow = 0
KHigh = Nx #int(Nx/2)  #4000

partFiles=processedDir+processedFile
if os.path.exists(partFiles+"part_1.dat") and os.path.exists(partFiles+"part_2.dat") and os.path.exists(partFiles+"part_3.dat") and os.path.exists(partFiles+"part_4.dat"):
	processing = False #Flag to control plotting function
	print("Data exists. Dispersion will be plotted. Let's load the first data.")
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
else:
	print("Data does not exist. Plotting function will be turned off. Now data are being processed. Wait...")
	processing = True #Flag to control plotting function
	pn = int(1) #part file number initialization
	for s in t:
		Exxt = [] # Setting Empty Array
	########## Data loading  #########
		for i in tqdm(s):
    			fileName=dataDir+fileBase+'_%08d'%i+'.dat'
    			index_x,index_y,x,y,Ex = np.loadtxt(fileName, unpack=True)
    			Exxy = np.reshape(Ex,(Nx, Ny))
    			Exxt.append(Exxy[:,yLoc])
	###################################
		Exxt = np.array(Exxt)
		print("Shape of final data Exxt",Exxt.shape)
		if os.path.exists(processedDir):
			np.savetxt(processedDir+processedFile+"part_%d"%pn+".dat",Exxt,fmt='%.8f')
			print("Processed data saved at "+processedDir+" with the name: "+processedFile+"part_%d"%pn+".dat")
		else:
			print("Processed Data directory does not exists. Creating directory ...")
			os.system('mkdir '+processedDir)
			np.savetxt(processedDir+processedFile+"part_%d"%pn+".dat",Exxt,fmt='%.8f')
			print("Processed data saved at "+processedDir+" with the name:  "+processedFile+"part_%d"%pn+".dat")

		pn+=1 #increase part file number by 1
		print("Processing of Part - %d"%pn+" complete.")

	print("Processing of all data completed.")

if processing == False:
	print("Using PlasmaPy library. Wait ....")
	# FFT using PlasmaPy library
	F1 = plasma.plasma_dispersion_func(Exxt)
	F2 = plasma.plasma_dispersion_func_deriv(Exxt)

#####################################

############### Plotting ################	
	fig,ax = plt.subplots(constrained_layout=True,dpi=150)
	plt.rcParams["font.size"] = "12"
	plt.rcParams["font.family"] = "serif"
	im = ax.contourf(F1[OmegaLow:OmegaHigh,KLow:KHigh,].real,cmap=cmap,levels=np.linspace(vmin,vmax,nlevels))
	ax.set_xlabel("$x/\\lambda_D$",fontsize=10)
	#ax.set_xlim([57,80])
	#ax.set_ylim([1000, 2000])
	ax.set_ylabel("$\\omega_{pe}t$",fontsize=10)
	#plt.title("Real")
	plt.colorbar(im,ax=ax)
	plt.savefig('dispersion'+run+'_%d'%yLoc+'.png')
	plt.show()
