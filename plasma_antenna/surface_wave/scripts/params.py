import numpy as np
from langmuir import *
import scipy.constants as sc

mAr	   = 6.6335209E-26
epsilonR   = 5
pDen	   = 1E15
tEeV       = 3
tIeV	   = 0.3
tEK        = tEeV*11604.525
tIK        = tIeV*11604.525
Ni         = pDen
Ne     	   = pDen
driftE     = 7.0e+06
tE         = np.sqrt(2*(tEeV*sc.e)/sc.m_e)  # m/s sqrt(2*(Ti*Q)/mE)
tI         = np.sqrt(2*(tIeV*sc.e)/mAr)   # m/s sqrt(2*(Ti*Q)/mE)
wpE 	   = (8.98E3)*np.sqrt(Ne*1E-6)
freq       = 2.45e9
print(wpE)
omegaPe_th = np.sqrt((Ne*np.square(sc.e))/(sc.epsilon_0*sc.m_e))
print(omegaPe_th)
omegaPe_lan= Electron(n=pDen, eV=tEeV).omega_p
epsi = 1- ((np.square(omegaPe_th))/(np.square(freq)))
print(epsi)
ds         = 0.005 * sc.speed_of_light / omegaPe_th
DL_th      = np.sqrt((sc.epsilon_0*tEK*sc.Boltzmann)/(np.square(sc.e)*Ne))
DL_lan    = Electron(n=pDen, eV=tEeV).debye
if omegaPe_th>1E9:
    print('Plasma frequency (theory) = %e'%(omegaPe_th/1E9)+' GHz\n'+'Plasma frequency (langmuir lib) = %e'%(omegaPe_lan/1E9)+' GHz')
elif omegaPe_th>1E6 and omegaPe_th<1E9:
    print('Plasma frequency (theory) = %e'%(omegaPe_th/1E6)+' MHz\n'+'Plasma frequency (langmuir lib) = %e'%(omegaPe_lan/1E6)+' MHz')
else:
    print('Plasma frequency (theory) = %e'%(omegaPe_th/1E3)+' kHz\n'+'Plasma frequency (langmuir lib) = %e'%(omegaPe_lan/1E3)+' kHz')
print('Debye Length (theory) = %e'%DL_th+' m\n'+'Debye Length (langmuir lib)= %e'%DL_lan+' m')
