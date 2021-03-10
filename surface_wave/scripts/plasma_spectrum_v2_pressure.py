#!/usr/bin/env python
import matplotlib.pyplot as plt
import numpy as np
import matplotlib.mlab as mlab
import matplotlib.gridspec as gridspec
from spectrum import*

#PRESS = np.array([1.8E-5,1.8E-4,1.8E-3])
# numerical data file
filename1="/home/rinku/XOOPIC/xoopic/Antenna/pot_osc_new/den_1e12/antenna_1e14_near_the_source_mid.txt"

filename2="/home/rinku/XOOPIC/xoopic/Antenna/pot_osc_new/den_1e13/antenna_1e13_electric_field_top_region_mid_length.txt"

filename3="/home/rinku/XOOPIC/xoopic/Antenna/pot_osc_new/den_1e14/antenna_1e14_electric_field_mid_length_top.txt"


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

dt1 = 2e-13

#t = data1
#dt = data1[2]-data1[1]
#samp_freq = 1/dt
#sig = data2

p1 = Periodogram(sig1, sampling=1/dt1,window='hann',scale_by_freq=False)  #Increase/Decrease the sampling value as per convenience
p1.run()
#p2 = Periodogram(sig2, sampling=1/dt1,window='hann',scale_by_freq=False)
#p2.run()
#p3 = Periodogram(sig3, sampling=1/dt1,window='hann',scale_by_freq=False)
#p3.run()

fig = plt.figure(figsize=(8,6))
p1.plot(norm=True, color='blue', linewidth=2)
#p2.plot(norm=True, color='red', linewidth=2)
#p3.plot(norm=True, color='green', linewidth=2)


plt.xscale('symlog')
plt.xlim(left=2e8)
plt.xlabel("Frequency / Hz",fontsize= 14)
plt.ylabel("Power Spectral Density / dB/Hz",fontsize= 13)
plt.tick_params(axis="x", labelsize=14)
plt.tick_params(axis="y", labelsize=14)

#plt.legend(['Pressure %3.1e'%PRESS[0]+' Torr','Pressure %3.1e'%PRESS[1]+' Torr','Pressure %3.1e'%PRESS[2]+' Torr'])

#plt.legend([' Near Plasma Source','Mid Region','End Region'],prop={"size":14})
#
plt.show()
