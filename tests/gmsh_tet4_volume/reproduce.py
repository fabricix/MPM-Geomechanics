#!/usr/bin/env python3
"""Build against an existing checkout; run JSON initialization checks locally.

No third-party Python packages, network, repository edits, or Git mutations.
Requires Linux/WSL, Python >=3.8, g++ with C++17 and OpenMP.
"""
import argparse
from concurrent.futures import ThreadPoolExecutor
import csv
import difflib
import hashlib
import json
from pathlib import Path
import shutil
import subprocess
import tempfile
from check_mesh import validate

HERE = Path(__file__).resolve().parent
CASES = [("cube_barycentric", 1.0, 6), ("cube_gaussian", 1.0, 24)]
CHANGES = [("addParticle(p, Mp/4.0, Vp/4.0);", "addParticle(p, Mp, Vp);")]

def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()

def run(argv, cwd, log, timeout=300):
    result = subprocess.run(argv, cwd=cwd, text=True, encoding="utf-8", errors="replace",
                            stdout=subprocess.PIPE, stderr=subprocess.STDOUT, timeout=timeout)
    log.write_text(result.stdout, encoding="utf-8")
    if result.returncode:
        raise RuntimeError("Command failed; inspect " + str(log))
    return result.stdout

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--repo", required=True, type=Path, help="Existing MPM-Geomechanics checkout")
    parser.add_argument("--compare-candidate", action="store_true",
                        help="Also compile the single-line TET4 candidate in a temporary copy; never edits --repo")
    parser.add_argument("--jobs", type=int, default=4, choices=range(1, 9))
    args = parser.parse_args()
    mesh_check = validate(HERE / "fixtures/unit_cube.msh")
    print("Mesh geometric volume:", mesh_check["sum_geometric_volume"], "tetrahedra:", mesh_check["tetrahedra"], flush=True)
    repo = args.repo.resolve()
    if not (repo / "src/BodyGmsh.cpp").is_file() or not (repo / "inc/Eigen/Core").is_file():
        parser.error("--repo must contain src/BodyGmsh.cpp and inc/Eigen/Core")
    compiler = shutil.which("g++")
    if not compiler:
        parser.error("g++ is required (C++17 and OpenMP)")
    # Include headers in the before/after fingerprint; original source is read only.
    original_files = sorted(p for folder in [repo/"src", repo/"inc"] for p in folder.rglob("*") if p.is_file())
    original_hashes = {p.relative_to(repo).as_posix(): digest(p) for p in original_files}
    runtime = HERE / ".runtime"
    runtime.mkdir(exist_ok=True)
    work = Path(tempfile.mkdtemp(prefix="run-", dir=str(runtime)))
    flags = ["-std=c++17", "-O1", "-g0", "-fopenmp"]
    for include in ["inc", "inc/Particle", "inc/Shape", "inc/Body", "inc/Materials", "inc/Mesh"]:
        flags += ["-I" + str(repo/include)]
    sources = sorted(p for p in (repo / "src").glob("*.cpp") if p.name != "Main.cpp")
    if not sources:
        parser.error("No upstream sources found")
    print("Compiling original sources in a new local directory...", flush=True)
    def compile_source(source):
        target = work / (source.stem + ".o")
        run([compiler, *flags, "-c", str(source), "-o", str(target)], work,
            work/(source.stem + ".compile.log"))
        return target
    with ThreadPoolExecutor(max_workers=args.jobs) as pool:
        objects = list(pool.map(compile_source, sources))
    harness = work / "volume_probe.o"
    run([compiler, *flags, "-c", str(HERE/"volume_probe.cpp"), "-o", str(harness)],
        work, work/"probe.compile.log")
    baseline = work / "baseline_probe"
    run([compiler, "-fopenmp", *map(str, objects), str(harness), "-o", str(baseline)],
        work, work/"baseline.link.log")
    def observe(binary, name):
        observations = []
        for label, exact_volume, count in CASES:
            result = subprocess.run([str(binary), label+".json", repr(exact_volume), str(count)],
                                    cwd=str(HERE/"fixtures"), text=True, encoding="utf-8",
                                    stdout=subprocess.PIPE, stderr=subprocess.STDOUT, timeout=30)
            (work / (name+"-"+label+".log")).write_text(result.stdout, encoding="utf-8")
            measurements = [line[len("MEASUREMENT "):] for line in result.stdout.splitlines()
                            if line.startswith("MEASUREMENT ")]
            if len(measurements) != 1 or result.returncode not in [0, 1]:
                raise RuntimeError("Invalid observation: " + label)
            row = json.loads(measurements[0])
            if result.returncode != (0 if row["mass_and_current_volume_ok"] else 1):
                raise RuntimeError("Inconsistent result status: " + label)
            row.update({"case": label, "probe_exit_code": result.returncode})
            observations.append(row)
            print(name, label, "current_volume=", row["current_volume"], "initial_volume=", row["initial_volume"],
                  "mass=", row["mass"], "expected=", row["expected_mass"],
                  "PASS" if row["mass_and_current_volume_ok"] else "FAIL", flush=True)
        return observations
    observed = {"baseline": observe(baseline, "baseline")}
    if args.compare_candidate:
        original = (repo/"src/BodyGmsh.cpp").read_text(encoding="utf-8")
        candidate = original
        for before, after in CHANGES:
            if candidate.count(before) != 1:
                raise RuntimeError("Candidate does not match this source version")
            candidate = candidate.replace(before, after)
        patch = "".join(difflib.unified_diff(original.splitlines(True), candidate.splitlines(True),
                                            fromfile="a/src/BodyGmsh.cpp", tofile="b/src/BodyGmsh.cpp"))
        (work/"candidate.patch").write_text(patch, encoding="utf-8")
        candidate_source = work / "candidate_BodyGmsh.cpp"
        candidate_source.write_text(candidate, encoding="utf-8")
        candidate_obj = work / "candidate_BodyGmsh.o"
        run([compiler, *flags, "-c", str(candidate_source), "-o", str(candidate_obj)],
            work, work/"candidate.compile.log")
        candidate_objects = [candidate_obj if p.name == "BodyGmsh.o" else p for p in objects]
        candidate_bin = work / "candidate_probe"
        run([compiler, "-fopenmp", *map(str, candidate_objects), str(harness), "-o", str(candidate_bin)],
            work, work/"candidate.link.log")
        observed["candidate"] = observe(candidate_bin, "candidate")
    after_hashes = {p.relative_to(repo).as_posix(): digest(p) for p in original_files}
    if original_hashes != after_hashes:
        raise RuntimeError("Original src/inc changed during this run")
    git_executable = shutil.which("git")
    git = subprocess.run([git_executable, "rev-parse", "HEAD"], cwd=repo, text=True,
                         capture_output=True) if git_executable else None
    source_fingerprint = hashlib.sha256(json.dumps(original_hashes, sort_keys=True).encode()).hexdigest()
    report = {"scope": "Unit cube with six TET4 generated by Gmsh; particle initialization via original Input and BodyGmsh; no time integration",
              "mesh_check": mesh_check,
              "volume_definition": "current_volume is sum(m/rho); initial_volume is sum(size.x*size.y*size.z), reported separately",
              "source_revision": git.stdout.strip() if git and git.returncode == 0 else None,
              "compiler": subprocess.check_output([compiler, "--version"], text=True).splitlines()[0],
              "flags": ["-std=c++17", "-O1", "-g0", "-fopenmp"],
              "rtol": 1e-12, "src_inc_unchanged": True,
              "src_inc_sha256": source_fingerprint,
              "key_sources_sha256": {name: original_hashes[name] for name in ["src/BodyGmsh.cpp", "src/Input.cpp", "inc/Particle/Particle.h"]},
              "observations": observed}
    (work/"results.json").write_text(json.dumps(report, indent=2), encoding="utf-8")
    for name, rows in observed.items():
        with (work/(name+".csv")).open("w", newline="", encoding="utf-8") as stream:
            writer = csv.DictWriter(stream, fieldnames=list(rows[0]))
            writer.writeheader()
            writer.writerows(rows)
    print("Results: " + str(work / "results.json"), flush=True)
    passed = all(row["mass_and_current_volume_ok"] for row in observed.get("candidate", observed["baseline"]))
    if args.compare_candidate:
        print("Candidate conservation: " + ("PASS" if passed else "FAIL") + "; source checkout unchanged.")
    else:
        print("Baseline conservation: " + ("PASS" if passed else "FAIL") + ". FAIL is expected on the reported version.")
    return 0 if passed else 1

if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (RuntimeError, OSError, subprocess.SubprocessError) as exc:
        print("Reproduction error: " + str(exc))
        raise SystemExit(2)
