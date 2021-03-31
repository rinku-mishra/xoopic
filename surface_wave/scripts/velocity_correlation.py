import matplotlib.pyplot as plt
import numpy as np
import seaborn as sns
import pandas as pd
from pylab import *
import cmath
# numerical data file
# CAUTION!! Run the eiedf_data_gen.sh file first
DIR="../inputs/E2_data"
PRESS = 0 #1.8E-4
DATA = "Ion"
# numerical data file

if DATA=="Ion":
    filename1=DIR+'/iedf_ux_x.txt'
    filename2=DIR+'/iedf_uy_x.txt'
    funcName = "Ion Velocity Correlation"
    mass =6.6335209E-26 #Argon
elif DATA=="Electron":
    filename1=DIR+'/iedf_ux_x.txt'
    filename2=DIR+'/iedf_uy_x.txt'
    funcName = "Ion Velocity Correlation"
    mass = 9.1E-31 #electron
else:
    raise Exception("Error in input DATA TYPE")



charge_electron = 1.6e-19
KE_in_EV_coeff = 0.5*mass/charge_electron
data_actx = np.loadtxt(filename1, delimiter="\t", skiprows=2, usecols=[0,2])
data_acty = np.loadtxt(filename2, delimiter="\t", skiprows=2, usecols=[0,2])


#velocity data within a range
#range1 = np.array([0.0,0.5,1.2])
#range2 = np.array([0.1,0.6,1.3])
range1 = np.array([0.0,0.1,1.2])
range2 = np.array([0.01,0.11,1.3])
datax1 = data_actx[:,0] #space data
datax2 = data_actx[:,1] #velocity data
datay1 = data_acty[:,0] #space data
datay2 = data_acty[:,1] #velocity data
#disp(range2[2])
#Empty data array to store velocity data for a location range
data_locx1 = np.empty([len(datax1), 1], dtype=float)
data_locx1[:] = np.NaN
data_locx2 = np.empty([len(datax1), 1], dtype=float)
data_locx2[:] = np.NaN
data_locx3 = np.empty([len(datax1), 1], dtype=float)
data_locx3[:] = np.NaN
j = 0
k = 0
l = 0
#storing data for the range given for vx
for i in range(len(datax1)):
    if (datax1[i] >= range1[0]) and (datax1[i] <= range2[0]):
        data_locx1[j]= datax2[i]
        j = j+1
    elif (datax1[i] >= range1[1]) and (datax1[i] <= range2[1]):
        data_locx2[k]= datax2[i]
        k = k+1
    elif (datax1[i] >= range1[2]) and (datax1[i] <= range2[2]):
        data_locx3[l]= datax2[i]
        l = l+1
    else:
        None


data_locy1 = np.empty([len(datay1), 1], dtype=float)
data_locy1[:] = np.NaN
data_locy2 = np.empty([len(datay1), 1], dtype=float)
data_locy2[:] = np.NaN
data_locy3 = np.empty([len(datay1), 1], dtype=float)
data_locy3[:] = np.NaN
j = 0
k = 0
l = 0
#storing data for the range given for vy
for i in range(len(datay1)):
    if (datay1[i] >= range1[0]) and (datay1[i] <= range2[0]):
        data_locy1[j]= datay2[i]
        j = j+1
    elif (datax1[i] >= range1[1]) and (datay1[i] <= range2[1]):
        data_locy2[k]= datay2[i]
        k = k+1
    elif (datay1[i] >= range1[2]) and (datay1[i] <= range2[2]):
        data_locy3[l]= datay2[i]
        l = l+1
    else:
        None
#Conversion of velocities to energy
#disp(data_loc1)
data1 = np.concatenate((data_locx1, data_locy1), axis=1)
data1 = pd.DataFrame(data1, columns=['Vx', 'Vy'])

data2 = np.concatenate((data_locx2, data_locy2), axis=1)
data2 = pd.DataFrame(data2, columns=['Vx', 'Vy'])

data3 = np.concatenate((data_locx3, data_locy3), axis=1)
data3 = pd.DataFrame(data3, columns=['Vx', 'Vy'])

#data4 = np.concatenate((data_locx1, data_locx3), axis=1)
#data4 = pd.DataFrame(data4, columns=['x', 'y'])

#fig, (ax1,ax2,ax3) = plt.subplots(3,1)
#sns.distplot(data_loc1[:], hist=False, kde=True, color = 'darkblue',
#             hist_kws={'edgecolor':'black'},
#             kde_kws={'shade': True, 'linewidth': 2},ax=ax1)
#ax1.set_xlabel('Velocity (m/s)')
#ax1.set_ylabel('A.U.')
##ax1.set_xlim(-10,50)
#ax1.legend(['Location: %1.2f'%range1[0]+'-%1.2f'%range2[0]])
#ax1.set_title(funcName+'\n for pressure = %3.1e'%PRESS+' Torr')
#
#sns.distplot(data_loc2[:], hist=False, kde=True, color = 'red',
#             hist_kws={'edgecolor':'black'},
#             kde_kws={'shade': True, 'linewidth': 2},ax=ax2)
#ax2.set_xlabel('Velocity (m/s)')
#ax2.set_ylabel('A.U.')
##ax2.set_xlim(-10,50)
#ax2.legend(['Location: %1.2f'%range1[1]+'-%1.2f'%range2[1]])
#
#sns.distplot(data_loc3[:], hist=False, kde=True, color = 'green',
#             hist_kws={'edgecolor':'black'},
#             kde_kws={'shade': True, 'linewidth': 2},ax=ax3)
#ax3.set_xlabel('Velocity (m/s)')
#ax3.set_ylabel('A.U.')
##ax3.set_xlim(-10,50)
#ax3.legend(['Location: %1.2f'%range1[2]+'-%1.2f'%range2[2]])
##ax3.set_xlim(0,100)
#
#fig, ax1 = plt.subplots(1,1)
#sns.distplot(data_loc1[:], hist=False, kde=True, color = 'darkblue',
#             hist_kws={'edgecolor':'black'},
#             kde_kws={'shade': True, 'linewidth': 2},ax=ax1)
#sns.distplot(data_loc2[:], hist=False, kde=True, color = 'red',
#             hist_kws={'edgecolor':'black'},
#             kde_kws={'shade': True, 'linewidth': 2},ax=ax1)
#sns.distplot(data_loc3[:], hist=False, kde=True, color = 'green',
#             hist_kws={'edgecolor':'black'},
#             kde_kws={'shade': True, 'linewidth': 2},ax=ax1)
#ax1.grid(True)
#ax1.set_xlabel('Velocity (m/s)')
##ax1.set_xlim(-10,50)
#ax1.set_ylabel('A.U.')
#ax1.set_title(funcName+'\n for pressure = %3.1e'%PRESS+' Torr')
#ax1.legend(['Location: %1.2f'%range1[0]+'-%1.2f'%range2[0],'Location: %1.2f'%range1[1]+'-%1.2f'%range2[1],'Location: %1.2f'%range1[2]+'-%1.2f'%range2[2]])


#fig, (ax1,ax2) = plt.subplots(2,1)
sns.jointplot("Vx", "Vy", data1, kind='kde');
sns.jointplot("Vx", "Vy", data2, kind='kde');
sns.jointplot("Vx", "Vy", data3, kind='kde');
sns.axes_style('white')

#ax1.set_xlabel('Velocity (m/s)')
##ax1.set_xlim(-10,50)
#ax1.set_ylabel('A.U.')
#ax1.set_title(funcName+'\n for pressure = %3.1e'%PRESS+' Torr')

plt.show()
