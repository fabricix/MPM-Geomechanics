# Planar direct shear verification

This folder contains figures and scripts for the two-stage planar direct shear verification: normal loading followed by shearing with a prescribed horizontal velocity. The inputs in this folder are `step 1.json` for normal loading and `Step 2.json` for shearing. The second stage loads the stress state saved by the first. The neighboring `01-normal-loading` and `02-direct-shear` directories contain the simulation outputs read by the plotting scripts.

- `direct-shear-model.png` and `direct-shear-model-3d.png`: initial particle geometry and Eulerian grid.
- `direct-shear-consolidation.png`: mean compressive stress and kinetic energy during normal loading.
- `direct-shear-velocity-check.png`: prescribed-particle velocity and displacement.
- `direct-shear-response.png`: shear stress from the prescribed-velocity reaction and relative vertical displacement.

