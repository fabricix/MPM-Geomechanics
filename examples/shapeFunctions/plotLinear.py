#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Fri Apr 23 14:26:00 2021

@author: Fabricio Fernandez 
@e-mail: fabricio.hmf@gmail.com

"""
import numpy as np

    
# linear shape function
def NiShapeFunction(L,lp,x,xI):
    
    if(abs(x-xI)>=L):
        return 0
    
    if(-L<(x-xI) and (x-xI)<=0):
        return 1+(x-xI)/L
    
    if(0<(x-xI) and (x-xI)<L):
        return 1-(x-xI)/L

# shape function derivate
def GNiShapeFunction(L,lp,x,xI):
    
    if(abs(x-xI)>=L):
        return 0
    
    if(-L<(x-xI) and (x-xI)<=0):
        return 1/L
    
    if(0<(x-xI) and (x-xI)<L):
        return -1/L
    
# create a plot
import matplotlib.pyplot as plt
plt.close("all")
fig,ax=plt.subplots(1,1)

# load the MPM++ results 
import pandas as pd
sh1=pd.read_csv("shapeLinear.csv")

# plot shape function and gradients in x
ax.plot(sh1.x,sh1.sx,'--b',label='Sx-MPM++')
ax.plot(sh1.x,sh1.gx,'--r',label='Gx-MPM++')
ax.legend()

# plot shape function and gradients in y
ax.plot(sh1.y,sh1.sy,':b',label='Sy-MPM++')
ax.plot(sh1.y,sh1.gy,':r',label='Gy-MPM++')
ax.legend()

# plot shape function and gradients in z
ax.plot(sh1.z,sh1.sz,'-.b',label='Sz-MPM++')
ax.plot(sh1.z,sh1.gz,'-.r',label='Gz-MPM++')
ax.legend()

#
# Results comparison
#

# particle size
L = 1.0

# particle characteristic length
lp = 0.25

# particle position
xp = np.linspace(-3,3,300)

# node position  
xi = 0.5

# vectors to calculate the shape functions and its gradients
Ni=np.zeros_like(xp) 
dNi=np.zeros_like(xp)
Ni2=np.zeros_like(xp) 
dNi2=np.zeros_like(xp)

# performs the computation
for i in np.arange(xp.size):
    
    # analytical solution
    Ni[i] = NiShapeFunction(L,lp,xp[i],xi)
    dNi[i]  = GNiShapeFunction(L,lp,xp[i],xi)

# plot results
ax.plot(xp,Ni,'xb',label='S-MPM-Py')
ax.plot(xp,dNi,'xr',label='G-MPM-Py')

# add legends and show the figure
ax.legend()
plt.show()