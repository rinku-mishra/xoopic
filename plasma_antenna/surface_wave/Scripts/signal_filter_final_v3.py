#!/usr/bin/env python3
import numpy as np
import matplotlib.pyplot as plt
from pylab import *
from scipy import signal
import pandas as pd 


filename = "/home/rinku/XOOPIC/xoopic/Antenna/den_1e13/antenna_elecric_field_near_the_source.txt"


time0,sig0 = np.loadtxt(filename, unpack=True)

t1 = time0
dt = time0[2]-time0[1]
fs = 1/dt #sampling frequency

# create the 100e6 Hz filter

# Create/view notch filter
notch_freq = 100e6  # Frequency to be removed from signal (Hz)
quality_factor = 150.0  # Quality factor
b_notch, a_notch = signal.iirnotch(notch_freq, quality_factor, fs)
freq, h = signal.freqz(b_notch, a_notch, fs)


y_notched = signal.filtfilt(b_notch, a_notch, sig0)


############### file writing
filtered_data = np.array([t1, y_notched])

filtered_data = filtered_data.T
#here you transpose your data, so to have it in two columns

datafile_path = "/home/rinku/XOOPIC/xoopic/Antenna/den_1e12/datafile.txt"
with open(datafile_path, 'w+') as datafile_id:
#here you open the ascii file


	np.savetxt(datafile_id, filtered_data, fmt=['%5.15f','%3.7f'])
	#here the ascii file is written. 



figure(1)
plt.plot( t1, sig0, t1, y_notched)

figure(2)
plt.psd(sig0, NFFT=len(t1), pad_to=len(t1), Fs=fs)
plt.psd(y_notched, NFFT=len(t1), pad_to=len(t1), Fs=fs)
plt.legend(['Unfiltered','Filtered'])


plt.show()
