#!/usr/bin/env python3
import numpy as np
import matplotlib.pyplot as plt
from pylab import *
from scipy import signal
import pandas as pd 


filename = "/home/rinku/XOOPIC/xoopic/Antenna/den_1e16/antenna_electric_field_near_the_source.txt"


time0,sig0 = np.loadtxt(filename, unpack=True)

#6DELETE FIRST PART OF THE SIGNAL
DEL_LEN = 1000
time= time0[DEL_LEN:len(time0)]
sig= sig0[DEL_LEN:len(sig0)]

dt = time[2]-time[1]
fs = 1/dt #sampling frequency

#Filter requirements.
fc = 1e7      # desired cutoff frequency of the filter, Hz
w = fc/(fs/2)   # Normalize the frequency

### Signal Filter
b, a = signal.butter(5, w, 'low')

output = signal.filtfilt(b, a, sig)


###### file writing

filtered_data = np.array([time, output])

filtered_data = filtered_data.T
#here you transpose your data, so to have it in two columns

datafile_path = "/home/rinku/XOOPIC/xoopic/Antenna/den_1e16/datafile.txt"
with open(datafile_path, 'w+') as datafile_id:
#here you open the ascii file


	np.savetxt(datafile_id, filtered_data, fmt=['%5.15f','%3.7f'])
	#here the ascii file is written. 


## Plot
fig,((ax1, ax2), (ax3, ax4))=plt.subplots(2,2,figsize=(10,8))

ax1.plot(time, sig, label='unfiltered')
ax1.set_xlabel("Time (s)",fontsize=12)
ax1.set_ylabel("Potential",fontsize=12)
ax1.legend()

ax2.psd(sig, NFFT=len(time), pad_to=len(time), Fs=fs, label='PSD unfiltered')
ax2.legend()

ax3.plot(time, output, label='filtered')
ax3.set_xlabel("Time (s)",fontsize=12)
ax3.set_ylabel("Potential",fontsize=12)
ax3.legend()

ax4.psd(output, NFFT=len(time), pad_to=len(time), Fs=fs, label='PSD filtered')
ax4.legend()

#plt.show()
