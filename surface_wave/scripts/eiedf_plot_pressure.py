import matplotlib.pyplot as plt
import numpy as np
import seaborn as sns
from pylab import *
import cmath
# numerical data file
# CAUTION!! Run the eiedf_data_gen.sh file first
DIR="/home/rinku/Documents/Plasma_antenna/New_data/den_1e15"
PRESS = 0.09
DATA = "Electron"
# numerical data file

if DATA=="Ion":
    filename1=DIR+'/iedf_vx.txt'
    funcName = "IEDF"
    mass =6.6335209E-26 #Argon
elif DATA=="Electron":
    filename1=DIR+'/eedf_vx.txt'
    funcName = "EEDF"
    mass = 9.1E-31 #electron
else:
    raise Exception("Error in input DATA TYPE")



charge_electron = 1.6e-19
KE_in_EV_coeff = 0.5*mass/charge_electron
data_act = np.loadtxt(filename1, delimiter="\t", skiprows=2, usecols=[0,2])


#velocity data within a range
range1 = np.array([0.09,0.14,0.24])
range2 = np.array([ 0.13,0.24,0.29])
data1 = data_act[:,0] #space data
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
    if (data1[i] >= range1[0]) and (data1[i] <= range2[0]):
        data_loc1[j]= data2[i]
        j = j+1
    elif (data1[i] >= range1[1]) and (data1[i] <= range2[1]):
        data_loc2[k]= data2[i]
        k = k+1
    elif (data1[i] >= range1[2]) and (data1[i] <= range2[2]):
        data_loc3[l]= data2[i]
        l = l+1
    else:
        None
#Conversion of velocities to energy
#data in eV

data_loc1 = data_loc1*data_loc1
data_EV_loc1 = KE_in_EV_coeff*data_loc1
data_loc2 = data_loc2*data_loc2
data_EV_loc2 = KE_in_EV_coeff*data_loc2
data_loc3 = data_loc3*data_loc3
data_EV_loc3 = KE_in_EV_coeff*data_loc3

fig, (ax1,ax2,ax3) = plt.subplots(3,1)
sns.distplot(data_EV_loc1[:], hist=False, kde=True, color = 'darkblue',
             hist_kws={'edgecolor':'black'},
             kde_kws={'shade': True, 'linewidth': 2},ax=ax1)
ax1.set_xlabel('Energy (eV)',fontsize = 15)
ax1.set_ylabel('A.U.',fontsize = 15)
#ax1.set_xlim(-10,50)
ax1.legend(['Location: %1.2f'%range1[0]+'-%1.2f'%range2[0]],fontsize = 15)
#ax1.set_title(DATA+' Energy Distribution Function('+funcName+') \n for pressure = 0.07 Torr',fontsize = 15)

sns.distplot(data_EV_loc2[:], hist=False, kde=True, color = 'red',
             hist_kws={'edgecolor':'black'},
             kde_kws={'shade': True, 'linewidth': 2},ax=ax2)
ax2.set_xlabel('Energy (eV)',fontsize = 15)
ax2.set_ylabel('A.U.',fontsize = 15)
#ax2.set_xlim(-10,50)
ax2.legend(['Location: %1.2f'%range1[1]+'-%1.2f'%range2[1]],fontsize = 15)

sns.distplot(data_EV_loc3[:], hist=False, kde=True, color = 'green',
             hist_kws={'edgecolor':'black'},
             kde_kws={'shade': True, 'linewidth': 2},ax=ax3)
ax3.set_xlabel('Energy (eV)',fontsize = 15)
ax3.set_ylabel('A.U.',fontsize = 15)
#ax3.set_xlim(-10,50)
ax3.legend(['Location: %1.2f'%range1[2]+'-%1.2f'%range2[2]],fontsize = 15)
#ax3.set_xlim(0,100)

#fig = plt.figure(figsize=(10,8))
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
ax1.set_xlabel('Energy (eV)',fontsize = 15)
plt.xticks(fontsize=12)
plt.yticks(fontsize=12)
#ax1.set_xlim(-0.1,0.6)
ax1.set_ylabel('A.U.',fontsize = 15)
#ax1.set_title(DATA+' Energy Distribution Function('+funcName+') \n for pressure = 0.09 Torr',fontsize = 15)
ax1.legend(['Location: %1.2f'%range1[0]+'-%1.2f'%range2[0],'Location: %1.2f'%range1[1]+'-%1.2f'%range2[1],'Location: %1.2f'%range1[2]+'-%1.2f'%range2[2]],fontsize = 15)

plt.show()
