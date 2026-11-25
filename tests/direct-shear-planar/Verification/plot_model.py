"""Draw an X-Z slice of the initial particles and Eulerian grid for the manual."""
from pathlib import Path
import json
import xml.etree.ElementTree as ET
import numpy as np
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from matplotlib.lines import Line2D

here = Path(__file__).resolve().parent
stage = here.parent / '01-normal-loading'
d = json.loads((stage / 'input.json').read_text(encoding='utf-8'))
tree = ET.parse(stage / 'particles/particles_1.vtu')
xyz = np.fromstring(tree.find('.//Points/DataArray').text, sep=' ').reshape(-1, 3)
body = np.fromstring(tree.find('.//PointData/DataArray[@Name="Body Id"]').text, sep=' ').astype(int)
ys = np.unique(xyz[:, 1]); mask = np.isclose(xyz[:, 1], ys[0], atol=1e-9)
fig, ax = plt.subplots(figsize=(11, 5.2))
origin = np.array(d['mesh']['origin'])*1000
cell = np.array(d['mesh']['cells_dimension'])*1000
nx, _, nz = d['mesh']['cells_number']
for i in range(nx+1): ax.plot([origin[0]+i*cell[0]]*2, [origin[2],origin[2]+nz*cell[2]], color='#bbc5ce',lw=.55,zorder=0)
for i in range(nz+1): ax.plot([origin[0],origin[0]+nx*cell[0]], [origin[2]+i*cell[2]]*2, color='#bbc5ce',lw=.55,zorder=0)
for bid,color in ((1,'#23638e'),(2,'#d47c32')):
 selected=mask & (body==bid);ax.scatter(xyz[selected,0]*1000,xyz[selected,2]*1000,s=9,c=color,edgecolors='none',label=f'Block {bid} particles',zorder=2)
ax.set(xlim=(-1,121),ylim=(-1,61),xlabel='x (mm)',ylabel='z (mm)',title='Planar direct shear: initial particles and Eulerian grid')
ax.set_aspect('equal');ax.legend(handles=[Line2D([0],[0],marker='o',color='w',markerfacecolor=c,markersize=6,label=f'Block {i} particles') for i,c in ((1,'#23638e'),(2,'#d47c32'))]+[Line2D([0],[0],color='#bbc5ce',label='Eulerian grid (5 mm)')],loc='upper center',bbox_to_anchor=(.5,-.14),ncol=3,frameon=False)
fig.text(.5,.015,f'X-Z slice at y = {ys[0]*1000:g} mm; {mask.sum()} of {len(xyz)} particles shown.',ha='center',fontsize=9)
fig.tight_layout(rect=(0,.06,1,1))
out=here.parents[2]/'manual/latex/figures'
for ext in ('pdf','png'):fig.savefig(out/f'direct-shear-model.{ext}',dpi=200,bbox_inches='tight')
print(f'{mask.sum()} particles in slice, {len(xyz)} total')

# Three-dimensional view: all points and grid lines on the exterior faces.
fig = plt.figure(figsize=(11, 6.5))
ax = fig.add_subplot(111, projection='3d')
p = xyz * 1000
for bid, color in ((1, '#23638e'), (2, '#d47c32')):
    selected = body == bid
    ax.scatter(p[selected, 0], p[selected, 1], p[selected, 2], s=2.2,
               color=color, alpha=.6, depthshade=False, label=f'Block {bid} particles')
ny = d['mesh']['cells_number'][1]
xgrid = origin[0] + np.arange(nx + 1) * cell[0]
ygrid = origin[1] + np.arange(ny + 1) * cell[1]
zgrid = origin[2] + np.arange(nz + 1) * cell[2]
gridcolor = '#a3adb6'
for y in (ygrid[0], ygrid[-1]):
    for x in xgrid: ax.plot([x, x], [y, y], [zgrid[0], zgrid[-1]], color=gridcolor, lw=.45, alpha=.55)
    for z in zgrid: ax.plot([xgrid[0], xgrid[-1]], [y, y], [z, z], color=gridcolor, lw=.45, alpha=.55)
for x in (xgrid[0], xgrid[-1]):
    for z in zgrid: ax.plot([x, x], [ygrid[0], ygrid[-1]], [z, z], color=gridcolor, lw=.45, alpha=.55)
for z in (zgrid[0], zgrid[-1]):
    for x in xgrid: ax.plot([x, x], [ygrid[0], ygrid[-1]], [z, z], color=gridcolor, lw=.45, alpha=.55)
ax.set(xlim=(xgrid[0], xgrid[-1]), ylim=(ygrid[0], ygrid[-1]), zlim=(zgrid[0], zgrid[-1]),
       xlabel='x (mm)', ylabel='y (mm)', zlabel='z (mm)', title='Planar direct shear: 3D model and Eulerian grid')
ax.set_yticks([0, 10])
ax.set_box_aspect((120, 10, 60))
ax.view_init(elev=22, azim=-65)
ax.grid(False)
ax.legend(handles=[Line2D([0],[0],marker='o',color='w',markerfacecolor=c,markersize=6,label=f'Block {i} particles') for i,c in ((1,'#23638e'),(2,'#d47c32'))]+[Line2D([0],[0],color=gridcolor,label='Grid exterior (5 mm)')],loc='lower center',bbox_to_anchor=(.5,.02),ncol=3,frameon=False)
fig.tight_layout()
for ext in ('png', 'pdf'): fig.savefig(out/f'direct-shear-model-3d.{ext}',dpi=200,bbox_inches='tight')
