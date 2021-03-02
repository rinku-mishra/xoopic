import matplotlib.pyplot as plt
import numpy as np
import seaborn as sns
from pylab import *
import cmath
# numerical data file
# CAUTION!! Run the eiedf_data_gen.sh file first
DIR="/home/rinku/Documents/Plasma_antenna/New_data/den_1e15"
DATA = "Electron"
PlotType = "VDF" #EDF or VDF
# numerical data file

if DATA=="Ion":
    filename1=DIR+'/iedf_ux_x.txt'
    filename2=DIR+'/iedf_ux_y.txt'
    funcName = "IEDF"
    mass =6.6335209E-26 #Argon
elif DATA=="Electron":
    filename1=DIR+'/eedf_ux_x.txt'
    filename2=DIR+'/eedf_ux_y.txt'
    funcName = "EEDF"
    mass = 9.1E-31 #electron
else:
    raise Exception("Error in input DATA TYPE")



charge_electron = 1.6e-19
KE_in_EV_coeff = 0.5*mass/charge_electron
data_act = np.loadtxt(filename1, delimiter="\t", skiprows=2, usecols=[0,2])
data_y = np.loadtxt(filename2, delimiter="\t", skiprows=2, usecols=[0])


#velocity data within a range
yrange = [0.01,0.05] #Put start and end values of y from sampling space
range1 = np.array([0.09,0.14,0.20])
range2 = np.array([0.13,0.19,0.26])
data1 = data_act[:,0] #space data
datay = data_y #space data
data2 = data_act[:,1] #velocity data
#disp(range2[2])
#Empty data array to store velocity data for a location range
data_loc1 = np.empty([len(data1), 1], dtype=float)
data_loc1[:] = np.NaN
data_loc2 = np.empty([len(data1), 1], dtype=float)
data_loc2[:] = np.NaN
data_loc3 = np.empty([len(data1), 1], dtype=float)
data_loc3[:] = np.NaN
j = 0
k = 0
l = 0
#storing data for the range given
for i in range(len(data1)):
    if (data1[i] >= range1[0]) and \
       (data1[i] <= range2[0]) and \
       (datay[i] >= yrange[0]) and \
       (datay[i] <= yrange[1]) :
        data_loc1[j]= data2[i]
        j = j+1
    elif (data1[i] >= range1[1]) and \
         (data1[i] <= range2[1]) and \
         (datay[i] >= yrange[0]) and \
         (datay[i] <= yrange[1]) :
          data_loc2[k]= data2[i]
          k = k+1
    elif (data1[i] >= range1[2]) and \
         (data1[i] <= range2[2]) and \
         (datay[i] >= yrange[0]) and \
         (datay[i] <= yrange[1]) :
          data_loc3[l]= data2[i]
          l = l+1
    else:
        None
#Conversion of velocities to energy
#data in eV


if PlotType == "EDF":
    data_loc1 = data_loc1*data_loc1
    data_EV_loc1 = KE_in_EV_coeff*data_loc1
    data_loc2 = data_loc2*data_loc2
    data_EV_loc2 = KE_in_EV_coeff*data_loc2
    data_loc3 = data_loc3*data_loc3
    data_EV_loc3 = KE_in_EV_coeff*data_loc3
else:
    data_EV_loc1 = data_loc1
    data_EV_loc2 = data_loc2
    data_EV_loc3 = data_loc3

fig, (ax1,ax2,ax3) = plt.subplots(3,1)
plt.rcParams["font.size"] = "13"
plt.rcParams["font.family"] = "sans serif"
sns.distplot(data_EV_loc1[:], hist=True, kde=True, color = 'darkblue',
             hist_kws={'edgecolor':'black'},
             kde_kws={'shade': True, 'linewidth': 2},ax=ax1)
if PlotType == "EDF":
    ax1.set_xlabel('Energy (eV)')
elif PlotType == "VDF":
    ax1.set_xlabel('$V_x$ (m/s)')
else:
    raise Exception("Error in Plot TYPE")

ax1.set_ylabel('A.U.')
#ax1.set_xlim(-1,10)
ax1.legend(['Location: %1.2f'%range1[0]+'-%1.2f'%range2[0]])
# ax1.set_title(DATA+' Energy Distribution Function('+funcName+') \n for pressure = %3.1e'%PRESS+' Torr')

sns.distplot(data_EV_loc2[:], hist=True, kde=True, color = 'red',
             hist_kws={'edgecolor':'black'},
             kde_kws={'shade': True, 'linewidth': 2},ax=ax2)
if PlotType == "EDF":
    ax2.set_xlabel('Energy (eV)')
elif PlotType == "VDF":
    ax2.set_xlabel('$V_x$ (m/s)')
else:
    raise Exception("Error in Plot TYPE")
ax2.set_ylabel('A.U.')
#ax2.set_xlim(-10,30)
ax2.legend(['Location: %1.2f'%range1[1]+'-%1.2f'%range2[1]])

sns.distplot(data_EV_loc3[:], hist=True, kde=True, color = 'green',
             hist_kws={'edgecolor':'black'},
             kde_kws={'shade': True, 'linewidth': 2},ax=ax3)
if PlotType == "EDF":
    ax3.set_xlabel('Energy (eV)')
elif PlotType == "VDF":
    ax3.set_xlabel('$V_x$ (m/s)')
else:
    raise Exception("Error in Plot TYPE")
ax3.set_ylabel('A.U.')
#ax3.set_xlim(-10,30)
ax3.legend(['Location: %1.2f'%range1[2]+'-%1.2f'%range2[2]])
#ax3.set_xlim(0,100)

fig, ax1 = plt.subplots(1,1)
sns.distplot(data_EV_loc1[:], hist=False, kde=True, color = 'darkblue',
             hist_kws={'edgecolor':'black'},
             kde_kws={'shade': True, 'linewidth': 2},ax=ax1)
sns.distplot(data_EV_loc2[:], hist=False, kde=True, color = 'red',
             hist_kws={'edgecolor':'black'},
             kde_kws={'shade': True, 'linewidth': 2},ax=ax1)
sns.distplot(data_EV_loc3[:], hist=False, kde=True, color = 'green',
             hist_kws={'edgecolor':'black'},
             kde_kws={'shade': True, 'linewidth': 2},ax=ax1)
ax1.grid(True)
if PlotType == "EDF":
    ax1.set_xlabel('Energy (eV)')
elif PlotType == "VDF":
    ax1.set_xlabel('$V_x$ (m/s)')
else:
    raise Exception("Error in Plot TYPE")

#ax1.set_xlim(left = 0)
ax1.set_ylabel('A.U.')
# ax1.set_title(DATA+' Energy Distribution Function('+funcName+') \n for pressure = %3.1e'%PRESS+' Torr')
ax1.legend(['Location: %1.2f'%range1[0]+'-%1.2f'%range2[0],'Location: %1.2f'%range1[1]+'-%1.2f'%range2[1],'Location: %1.2f'%range1[2]+'-%1.2f'%range2[2]])

plt.show()
