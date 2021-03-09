#!/usr/bin/env python
import matplotlib.pyplot as plt
import numpy as np
import matplotlib.mlab as mlab
import matplotlib.gridspec as gridspec
from spectrum import *

#PRESS = np.array([1.8E-5,1.8E-4,1.8E-3])
# numerical data file
filename1="/home/rinku/XOOPIC/xoopic/Antenna/den_1e12/antenna_electric_field_end_region.txt"
filename2="/home/rinku/XOOPIC/xoopic/Antenna/den_1e12/antenna_electric_field_mid_portion.txt"
filename3="/home/rinku/XOOPIC/xoopic/Antenna/den_1e12/antenna_electric_field_near_the_source.txt"

data_act1 = np.loadtxt(filename1, delimiter="\t", skiprows=2, usecols=[0,2])
data_act2 = np.loadtxt(filename2, delimiter="\t", skiprows=2, usecols=[0,2])
data_act3 = np.loadtxt(filename3, delimiter="\t", skiprows=2, usecols=[0,2])

data1 = data_act1[:,0] #time data
data2 = data_act1[:,1] #potential data

data3 = data_act2[:,0] #time data
data4 = data_act2[:,1] #potential data

data5 = data_act3[:,0] #time data
data6 = data_act3[:,1] #potential data


t1 = data1
dt1 = data1[2]-data1[1]
sig1 = data2

t2 = data3
dt2 = data3[2]-data3[1]
sig2 = data4

t3 = data5
dt3 = data5[2]-data5[1]
sig3 = data6


t = data1
dt = data1[2]-data1[1]
samp_freq = 1/2e-13
sig1 = 1/2e-13

p1 = Periodogram(sig1, sampling=1/2e-13,window='hann',scale_by_freq=False)  #Increase/Decrease the sampling value as per convenience
p1.run()

p1.plot(norm=True, color='blue', linewidth=0.2)


plt.xscale('symlog')
plt.xlim(1E3,5E7)
plt.xlabel("Frequency / Hz")
plt.ylabel("Power Spectral Density / dB/Hz")
ax.tick_params(axis="x", labelsize=18)
ax.tick_params(axis="y", labelsize=18)

#plt.legend(['Pressure %3.1e'%PRESS[0]+' Torr','Pressure %3.1e'%PRESS[1]+' Torr','Pressure %3.1e'%PRESS[2]+' Torr'])
#
plt.show()
