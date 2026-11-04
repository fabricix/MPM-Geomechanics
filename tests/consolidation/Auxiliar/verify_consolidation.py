"""Run and compare a prepressurized 1D column with its diffusion solution.

Usage:
  python tests/consolidation/Auxiliar/verify_consolidation.py --exe PATH_TO_EXE --output PATH_TO_RUNS
  python tests/consolidation/Auxiliar/verify_consolidation.py --output PATH_TO_RUNS --analyze-only

Requires numpy and matplotlib. Each solver runs in its own output directory.
The user's consolidation.json and the solver sources are never overwritten.
"""

import argparse
import copy
import csv
import hashlib
import json
from pathlib import Path
import subprocess
import xml.etree.ElementTree as ET

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import numpy as np


HERE = Path(__file__).resolve().parent
CASES = {"n20": (20, 1e-5), "n40": (40, 5e-6),
         "n40_dt_half": (40, 2.5e-6), "n20_slow": (20, 1e-5),
         "n40_slow": (40, 5e-6)}
SOURCE = "https://docs.itascacg.com/pfc600/flac3d/zone/test3d/Fluid/1DConsolidation/1dconsolidation.html"


def prepare(output):
    sketch = json.loads((HERE / "consolidation.json").read_text())
    for name, (cells, dt) in CASES.items():
        d = copy.deepcopy(sketch)
        d.pop("NO_load_distributed_box", None)
        d.update(time=0.16, time_step=dt, n_threads=1, damping={"type": "none"})
        if name.endswith("_slow"):
            d["time"] = 1.6
            d["material"]["elastic"]["hydraulic_conductivity"] = [0.0001] * 3
        d["mesh"].update(cells_dimension=[1, 1, 1 / cells], cells_number=[1, 1, cells])
        d["body"]["soil"]["point_p2"] = [1, 1, 1]
        d["initial_pressure_box"] = {"initial": {
            "point_p1": [0, 0, 0], "point_p2": [1, 1, 1], "pressure": 1000}}
        # One uppermost particle layer, assigned at setup and kept prescribed.
        top = {"point_p1": [0, 0, 1 - 0.26 / cells], "point_p2": [1, 1, 1]}
        d["prescribed_pressure_box"] = {"drain": {**top, "pressure": 0}}
        # This API takes TOTAL FORCE (N), not traction (Pa). Area = 1 m2.
        d["load_distributed_box"] = {"top_load": {**top, "load": [0, 0, -1000]}}
        d["results"] = {"print": 80, "material_point_results": [
            "id", "displacement", "pressure", "stress", "velocity", "active"],
            "grid_nodal_results": ["none"]}
        folder = output / name
        folder.mkdir(parents=True, exist_ok=True)
        (folder / "input.json").write_text(json.dumps(d, indent=2) + "\n")


def analytical(z, t, height, p0, constrained_modulus, cv):
    """Pressure and displacement increment from the initially pressurized state.

    Base z=0 is fixed/impervious; p(H)=0; q=p0 is constant; alpha=1.
    The initially effective-stress-free configuration is the displacement datum.
    """
    if t == 0:
        return np.full_like(z, p0), np.zeros_like(z)
    m = np.arange(400)
    a = (2 * m + 1) * np.pi / 2
    decay = np.exp(-a * a * cv * t / height**2)
    phase = np.outer(z / height, a)
    pressure = 2 * p0 * (np.cos(phase) @ (((-1.)**m / a) * decay))
    integral = 2 * p0 * height * (np.sin(phase) @ (((-1.)**m / a**2) * decay))
    displacement = (integral - p0 * z) / constrained_modulus
    return pressure, displacement


def read_state(path):
    tree = ET.parse(path)
    fields = {a.attrib["Name"]: np.fromstring(a.text, sep=" ")
              for a in tree.findall(".//PointData/DataArray")}
    xyz = np.fromstring(tree.find(".//Points/DataArray").text, sep=" ").reshape(-1, 3)
    displacement = fields["Displacement"].reshape(-1, 3)
    return xyz, displacement, fields


