import numpy as np
import math 

Q   = 1.60217662e-19
mAr	     = 6.6335209E-26
mE 		 = 9.11E-31
speedLight = 2.9979e8
EPS0       = 8.85418782e-12
EPSR       = 5
KB = 1.38e-23
PLA_DEN	 = 1E14
Te         =   0.003
Ti 		 = 0.3
Ni      	 = PLA_DEN
Ne 	    = 1.0 * PLA_DEN
DRIFTE    = 7.0e+06//1E6
TE        = math.sqrt(2*(Te*Q)/mE)
TI        = math.sqrt(2*(Ti*Q)/mAr)  
wpe       = (8.98E3)*math.sqrt(Ne*1E-6)
PLASMA_FREQ_E  = math.sqrt((Ne*Q*Q)/(EPS0*mE))/(2*math.pi)
ds         = 0.005 * speedLight / PLASMA_FREQ_E
Freq = 2.45e9
DL = math.sqrt((EPS0*TE*KB)/(Ne*Q*Q)) 
pRNum = 20
pLNum = 100
gNum = 10
plasmaRad = (DL*pRNum) 
gThick    = DL*gNum              
sysRad    = plasmaRad + gThick  
plasmaLen = (DL*pLNum)   

print("DL=",DL)    
print("plasmaLen=",plasmaLen)  
print("sysRad=",sysRad)
print("TE=",TE)
