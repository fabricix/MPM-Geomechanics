#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Fri Apr 23 14:26:00 2021

@author: Fabricio Fernandez 
@e-mail: fabricio.hmf@gmail.com

"""
import numpy as np

# sign function
def sgn(a):
    if a<0.0:
        return -1.0
    else:
        return 1.0
    
# functions
def NiGIMPShapeFunction(L,lp,xp,xi):
    
    if (xp-xi) <= (-L-lp):
        return 0
        
    if (-L-lp)<=(xp-xi) and (xp-xi)<=(-L+lp):
        return ((L+lp+(xp-xi))**2)/(4*L*lp)
    
    if (-L+lp)<=(xp-xi) and (xp-xi)<=(-lp):
        return 1 + (xp-xi)/L

    if (-lp)<=(xp-xi) and (xp-xi)<= (lp):
        return 1 - ((xp-xi)**2 + lp**2)/(2*L*lp)
    
    if (lp)<=(xp-xi) and (xp-xi)<= (L-lp):
        return 1-(xp-xi)/L

    if (L-lp)<=(xp-xi) and (xp-xi) <= (L+lp):
        return ((L+lp-(xp-xi))**2)/(4*L*lp)
        
    if (L+lp)<=(xp-xi):
        return 0

# functions
def GNiGIMPShapeFunction(L,lp,xp,xi):
    
    s = xp-xi

    if abs(s) >= (L+lp):
        return 0
    
    if (-L-lp<s<=-L+lp):
        return (L+lp+s)/(2*L*lp)
    
    if (-L+lp<s<=-lp):
        return 1/L
    
    if (-lp<s<=lp):
        return -s/L/lp
    
    if lp<s<=L-lp:
        return -1/L
    
    if (L-lp<s<=L+lp):
        return -(L+lp-s)/(2*L*lp)
    
    
# functions
def NiGIMPShapeFunctionMPM3D(L,lp,xp,xi):
    
    r = abs(xp-xi);
    l = lp;
    h = L;
    
    if(r<l):
        return(1.0-(r*r+l*l)/(2.*h*l))
    
    if(r<h-l):
        return(1.0-r/h)
    
    if(r<h+l):
        return((h+l-r)*(h+l-r)/(4.0*h*l))
      
    return 0.0;
     
# derivate of functions
def GNiGIMPShapeFunctionMPM3D(L,lp,xp,xi):
    
    x = xp-xi
    r = abs(x);
    l = lp;
    h = L;
    
    if(r<l):
        return(-x/(h*l))
    
    if(r<h-l):
        return(-sgn(x)/h)
    
    if(r<h+l):
        return((h+l-r)/(-2.0*sgn(x)*h*l))
      
    return 0.0;


import pandas as pd
sh1=pd.read_csv("shape.csv")

import matplotlib.pyplot as plt
plt.close("all")
fig,ax=plt.subplots(1,1)

# x
ax.plot(sh1.x,sh1.sx,'--b',label='Sx-MPM++')
ax.plot(sh1.x,sh1.gx,'--r',label='Gx-MPM++')
ax.legend()

# # y
ax.plot(sh1.y,sh1.sy,':b',label='Sy-MPM++')
ax.plot(sh1.y,sh1.gy,':r',label='Gy-MPM++')
ax.legend()

# # z
ax.plot(sh1.z,sh1.sz,'-.b',label='Sz-MPM++')
ax.plot(sh1.z,sh1.gz,'-.r',label='Gz-MPM++')
ax.legend()

# Plot verification

L = 1.0

# porticle caracteristic lenght
lp = 0.25

# particle position
xp = np.linspace(-3,3,300)

# node position  
xi = 0.5

# shape function of nodes
Ni = np.zeros_like(xp) 
dNi = np.zeros_like(xp)

Ni2 = np.zeros_like(xp) 
dNi2 = np.zeros_like(xp)

for i in np.arange(xp.size):
    
    Ni[i] = NiGIMPShapeFunction(L,lp,xp[i],xi)
    dNi[i]  = GNiGIMPShapeFunction(L,lp,xp[i],xi)
        
    Ni2[i] = NiGIMPShapeFunctionMPM3D(L,lp,xp[i],xi)
    dNi2[i] = GNiGIMPShapeFunctionMPM3D(L,lp,xp[i],xi)
        

ax.plot(xp,Ni,'xb',label='S-Zhuo Zhuang')
ax.plot(xp,dNi,'xr',label='G-Zhuo Zhuang')

ax.plot(xp,Ni,'+b',label='S-MPM-PUCRio')
ax.plot(xp,dNi,'+r',label='G-MPM-PUCRio')

ax.legend()

plt.show()