def analyze(output):
    metrics = {}
    fig, axes = plt.subplots(1, 3, figsize=(16, 4.6))
    profile_fig, profile_axes = plt.subplots(1, 3, figsize=(13, 4.6), sharey=True)
    all_rows = []
    for name, (cells, dt) in CASES.items():
        folder = output / name
        d = json.loads((folder / "input.json").read_text())
        material = d["material"]["elastic"]
        young, nu = material["young"], material["poisson"]
        modulus = young * (1 - nu) / ((1 + nu) * (1 - 2 * nu))
        storage = 1 / modulus + material["porosity"] / material["bulk_fluid"]
        cv = material["hydraulic_conductivity"][2] / (material["density_fluid"] * 9.81 * storage)
        p0, height = 1000., 1.
        settlement_scale = p0 * height / modulus
        collection = ET.parse(folder / "particles/particleTimeSerie.pvd").findall(".//DataSet")
        times, means, mean_refs, settlements, settlement_refs, errors = [], [], [], [], [], []
        snapshots = []
        all_finite, all_active, max_lateral = True, True, 0.
        for i, item in enumerate(collection):
            label_t = float(item.attrib["timestep"])
            # The current solver writes the UPDATED state before advancing iTime.
            # The initial snapshot is unadvanced. Do not change solver code here.
            t = 0. if i == 0 else label_t + dt
            xyz, u, fields = read_state(folder / "particles" / item.attrib["file"])
            z = (xyz - u)[:, 2]
            p = fields["Pressure"]
            pa, ua = analytical(z, t, height, p0, modulus, cv)
            all_finite &= bool(np.isfinite(xyz).all() and np.isfinite(u).all()
                               and all(np.isfinite(v).all() for v in fields.values()))
            all_active &= bool(np.all(fields["Particle Active"] == 1))
            max_lateral = max(max_lateral, float(np.abs(u[:, :2]).max()))
            top = z > height - 0.26 / cells
            error = float(np.sqrt(np.mean((p - pa)**2)) / p0)
            uz_error = float(np.sqrt(np.mean((u[:, 2] - ua)**2)) / settlement_scale)
            times.append(t)
            means.append(float(p.mean() / p0))
            mean_refs.append(float(pa.mean() / p0))
            settlements.append(float(-u[top, 2].mean()))
            settlement_refs.append(float(-ua[top].mean()))
            errors.append(error)
            snapshots.append((t, z, p, pa))
            all_rows.append({"case": name, "label_time_s": label_t, "physical_time_s": t,
                "Tv": cv * t, "mean_pressure_Pa": float(p.mean()),
                "mean_reference_Pa": float(pa.mean()), "pressure_rmse_over_p0": error,
                "displacement_rmse_over_final_settlement": uz_error,
                "top_layer_settlement_m": settlements[-1],
                "top_layer_reference_m": settlement_refs[-1],
                "min_pressure_Pa": float(p.min()), "max_pressure_Pa": float(p.max())})
        eligible = [r for r in all_rows if r["case"] == name and r["Tv"] >= 0.05]
        max_pressure_error = max(r["pressure_rmse_over_p0"] for r in eligible)
        max_displacement_error = max(r["displacement_rmse_over_final_settlement"] for r in eligible)
        late = [r for r in eligible if r["Tv"] >= 0.3]
        passed = bool(all_finite and all_active and max_lateral < 1e-8
                      and max_pressure_error < 0.05 and max_displacement_error < 0.05)
        metrics[name] = {"cells_z": cells, "particles": len(z), "dt_s": dt,
            "constrained_modulus_Pa": modulus, "storage_per_Pa": storage,
            "cv_m2_per_s": cv, "characteristic_time_s": height**2 / cv,
            "last_physical_output_s": times[-1], "all_finite": all_finite,
            "all_active": all_active, "max_lateral_displacement_m": max_lateral,
            "max_pressure_rmse_over_p0_after_Tv_0_05": max_pressure_error,
            "max_displacement_rmse_over_final_settlement_after_Tv_0_05": max_displacement_error,
            "max_pressure_rmse_over_p0_after_Tv_0_3": max(
                (r["pressure_rmse_over_p0"] for r in late), default=None),
            "last_pressure_rmse_over_p0": errors[-1], "passed": passed}
        axes[0].plot(np.array(times) * cv, means, label=name)
        axes[1].plot(np.array(times) * cv, np.array(settlements) * 1e6, label=name)
        axes[2].plot(np.array(times) * cv, np.array(errors) * 100, label=name)
        if name == "n40_dt_half":
            axes[0].plot(np.array(times) * cv, mean_refs, "k--", label="Analytical, particle sampling")
            axes[1].plot(np.array(times) * cv, np.array(settlement_refs) * 1e6, "k--", label="Analytical, top particle layer")
        for ax, target in zip(profile_axes, [0.1, 0.5, 1.]):
            if times[-1] * cv < target:
                continue
            t, z, p, pa = min(snapshots, key=lambda s: abs(s[0] * cv - target))
            zs = np.unique(np.round(z, 5))
            pm = np.array([p[np.abs(z - zz) < 2e-5].mean() for zz in zs])
            ax.plot(pm / p0, zs, ".-", ms=2, label=name)
            if name == "n40_dt_half":
                zz = np.linspace(0, 1, 200)
                pp, _ = analytical(zz, target / cv, height, p0, modulus, cv)
                ax.plot(pp / p0, zz, "k--", label="Analytical at target Tv")
            ax.set(title=f"Tv approximately {target}", xlabel="p / p0")
    for ax, title, ylabel in zip(axes,
            ["Mean pressure at particle positions", "Settlement of upper particle layer", "Pressure profile error"],
            ["mean(p) / p0", "Settlement (micrometres)", "RMSE / p0 (%)"]):
        ax.set(title=title, xlabel="Tv = cv t / H squared", ylabel=ylabel)
        ax.grid(alpha=.3)
        ax.legend(fontsize=7)
    profile_axes[0].set_ylabel("Initial height z / H")
    for ax in profile_axes:
        ax.grid(alpha=.3)
        ax.legend(fontsize=7)
    fig.tight_layout()
    profile_fig.tight_layout()
    fig.savefig(output / "comparison.png", dpi=180)
    profile_fig.savefig(output / "pressure_profiles.png", dpi=180)
    plt.close("all")
    with (output / "comparison.csv").open("w", newline="") as f:
        writer = csv.DictWriter(f, fieldnames=list(all_rows[0]))
        writer.writeheader()
        writer.writerows(all_rows)
    (output / "metrics.json").write_text(json.dumps(metrics, indent=2) + "\n")
    print(json.dumps(metrics, indent=2))
    return all(m["passed"] for m in metrics.values())


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--exe", type=Path)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--analyze-only", action="store_true")
    args = parser.parse_args()
    output = args.output.resolve()
    output.mkdir(parents=True, exist_ok=True)
    if not args.analyze_only:
        if not args.exe:
            parser.error("--exe is required unless --analyze-only is used")
        exe = args.exe.resolve()
        prepare(output)
        (output / "run_metadata.json").write_text(json.dumps({
            "executable": str(exe), "sha256": hashlib.sha256(exe.read_bytes()).hexdigest(),
            "reference": SOURCE, "time_label_correction_s": "+dt for noninitial outputs",
            "acceptance": "RMSE pressure/p0 and displacement/(p0 H / M) < 5% for Tv >= 0.05; finite, active, lateral displacement < 1e-8 m"}, indent=2))
        for name in CASES:
            with (output / name / "execution.log").open("w") as log:
                subprocess.run([str(exe), "input.json"], cwd=output / name,
                               stdout=log, stderr=subprocess.STDOUT, check=True)
    raise SystemExit(0 if analyze(output) else 1)


if __name__ == "__main__":
    main()
