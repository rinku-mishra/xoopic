#!/usr/bin/python
import matplotlib.pyplot as plt
import numpy as np
import matplotlib.mlab as mlab
import matplotlib.gridspec as gridspec


# numerical data file


filename1="/home/rinku/Documents/Plasma_antenna/New_data/den_1e12/antenna_1e12_Electric_field_mid_length_lower_dielectric.txt"

filename2="/home/rinku/XOOPIC/xoopic/Antenna/pot_osc_new/den_1e15/antenna_1e15_electric_field_near_the_source_top.txt"


data_act1 = np.loadtxt(filename1, delimiter="\t", skiprows=2, usecols=[0,2])

data_act2 = np.loadtxt(filename2, delimiter="\t", skiprows=2, usecols=[0,2])

data1 = data_act1[:,0] #time data
data2 = data_act1[:,1] #potential data

data3 = data_act2[:,0] #time data
data4 = data_act2[:,1] #potential data

t1 = data1
dt1 = data1[2]-data1[1]
sig1 = data2
fs1 = 1/2e-13

t2 = data3
dt2 = data3[2]-data3[1]
sig2 = data4
fs2 = 1/2e-13

#plt.subplot(211)
#plt.plot(t1, sig1)

# Plot the raw time series
fig = plt.figure(constrained_layout=True)
gs = gridspec.GridSpec(2, 3, figure=fig)
ax = fig.add_subplot(gs[0, :])
ax.psd(sig1, NFFT=len(t1), pad_to=len(t1), Fs=fs1)
plt.xlim(left=1E9) #put the desired value of x-axis limit
plt.title('PSD')
plt.xscale('symlog')


fig.set_figheight(12)
fig.set_figwidth(8)
plt.tick_params(axis="x", labelsize=14)
plt.tick_params(axis="y", labelsize=14)
#plt.subplot(212)
#plt.psd(sig1, 2048, 1 / dt1)

# Plot the PSD with different amounts of zero padding. This uses the entire
# time series at once
ax2 = fig.add_subplot(gs[1, :])
ax2.psd(sig2, NFFT=len(t2), pad_to=len(t2), Fs=fs2)
plt.xlim(left=1E9) #put the desired value of x-axis limit
plt.title('PSD')
plt.xscale('symlog')
plt.tick_params(axis="x", labelsize=14)
plt.tick_params(axis="y", labelsize=14)

plt.show()
