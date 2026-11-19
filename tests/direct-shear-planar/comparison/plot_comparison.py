from pathlib import Path
import json,re,sys,xml.etree.ElementTree as ET
import numpy as np
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt

ROOT=Path(__file__).resolve().parents[1]
CASES=[('grid-5mm/02-direct-shear','h = 5 mm; v = 0,05 m/s'),
       ('grid-5mm/03-direct-shear-v001-t005','h = 5 mm; v = 0,01 m/s'),
       ('grid-2,5mm/02-direct-shear','h = 2,5 mm; v = 0,05 m/s')]

def read(path):
    tree=ET.parse(path)
    arrays={e.get('Name'):np.fromstring(e.text or '',sep=' ').reshape(-1,int(e.get('NumberOfComponents','1')))
            for e in tree.findall('.//PointData/DataArray')}
    assert all(np.isfinite(a).all() for a in arrays.values()),str(path)
    return tree,arrays

def extract(stage):
    d=json.loads((stage/'input.json').read_text());condition=d['prescribed_velocity'][0]
    assert len(d['prescribed_velocity'])==1
    raw=(stage/'solver.log').read_bytes()
    log=raw.decode('utf-16' if raw.startswith((b'\xff\xfe',b'\xfe\xff')) else 'utf-8',errors='replace')
    assert 'Elapsed time:' in log,'Simulation is incomplete: '+str(stage)
    dt=float(re.search(r'Time step\s*:\s*([\d.eE+-]+)',log).group(1))
    frames=ET.parse(stage/'particles/particleTimeSerie.pvd').findall('.//DataSet')
    gridframes=ET.parse(stage/'grid/gridTimeSerie.pvd').findall('.//DataSet')
    initial,a=read(stage/'particles'/frames[0].get('file'))
    x0=np.fromstring(initial.find('.//Points/DataArray').text,sep=' ').reshape(-1,3)-a['Displacement']
    ids=a['Body Id'].ravel();mass=a['Mass'].ravel();particle_ids=a['Particle Id'].ravel() if 'Particle Id' in a else None
    driven=(ids==condition['body_id'])&np.all(x0>=condition['point_p1'],axis=1)&np.all(x0<=condition['point_p2'],axis=1)
    lower=d['body'][0];lower_points=np.array(lower['points'])
    base=(ids==lower['id'])&(x0[:,2]<lower_points[:,2].min()+d['mesh']['cells_dimension'][2]/2)
    area=np.ptp(lower_points[:,0])*lower['extrude_displacement']
    assert driven.any() and base.any() and area>0
    rows=[]
    for index,frame in enumerate(frames):
        _,a=read(stage/'particles'/frame.get('file'))
        assert np.array_equal(ids,a['Body Id'].ravel())
        if particle_ids is not None:assert np.array_equal(particle_ids,a['Particle Id'].ravel())
        assert np.all(a['Particle Active']==1)
        gridframe=gridframes[index]
        assert abs(float(frame.get('timestep'))-float(gridframe.get('timestep')))<1e-10
        _,grid=read(stage/'grid'/gridframe.get('file'))
        force=grid['prescribed_velocity_reaction_1'][:,0].sum()
        time=float(frame.get('timestep'))+(dt if index else 0)
        relative_z=np.average(a['Displacement'][driven,2],weights=mass[driven])-np.average(a['Displacement'][base,2],weights=mass[base])
        rows.append([condition['velocity'][0]*time*1000,force/area/1e6,relative_z*1e6])
    rows=np.array(rows);rows[:,2]-=rows[0,2]
    print(stage.relative_to(ROOT),'frames',len(rows),'area',area,'controlled',driven.sum(),
          'max stress MPa',rows[:,1].max(),'last vertical um',rows[-1,2],flush=True)
    return rows,d

def draw(rows,d,folder):
    folder.mkdir(exist_ok=True)
    for index,name in [(1,'tensao-de-cisalhamento'),(2,'deslocamento-vertical')]:
        fig,ax=plt.subplots(figsize=(8,5))
        ax.plot(rows[:,0],rows[:,index],lw=1.4,color='#1565c0' if index==1 else '#047857',
                label='tens\u00e3o de cisalhamento' if index==1 else None)
        if index==1:
            ax.axhline(.6,color='gray',ls='--',lw=1,label=r'$\mu\sigma_n$ nominal = 0,6 MPa');ax.legend(fontsize=9)
        else:ax.axhline(0,color='gray',lw=.7)
        decorate(ax,index)
        ax.set_title(f"Malha: {d['mesh']['cells_dimension'][0]*1000:g} mm; v = {d['prescribed_velocity'][0]['velocity'][0]:g} m/s; t = {d['time']:g} s")
        fig.tight_layout()
        for ext in ['png','pdf']:fig.savefig(folder/f'{name}.{ext}',dpi=180)
        plt.close(fig)

def decorate(ax,index):
    ax.set_xlabel('Deslocamento horizontal prescrito Vt (mm)')
    ax.set_ylabel('Tens\u00e3o de cisalhamento (MPa)' if index==1 else 'Deslocamento vertical médio (µm)')
    ax.set_xlim(left=0);ax.grid(alpha=.25)

data=[]
for path,label in CASES:
    if len(sys.argv)>1 and path not in sys.argv[1:]:continue
    stage=ROOT/path;rows,d=extract(stage);draw(rows,d,stage/'graficos');data.append((label,rows))
if len(data)==3:
    folder=ROOT/'comparison';folder.mkdir(exist_ok=True)
    for index,name in [(1,'tensao-de-cisalhamento'),(2,'deslocamento-vertical')]:
        fig,ax=plt.subplots(figsize=(9,5.5))
        for label,rows in data:ax.plot(rows[:,0],rows[:,index],lw=1.3,label=label)
        if index==1:ax.axhline(.6,color='gray',ls='--',lw=1,label=r'$\mu\sigma_n$ nominal')
        else:ax.axhline(0,color='gray',lw=.7)
        decorate(ax,index);ax.legend(fontsize=9);fig.tight_layout()
        for ext in ['png','pdf']:fig.savefig(folder/f'{name}.{ext}',dpi=180)
        plt.close(fig)
