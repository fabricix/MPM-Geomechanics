"""Generate the planar direct-shear verification plots from saved VTK outputs.

Run: python generate_plots.py [--output-dir PATH]
Requires numpy and matplotlib. Defaults to this script's directory.
The displayed times come directly from the PVD files; no solver.log is needed.
"""
from pathlib import Path
import argparse
import json
import xml.etree.ElementTree as ET

import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import numpy as np

HERE = Path(__file__).resolve().parent
RUN = HERE.parent


def vtk(path):
    tree = ET.parse(path)
    fields = {e.get('Name'): np.fromstring(e.text or '', sep=' ').reshape(
        -1, int(e.get('NumberOfComponents', '1')))
        for e in tree.findall('.//PointData/DataArray')}
    points = tree.find('.//Points/DataArray')
    xyz = None if points is None else np.fromstring(points.text or '', sep=' ').reshape(-1, 3)
    return fields, xyz


def frames(path):
    return ET.parse(path).findall('.//DataSet')


def extract(stage_name):
    stage = RUN / stage_name
    config = json.loads((stage / 'input.json').read_text(encoding='utf-8'))
    series = frames(stage / 'particles/particleTimeSerie.pvd')
    grids = frames(stage / 'grid/gridTimeSerie.pvd') if 'prescribed_velocity' in config else None
    # Some solver outputs append an extra final grid frame with timestamp zero.
    if grids is not None and len(grids) == len(series) + 1 and float(grids[-1].get('timestep')) == 0:
        grids = grids[:-1]
    if not series or (grids is not None and len(grids) != len(series)):
        raise ValueError(f'Incomplete or mismatched particle/grid series: {stage}')
    first, xyz = vtk(stage / 'particles' / series[0].get('file'))
    initial = xyz - first['Displacement']
    ids = first['Particle Id'].ravel()
    body = first['Body Id'].ravel()
    mass = first['Mass'].ravel()
    load = config['load_distributed_box'][0]
    loaded = np.all(initial >= load['point_p1'], axis=1) & np.all(initial <= load['point_p2'], axis=1)
    interior = ~loaded
    lower = config['body'][0]
    area = np.ptp(np.array(lower['points'])[:, 0]) * lower['extrude_displacement']
    if not interior.any() or area <= 0:
        raise ValueError('Invalid averaging region or interface area')
    condition = config.get('prescribed_velocity', [None])[0]
    driven = base = None
    if condition:
        driven = ((body == condition['body_id']) & np.all(initial >= condition['point_p1'], axis=1)
                  & np.all(initial <= condition['point_p2'], axis=1))
        base = ((body == lower['id']) & (initial[:, 2] < np.min(np.array(lower['points'])[:, 2])
                + config['mesh']['cells_dimension'][2] / 2))
        if not driven.any() or not base.any():
            raise ValueError('Empty driven or base particle selection')
    rows = []
    for i, frame in enumerate(series):
        a, _ = vtk(stage / 'particles' / frame.get('file'))
        if not np.array_equal(ids, a['Particle Id'].ravel()) or not np.all(a['Particle Active'] == 1):
            raise ValueError(f'Particle ordering changed or particles became inactive in {frame.get("file")}')
        t = float(frame.get('timestep'))
        compression = -np.average(a['Stress_ZZ'].ravel()[interior], weights=mass[interior]) / 1e6
        vmin = vmax = ux = relative_z = force = 0.0
        if condition:
            if abs(t - float(grids[i].get('timestep'))) > 1e-9:
                raise ValueError('Particle and grid frames do not match')
            g, _ = vtk(stage / 'grid' / grids[i].get('file'))
            force = g['prescribed_velocity_reaction_1'][:, 0].sum()
            vmin = a['Velocity'][driven, 0].min()
            vmax = a['Velocity'][driven, 0].max()
            ux = np.average(a['Displacement'][driven, 0], weights=mass[driven])
            relative_z = (np.average(a['Displacement'][driven, 2], weights=mass[driven])
                          - np.average(a['Displacement'][base, 2], weights=mass[base]))
        rows.append((t, compression, vmin, vmax, ux, relative_z, force / area / 1e6))
    return np.array(rows), config, area


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output-dir', type=Path, default=HERE)
    args = parser.parse_args()
    args.output_dir.mkdir(parents=True, exist_ok=True)
    normal, normal_cfg, area = extract('01-normal-loading')
    shear, shear_cfg, _ = extract('02-direct-shear')
    velocity = shear_cfg['prescribed_velocity'][0]['velocity'][0]
    nominal = -normal_cfg['load_distributed_box'][0]['load'][2] / area / 1e6
    mu = shear_cfg['material_contact']['pairs'][0]['friction']
    energy = np.genfromtxt(RUN / '01-normal-loading/time-energy.csv', delimiter=',', names=True)
    plt.rcParams.update({'font.size': 10, 'axes.spines.top': False, 'axes.spines.right': False})
    fig, ax = plt.subplots(1, 2, figsize=(10, 3.5))
    ax[0].plot(normal[:, 0] * 1000, normal[:, 1], color='#1565a0')
    ax[0].axhline(nominal, color='#555', ls='--', label='Nominal load / area')
    ax[0].set(xlabel='Normal-loading time (ms)', ylabel='Mean compressive stress (MPa)'); ax[0].legend(fontsize=8)
    ax[1].semilogy(energy['time'][1:] * 1000, np.maximum(energy['energy'][1:], 1e-35), color='#007b67')
    ax[1].set(xlabel='Normal-loading time (ms)', ylabel='Kinetic energy (J)')
    for a in ax: a.grid(alpha=.2)
    fig.tight_layout(); fig.savefig(args.output_dir / 'direct-shear-consolidation.png', dpi=200); plt.close(fig)
    fig, ax = plt.subplots(1, 2, figsize=(10, 3.5))
    ax[0].plot(shear[1:, 0]*1000, shear[1:, 2]*1000, label='Saved minimum and maximum', color='#1565a0')
    ax[0].plot(shear[1:, 0]*1000, shear[1:, 3]*1000, color='#1565a0')
    ax[0].axhline(velocity*1000, color='#bb4d22', ls='--', label=f'Prescribed: {velocity*1000:g} mm/s')
    ax[0].set(xlabel='Shearing time (ms)', ylabel='Driven-particle velocity (mm/s)'); ax[0].legend(fontsize=8)
    ax[1].plot(shear[:, 0]*1000, shear[:, 4]*1000, label='Saved mean displacement', color='#1565a0')
    ax[1].plot(shear[:, 0]*1000, velocity*shear[:, 0]*1000, ls='--', color='#bb4d22', label='Prescribed: v t')
    ax[1].set(xlabel='Shearing time (ms)', ylabel='Horizontal displacement (mm)'); ax[1].legend(fontsize=8)
    for a in ax: a.grid(alpha=.2)
    fig.tight_layout(); fig.savefig(args.output_dir / 'direct-shear-velocity-check.png', dpi=200); plt.close(fig)
    fig, ax = plt.subplots(1, 2, figsize=(10, 3.7))
    x = velocity * shear[:, 0] * 1000
    ax[0].plot(x, shear[:, 6]); ax[0].axhline(mu * nominal, color='gray', ls='--', label=f'Friction x nominal stress = {mu*nominal:g} MPa')
    ax[0].set_ylabel('Shear stress (MPa)'); ax[0].legend(fontsize=8)
    ax[1].plot(x, (shear[:, 5] - shear[0, 5])*1000); ax[1].set_ylabel('Mean relative vertical displacement (mm)')
    for a in ax: a.set_xlabel('Prescribed horizontal displacement (mm)'); a.grid(alpha=.2)
    fig.tight_layout(); fig.savefig(args.output_dir / 'direct-shear-response.png', dpi=200); plt.close(fig)
    print(f'Wrote three plots to {args.output_dir}; frames: {len(normal)} normal, {len(shear)} shear.')


if __name__ == '__main__':
    main()
