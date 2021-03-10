#!/usr/bin/python
import matplotlib.pyplot as plt
import numpy as np
import matplotlib.mlab as mlab
import matplotlib.gridspec as gridspec


# numerical data file
filename1="/home/rinku/XOOPIC/xoopic/Antenna/pot_osc_new/den_1e12/antenna_electric_field_top_mid_length.txt"


data_act1 = np.loadtxt(filename1, delimiter="\t", skiprows=2, usecols=[0,2])


data1 = data_act1[:,0] #time data
data2 = data_act1[:,1] #potential data



t1 = data1
dt1 = data1[2]-data1[1]
sig1 = data2
fs = 1/2e-13

#plt.subplot(211)
#plt.plot(t1, sig1)

# Plot the raw time series
#fig = plt.figure(constrained_layout=True)
#gs = gridspec.GridSpec(2, 3, figure=fig)
#ax = fig.add_subplot(gs[0, :])
#ax.plot(t1, sig1)
#ax.set_xlabel('time [s]')
#ax.set_ylabel('signal')

#p1 = Periodogram(sig1, fs=1/2e-13,window='hann',scale_by_freq=False)  #Increase/Decrease the sampling value as per convenience
#p1.run()

#plt.subplot(212)
#plt.psd(sig1, 2048, 1 / dt1)

# Plot the PSD with different amounts of zero padding. This uses the entire
# time series at once
#ax2 = fig.add_subplot(gs[1, :])
plt.psd(sig1, NFFT=len(t1), pad_to=len(t1), Fs=fs)
plt.xlim(left=1E7) #put the desired value of x-axis limit
plt.title('PSD')
plt.xscale('symlog')

plt.show()
