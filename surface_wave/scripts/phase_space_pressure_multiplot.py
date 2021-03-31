#!/usr/bin/env python
import matplotlib.pyplot as plt
import numpy as np
# import matplotlib.mlab as mlab
# import matplotlib.gridspec as gridspec
#from spectrum import *
from pylab import *
#import cmath
#import matplotlib.mlab as mlab
#import matplotlib.gridspec as gridspec
import matplotlib.font_manager as font_manager

DIR1="../inputs/E2_data"
DIR2="../inputs/E5_data"
DIR3="../inputs/E15_data"
DIR4="../inputs/E25_data"
#POWER = 2
#DATA = "Ion"
# numerical data file
filename1=DIR1+'/phase_space_ionx.txt'
filename2=DIR1+'/phase_space_electronx.txt'
filename3=DIR2+'/phase_space_ionx.txt'
filename4=DIR2+'/phase_space_electronx.txt'
filename5=DIR3+'/phase_space_ionx.txt'
filename6=DIR3+'/phase_space_electronx.txt'
filename7=DIR4+'/phase_space_ionx.txt'
filename8=DIR4+'/phase_space_electronx.txt'

data_act1 = np.loadtxt(filename1, delimiter="\t", skiprows=2, usecols=[0,2])
data_act2 = np.loadtxt(filename2, delimiter="\t", skiprows=2, usecols=[0,2])
data_act3 = np.loadtxt(filename3, delimiter="\t", skiprows=2, usecols=[0,2])
data_act4 = np.loadtxt(filename4, delimiter="\t", skiprows=2, usecols=[0,2])
data_act5 = np.loadtxt(filename5, delimiter="\t", skiprows=2, usecols=[0,2])
data_act6 = np.loadtxt(filename6, delimiter="\t", skiprows=2, usecols=[0,2])
data_act7 = np.loadtxt(filename7, delimiter="\t", skiprows=2, usecols=[0,2])
data_act8 = np.loadtxt(filename8, delimiter="\t", skiprows=2, usecols=[0,2])

datax1 = data_act1[:,0]
datay1 = data_act1[:,1]
datax2 = data_act2[:,0]
datay2 = data_act2[:,1]

datax3 = data_act3[:,0]
datay3 = data_act3[:,1]
datax4 = data_act4[:,0]
datay4 = data_act4[:,1]

datax5 = data_act5[:,0]
datay5 = data_act5[:,1]
datax6 = data_act6[:,0]
datay6 = data_act6[:,1]

datax7 = data_act7[:,0]
datay7 = data_act7[:,1]
datax8 = data_act8[:,0]
datay8 = data_act8[:,1]

figure(1)
fig, axs = plt.subplots(2,2)
axs[0, 0].scatter(datax1,datay1,s=1,marker='.')
axs[0, 0].set_title("Pressure - 0.01 Torr")
# Set the font dictionaries (for plot title and axis titles)
title_font = {'fontname':'Times', 'size':'16', 'color':'black', 'weight':'normal',
  'verticalalignment':'bottom'} # Bottom vertical alignment for more space
axis_font = {'fontname':'Times', 'size':'18'}
fig.tight_layout(pad=1.8)


axs[0, 1].scatter(datax3,datay3,s=1,marker='.')
axs[0, 1].set_title("Pressure - 0.03 Torr")
title_font = {'fontname':'Times', 'size':'16', 'color':'black', 'weight':'normal',
  'verticalalignment':'bottom'} # Bottom vertical alignment for more space
axis_font = {'fontname':'Times', 'size':'18'}
fig.tight_layout(pad=1.8)

axs[1, 0].scatter(datax5,datay5,s=1,marker='.')
axs[1, 0].set_title("Pressure - 0.05 Torr")
title_font = {'fontname':'Times', 'size':'16', 'color':'black', 'weight':'normal',
  'verticalalignment':'bottom'} # Bottom vertical alignment for more space
axis_font = {'fontname':'Times', 'size':'18'}
fig.tight_layout(pad=1.8)

axs[1, 1].scatter(datax7,datay7,s=1,marker='.')
axs[1, 1].set_title("Pressure - 0.07 Torr")
title_font = {'fontname':'Times', 'size':'16', 'color':'black', 'weight':'normal',
  'verticalalignment':'bottom'} # Bottom vertical alignment for more space
axis_font = {'fontname':'Times', 'size':'18'}
fig.tight_layout(pad=1.8)

for ax in axs.flat:
    ax.set(xlabel='Axial Length', ylabel='Radial Length')

# Hide x labels and tick labels for top plots and y ticks for right plots.
for ax in axs.flat:
    ax.label_outer()

###########

figure(2)
fig, axs = plt.subplots(2,2)
axs[0, 0].scatter(datax2,datay2,s=1,marker='.')
axs[0, 0].set_title("Pressure - 0.01 Torr")
title_font = {'fontname':'Times', 'size':'16', 'color':'black', 'weight':'normal',
  'verticalalignment':'bottom'} # Bottom vertical alignment for more space
axis_font = {'fontname':'Times', 'size':'18'}
fig.tight_layout(pad=1.8)


axs[0, 1].scatter(datax4,datay4,s=1,marker='.')
axs[0, 1].set_title("Pressure - 0.03 Torr")
title_font = {'fontname':'Times', 'size':'16', 'color':'black', 'weight':'normal',
  'verticalalignment':'bottom'} # Bottom vertical alignment for more space
axis_font = {'fontname':'Times', 'size':'18'}
fig.tight_layout(pad=1.8)

axs[1, 0].scatter(datax6,datay6,s=1,marker='.')
axs[1, 0].set_title("Pressure - 0.05 Torr")
title_font = {'fontname':'Times', 'size':'16', 'color':'black', 'weight':'normal',
  'verticalalignment':'bottom'} # Bottom vertical alignment for more space
axis_font = {'fontname':'Times', 'size':'18'}
fig.tight_layout(pad=1.8)

axs[1, 1].scatter(datax8,datay8,s=1,marker='.')
axs[1, 1].set_title("Pressure - 0.07 Torr")
title_font = {'fontname':'Times', 'size':'16', 'color':'black', 'weight':'normal',
  'verticalalignment':'bottom'} # Bottom vertical alignment for more space
axis_font = {'fontname':'Times', 'size':'18'}
fig.tight_layout(pad=1.8)



#for ax in axs.flat:
#    ax.set(xlabel='Axial Length', ylabel='Radial Length')

# Hide x labels and tick labels for top plots and y ticks for right plots.
#for ax in axs.flat:
#    ax.label_outer()

#plt.savefig("figures/disp_%3d"%TIMESTEP+"_%3d"%OMEGA_HIGHEND+"_khighend%3d"%K_HIGHEND1+'log10_def.png')
#print("First image done")

plt.show()
