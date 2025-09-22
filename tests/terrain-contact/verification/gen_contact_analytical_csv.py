#!/usr/bin/env python3

# Generates a CSV file containing the analytical solution for the vertical position z(t) of a block falling over a inclined plane with friction.
# Example usage:
# python gen_contact_analytical_csv.py --mu 0.25 --theta 20 --out myfile.csv

import argparse
import math
import csv
from pathlib import Path
import sys

def main():
    ap = argparse.ArgumentParser(
        description="Generate CSV of analytical displacement x(t) along an inclined plane with friction."
    )
    ap.add_argument("--mu", type=float, default=0.30, help="Friction coefficient (default: 0.30)")
    ap.add_argument("--theta", type=float, default=30.0, help="Incline angle in degrees (default: 30)")
    ap.add_argument("--g", type=float, default=9.81, help="Gravity acceleration [m/s^2] (default: 9.81)")
    ap.add_argument("--T", type=float, default=2.0, help="Total simulation time [s] (default: 2.0)")
    ap.add_argument("--dt", type=float, default=0.001, help="Time step [s] (default: 0.001)")
    ap.add_argument("--out", type=str, default=None, help="Output CSV file name (auto-generated if not provided)")
    args = ap.parse_args()

    theta_rad = math.radians(args.theta)
    ax = args.g * math.sin(theta_rad) - args.mu * args.g * math.cos(theta_rad)

    # Warn if block does not slide
    if ax <= 0.0:
        print(f"[WARN] ax = {ax:.9f} m/s^2 ≤ 0 ⇒ no sliding along the plane for mu={args.mu}, theta={args.theta}°.", file=sys.stderr)

    n = int(round(args.T / args.dt)) + 1

    # Auto filename if not provided
    if args.out is None:
        out_name = f"analytical_contact_x_mu{args.mu:.2f}_theta{int(args.theta)}.csv"
    else:
        out_name = args.out
    out_path = Path(out_name).resolve()

    with out_path.open("w", newline="") as f:
        w = csv.writer(f)
        w.writerow(["t", "x", "vx", "ax"])
        for i in range(n):
            t = i * args.dt
            x = 0.5 * ax * t * t
            vx = ax * t
            w.writerow([f"{t:.6f}", f"{x:.9f}", f"{vx:.9f}", f"{ax:.9f}"])

    print(f"CSV generated: {out_path}")
    print(f"Parameters: mu={args.mu}, theta={args.theta}°, g={args.g}, T={args.T}, dt={args.dt}")
    print(f"Tangential acceleration ax = {ax:.9f} m/s²")

if __name__ == "__main__":
    main()
