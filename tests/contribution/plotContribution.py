#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Mon May 10 10:43:02 2021

@author: Fabricio Fernandez 
@e-mail: fabricio.hmf@gmail.com

"""

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

# load the MPM-Geomechanics results 
mpm_cpp=[]
mpm_cpp.append(pd.read_csv("particle0.csv"))
mpm_cpp.append(pd.read_csv("particle1.csv"))
mpm_cpp.append(pd.read_csv("particle2.csv"))

# load the MPM-PUCRio results
mpm_puc=[]
mpm_puc.append(pd.read_csv("particle0-MPM-PUCRio.csv"))
mpm_puc.append(pd.read_csv("particle1-MPM-PUCRio.csv"))
mpm_puc.append(pd.read_csv("particle2-MPM-PUCRio.csv"))

labels = mpm_cpp[0].node

x = np.arange(len(labels))  # the label locations
width = 0.3  # the width of the bars

plt.close('all')

for i in range(len(mpm_cpp)):

    fig, ax = plt.subplots()
    rects1 = ax.bar(x - width/2, mpm_cpp[i].w, width, label='MPM-Geomechanics',color='b')
    rects2 = ax.bar(x + width/2, mpm_puc[i].w, width, label='MPM-PUCRio',color='r')
    
    # Add some text for labels, title and custom x-axis tick labels, etc.
    ax.set_title('Particle %d'%i)
    ax.set_ylabel('Weights')
    ax.set_xlabel('Node')
    ax.set_xticks(x)
    ax.set_xticklabels(labels)
    ax.legend()
    
    #ax.bar_label(rects1, padding=3)
    #ax.bar_label(rects2, padding=3)
    
    fig.tight_layout()

plt.show()