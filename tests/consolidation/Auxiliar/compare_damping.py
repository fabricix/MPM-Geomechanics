"""Compare the existing local and kinetic damping implementations on n20.

python -B tests/consolidation/Auxiliar/compare_damping.py --output C:/MPM/artifacts/consolidation-review/damping-n20
Add --exe PATH --run to regenerate inputs and run all cases sequentially.
Requires the n20/input.json baseline in the output parent directory.
"""
import argparse
import csv
import hashlib
import json
from pathlib import Path
import subprocess
import xml.etree.ElementTree as ET

from verify_consolidation import analytical, read_state
import matplotlib.pyplot as plt
import numpy as np

CASES = {"alpha_0": ("None", {"type": "none"}),
         "alpha_005": ("Local alpha=0.05", {"type": "local", "alpha": .05}),
         "alpha_010": ("Local alpha=0.10", {"type": "local", "alpha": .1}),
         "alpha_020": ("Local alpha=0.20", {"type": "local", "alpha": .2}),
         "alpha_050": ("Local alpha=0.50", {"type": "local", "alpha": .5}),
         "kinetic": ("Kinetic relaxation", {"type": "kinetic"})}


def analyze(root):
    rows, metrics = [], {}
    fig, axes = plt.subplots(2, 2, figsize=(13, 9))
    profiles, pa_axes = plt.subplots(1, 3, figsize=(13, 4.6), sharey=True)
    for name, (label, damping) in CASES.items():
        folder = root / name
        d = json.loads((folder / "input.json").read_text())
        mat = d["material"]["elastic"]
        modulus = mat["young"] * (1 - mat["poisson"]) / ((1 + mat["poisson"]) * (1 - 2 * mat["poisson"]))
        cv = mat["hydraulic_conductivity"][2] / (mat["density_fluid"] * 9.81 * (1 / modulus + mat["porosity"] / mat["bulk_fluid"]))
        scale, p0, height = 1000 / modulus, 1000., 1.
        collection = ET.parse(folder / "particles/particleTimeSerie.pvd").findall(".//DataSet")
        history, snapshots = [], []
        finite, active, lateral = True, True, 0.
        for index, item in enumerate(collection):
            time = 0. if index == 0 else float(item.attrib["timestep"]) + d["time_step"]
            xyz, u, fields = read_state(folder / "particles" / item.attrib["file"])
            z = (xyz - u)[:, 2]
            p = fields["Pressure"]
            pa, ua = analytical(z, time, height, p0, modulus, cv)
            finite &= bool(np.isfinite(xyz).all() and np.isfinite(u).all()
                           and all(np.isfinite(v).all() for v in fields.values()))
            active &= bool(np.all(fields["Particle Active"] == 1))
            lateral = max(lateral, float(np.abs(u[:, :2]).max()))
            top = z > .987
            row = {"case": name, "time_s": time, "Tv": cv * time,
                   "pressure_rmse_over_p0": float(np.sqrt(np.mean((p-pa)**2)) / p0),
                   "mean_pressure_over_p0": float(p.mean() / p0),
                   "mean_reference_over_p0": float(pa.mean() / p0),
                   "settlement_top_m": float(-u[top, 2].mean()),
                   "settlement_reference_m": float(-ua[top].mean()),
                   "displacement_rmse_over_final_settlement": float(np.sqrt(np.mean((u[:, 2]-ua)**2)) / scale),
                   "min_pressure_Pa": float(p.min()), "max_pressure_Pa": float(p.max())}
            history.append(row)
            snapshots.append((time, z, p))
        evaluated = [r for r in history if r["Tv"] >= .05]
        early = [r for r in evaluated if r["Tv"] <= .3]
        late = [r for r in evaluated if r["Tv"] > .3]
        metrics[name] = {"damping": damping, "all_finite": finite, "all_active": active,
            "max_lateral_displacement_m": lateral,
            "max_pressure_rmse_over_p0_Tv_ge_005": max(r["pressure_rmse_over_p0"] for r in evaluated),
            "early_mean_pressure_rmse_over_p0_Tv_005_to_03": float(np.mean([r["pressure_rmse_over_p0"] for r in early])),
            "late_mean_pressure_rmse_over_p0_Tv_gt_03": float(np.mean([r["pressure_rmse_over_p0"] for r in late])),
            "last_pressure_rmse_over_p0": history[-1]["pressure_rmse_over_p0"],
            "max_displacement_rmse_over_final_settlement_Tv_ge_005": max(r["displacement_rmse_over_final_settlement"] for r in evaluated),
            "last_settlement_top_m": history[-1]["settlement_top_m"],
            "last_settlement_reference_m": history[-1]["settlement_reference_m"],
            "last_physical_output_s": history[-1]["time_s"]}
        rows.extend(history)
        tv = np.array([r["Tv"] for r in history])
        mean = [r["mean_pressure_over_p0"] for r in history]
        axes[0, 0].plot(tv, mean, label=label)
        axes[0, 1].plot(tv, mean, label=label)
        axes[1, 0].plot(tv, [r["settlement_top_m"]*1e6 for r in history], label=label)
        axes[1, 1].plot(tv, [r["pressure_rmse_over_p0"]*100 for r in history], label=label)
        if name == "alpha_0":
            for ax in axes[0]:
                ax.plot(tv, [r["mean_reference_over_p0"] for r in history], "k--", label="Analytical")
            axes[1, 0].plot(tv, [r["settlement_reference_m"]*1e6 for r in history], "k--", label="Analytical")
        for ax, target in zip(pa_axes, [.1, .5, 1.]):
            time, z, p = min(snapshots, key=lambda s: abs(s[0] * cv-target))
            zs = np.unique(np.round(z, 5))
            pm = [p[np.abs(z-zz)<2e-5].mean() / p0 for zz in zs]
            ax.plot(pm, zs, label=label)
            if name == "alpha_0":
                zz = np.linspace(0, 1, 200)
                pp, _ = analytical(zz, time, 1., p0, modulus, cv)
                ax.plot(pp/p0, zz, "k--", label="Analytical at sampled time")
            ax.set(title=f"Tv approximately {target}", xlabel="p / p0")
    axes[0, 0].set(title="Early pressure evolution", ylabel="Mean p / p0", xlim=(0, .3))
    axes[0, 1].set(title="Pressure evolution", ylabel="Mean p / p0")
    axes[1, 0].set(title="Settlement of upper particle layer", ylabel="Settlement (micrometres)")
    axes[1, 1].set(title="Pressure profile error", ylabel="RMSE / p0 (%)")
    for ax in axes.flat:
        ax.set_xlabel("Tv = cv t / H squared")
        ax.grid(alpha=.3)
        ax.legend(fontsize=7)
    pa_axes[0].set_ylabel("Initial height z / H")
    for ax in pa_axes:
        ax.grid(alpha=.3)
        ax.legend(fontsize=7)
    fig.tight_layout()
    profiles.tight_layout()
    fig.savefig(root / "comparison.png", dpi=180)
    profiles.savefig(root / "pressure_profiles.png", dpi=180)
    plt.close("all")
    with (root / "comparison.csv").open("w", newline="") as f:
        writer = csv.DictWriter(f, fieldnames=list(rows[0]))
        writer.writeheader()
        writer.writerows(rows)
    (root / "metrics.json").write_text(json.dumps(metrics, indent=2)+"\n")
    print(json.dumps(metrics, indent=2))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--exe", type=Path)
    parser.add_argument("--run", action="store_true")
    args = parser.parse_args()
    root = args.output.resolve()
    root.mkdir(parents=True, exist_ok=True)
    if args.run:
        if not args.exe:
            parser.error("--exe is required with --run")
        exe = args.exe.resolve()
        baseline = json.loads((root.parent / "n20/input.json").read_text())
        for name, (_, damping) in CASES.items():
            d = json.loads(json.dumps(baseline))
            d["results"]["print"] = 800
            d["damping"] = damping
            folder = root / name
            folder.mkdir(exist_ok=True)
            (folder / "input.json").write_text(json.dumps(d, indent=2)+"\n")
            with (folder / "execution.log").open("w") as log:
                subprocess.run([str(exe), "input.json"], cwd=folder, stdout=log,
                               stderr=subprocess.STDOUT, check=True)
        (root / "run_metadata.json").write_text(json.dumps({
            "executable": str(exe), "sha256": hashlib.sha256(exe.read_bytes()).hexdigest(),
            "base_case": "n20", "result_count": 800, "solver_modified": False,
            "cases": {name: damping for name, (_, damping) in CASES.items()}}, indent=2)+"\n")
    analyze(root)


if __name__ == "__main__":
    main()
