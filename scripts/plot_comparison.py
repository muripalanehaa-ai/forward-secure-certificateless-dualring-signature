# # import pandas as pd
# # import matplotlib.pyplot as plt

# # # ----------------------------
# # # Styling (publication-like)
# # # ----------------------------
# # plt.rcParams.update({
# #     "font.size": 12,
# #     "axes.titlesize": 14,
# #     "axes.labelsize": 12,
# #     "legend.fontsize": 10
# # })

# # # ----------------------------
# # # Load CSVs
# # # ----------------------------
# # fs_clrs = pd.read_csv("results_ring_size_dynamic.csv")
# # clrs_pbc = pd.read_csv("results_ring_size_clrs_pbc.csv")
# # fsdr = pd.read_csv("results_ring_size_fsdr.csv")

# # fs_clrs.columns = ["ring_size", "sign_ms", "verify_ms", "sig_bytes"]
# # clrs_pbc.columns = ["ring_size", "sign_ms", "verify_ms", "sig_bytes"]
# # fsdr.columns = ["ring_size", "sign_ms", "verify_ms", "sig_bytes"]

# # # =========================================================
# # # 1. Signing Time vs Ring Size
# # # =========================================================
# # plt.figure(figsize=(10, 6))

# # plt.plot(fs_clrs["ring_size"], fs_clrs["sign_ms"], marker="o", label="FS-CLRS (Dynamic)")
# # plt.plot(clrs_pbc["ring_size"], clrs_pbc["sign_ms"], marker="o", label="CLRS (PBC)")
# # plt.plot(fsdr["ring_size"], fsdr["sign_ms"], marker="o", label="FSDRS")

# # plt.xscale("log", base=2)
# # plt.yscale("log")

# # plt.title("Figure 1: Signing Time vs Ring Size (Log-Log Scale)")
# # plt.xlabel("Ring Size (log scale)")
# # plt.ylabel("Signing Time (ms, log scale)")
# # plt.grid(True, which="both", linestyle="--", alpha=0.5)
# # plt.legend()

# # plt.tight_layout()
# # plt.savefig("N_signing_time_vs_ring_size.png", dpi=300)
# # plt.savefig("N_signing_time_vs_ring_size.pdf")
# # plt.show()

# # # =========================================================
# # # 2. Verification Time vs Ring Size
# # # =========================================================
# # plt.figure(figsize=(10, 6))

# # plt.plot(fs_clrs["ring_size"], fs_clrs["verify_ms"], marker="o", label="FS-CLRS (Dynamic)")
# # plt.plot(clrs_pbc["ring_size"], clrs_pbc["verify_ms"], marker="o", label="CLRS (PBC)")
# # plt.plot(fsdr["ring_size"], fsdr["verify_ms"], marker="o", label="FSDRS")

# # plt.xscale("log", base=2)
# # plt.yscale("log")

# # plt.title("Figure 2: Verification Time vs Ring Size (Log-Log Scale)")
# # plt.xlabel("Ring Size (log scale)")
# # plt.ylabel("Verification Time (ms, log scale)")
# # plt.grid(True, which="both", linestyle="--", alpha=0.5)
# # plt.legend()

# # plt.tight_layout()
# # plt.savefig("N_verification_time_vs_ring_size.png", dpi=300)
# # plt.savefig("N_verification_time_vs_ring_size.pdf")
# # plt.show()

# # # =========================================================
# # # 3. Signature Size vs Ring Size
# # # =========================================================
# # plt.figure(figsize=(10, 6))

# # plt.plot(fs_clrs["ring_size"], fs_clrs["sig_bytes"], marker="o", label="FS-CLRS (Dynamic)")
# # plt.plot(clrs_pbc["ring_size"], clrs_pbc["sig_bytes"], marker="o", label="CLRS (PBC)")
# # plt.plot(fsdr["ring_size"], fsdr["sig_bytes"], marker="o", label="FSDRS")

# # plt.xscale("log", base=2)
# # plt.yscale("log")

# # plt.title("Figure 3: Signature Size vs Ring Size (Log-Log Scale)")
# # plt.xlabel("Ring Size (log scale)")
# # plt.ylabel("Signature Size (bytes, log scale)")
# # plt.grid(True, which="both", linestyle="--", alpha=0.5)
# # plt.legend()

# # plt.tight_layout()
# # plt.savefig("N_signature_size_vs_ring_size.png", dpi=300)
# # plt.savefig("N_signature_size_vs_ring_size.pdf")
# # plt.show()

# # # =========================================================
# # # 4. Epoch Stability (Signing Time)
# # # =========================================================
# # fs_epoch = pd.read_csv("results_epochs_dynamic.csv")
# # clrs_epoch = pd.read_csv("results_epochs_clrs_pbc.csv")
# # fsdr_epoch = pd.read_csv("results_epochs_fsdr.csv")

# # plt.figure(figsize=(10, 6))

# # plt.plot(fs_epoch["epoch"], fs_epoch["sign_ms"], label="FS-CLRS")
# # plt.plot(clrs_epoch["epoch"], clrs_epoch["sign_ms"], label="CLRS (PBC)")
# # plt.plot(fsdr_epoch["epoch"], fsdr_epoch["sign_ms"], label="FSDRS")

# # plt.title("Figure 4: Epoch Stability of Signing Time")
# # plt.xlabel("Epoch")
# # plt.ylabel("Signing Time (ms)")
# # plt.grid(True, linestyle="--", alpha=0.5)
# # plt.legend()

# # plt.tight_layout()
# # plt.savefig("N_epoch_stability_signing_time.png", dpi=300)
# # plt.savefig("N_epoch_stability_signing_time.pdf")
# # plt.show()


# # import pandas as pd
# # import matplotlib.pyplot as plt

# # # ----------------------------
# # # Styling (publication-like)
# # # ----------------------------
# # plt.rcParams.update({
# #     "font.size": 12,
# #     "axes.titlesize": 14,
# #     "axes.labelsize": 12,
# #     "legend.fontsize": 10
# # })

# # # ----------------------------
# # # Load CSVs
# # # ----------------------------
# # fs_clrs = pd.read_csv("results_ring_size_dynamic.csv")
# # clrs_pbc = pd.read_csv("results_ring_size_clrs_pbc.csv")
# # fsdr = pd.read_csv("results_ring_size_fsdr.csv")

# # fs_clrs.columns = ["ring_size", "sign_ms", "verify_ms", "sig_bytes"]
# # clrs_pbc.columns = ["ring_size", "sign_ms", "verify_ms", "sig_bytes"]
# # fsdr.columns = ["ring_size", "sign_ms", "verify_ms", "sig_bytes"]

# # # =========================================================
# # # 1. Signing Time vs Ring Size
# # # =========================================================
# # plt.figure(figsize=(10, 6))

# # plt.plot(fs_clrs["ring_size"], fs_clrs["sign_ms"], marker="o", label="FS-CLRS (Dynamic)")
# # plt.plot(clrs_pbc["ring_size"], clrs_pbc["sign_ms"], marker="o", label="CLRS (PBC)")
# # plt.plot(fsdr["ring_size"], fsdr["sign_ms"], marker="o", label="FSDRS")

# # plt.xscale("log", base=2)
# # plt.yscale("log")

# # plt.title("Figure 1: Signing Time vs Ring Size (Log-Log Scale)")
# # plt.xlabel("Ring Size (log scale)")
# # plt.ylabel("Signing Time (ms, log scale)")
# # plt.grid(True, which="both", linestyle="--", alpha=0.5)
# # plt.legend()

# # plt.tight_layout()
# # plt.savefig("N_signing_time_vs_ring_size.png", dpi=300)
# # plt.savefig("N_signing_time_vs_ring_size.pdf")
# # plt.show()

# # # =========================================================
# # # 2. Verification Time vs Ring Size
# # # =========================================================
# # plt.figure(figsize=(10, 6))

# # plt.plot(fs_clrs["ring_size"], fs_clrs["verify_ms"], marker="o", label="FS-CLRS (Dynamic)")
# # plt.plot(clrs_pbc["ring_size"], clrs_pbc["verify_ms"], marker="o", label="CLRS (PBC)")
# # plt.plot(fsdr["ring_size"], fsdr["verify_ms"], marker="o", label="FSDRS")

# # plt.xscale("log", base=2)
# # plt.yscale("log")

# # plt.title("Figure 2: Verification Time vs Ring Size (Log-Log Scale)")
# # plt.xlabel("Ring Size (log scale)")
# # plt.ylabel("Verification Time (ms, log scale)")
# # plt.grid(True, which="both", linestyle="--", alpha=0.5)
# # plt.legend()

# # plt.tight_layout()
# # plt.savefig("N_verification_time_vs_ring_size.png", dpi=300)
# # plt.savefig("N_verification_time_vs_ring_size.pdf")
# # plt.show()

# # # =========================================================
# # # 3. Signature Size vs Ring Size
# # # =========================================================
# # plt.figure(figsize=(10, 6))

# # plt.plot(fs_clrs["ring_size"], fs_clrs["sig_bytes"], marker="o", label="FS-CLRS (Dynamic)")
# # plt.plot(clrs_pbc["ring_size"], clrs_pbc["sig_bytes"], marker="o", label="CLRS (PBC)")
# # plt.plot(fsdr["ring_size"], fsdr["sig_bytes"], marker="o", label="FSDRS")

# # plt.xscale("log", base=2)
# # plt.yscale("log")

# # plt.title("Figure 3: Signature Size vs Ring Size (Log-Log Scale)")
# # plt.xlabel("Ring Size (log scale)")
# # plt.ylabel("Signature Size (bytes, log scale)")
# # plt.grid(True, which="both", linestyle="--", alpha=0.5)
# # plt.legend()

# # plt.tight_layout()
# # plt.savefig("N_signature_size_vs_ring_size.png", dpi=300)
# # plt.savefig("N_signature_size_vs_ring_size.pdf")
# # plt.show()

# # # =========================================================
# # # 4. Epoch Stability (Signing Time)
# # # =========================================================
# # fs_epoch = pd.read_csv("results_epochs_dynamic.csv")
# # clrs_epoch = pd.read_csv("results_epochs_clrs_pbc.csv")
# # fsdr_epoch = pd.read_csv("results_epochs_fsdr.csv")

# # plt.figure(figsize=(10, 6))

# # plt.plot(fs_epoch["epoch"], fs_epoch["sign_ms"], label="FS-CLRS")
# # plt.plot(clrs_epoch["epoch"], clrs_epoch["sign_ms"], label="CLRS (PBC)")
# # plt.plot(fsdr_epoch["epoch"], fsdr_epoch["sign_ms"], label="FSDRS")

# # plt.title("Figure 4: Epoch Stability of Signing Time")
# # plt.xlabel("Epoch")
# # plt.ylabel("Signing Time (ms)")
# # plt.grid(True, linestyle="--", alpha=0.5)
# # plt.legend()

# # plt.tight_layout()
# # plt.savefig("N_epoch_stability_signing_time.png", dpi=300)
# # plt.savefig("N_epoch_stability_signing_time.pdf")
# # plt.show()



# import pandas as pd
# import matplotlib.pyplot as plt

# # ==========================================================
# # LOAD CSV FILES
# # ==========================================================

# # Forward Secure Dynamic Ring Signature (Pairing Based)
# fsdr_ring = pd.read_csv("results_ring_size_fsdr_pairing.csv")
# fsdr_epoch = pd.read_csv("results_epochs_fsdr_pairing.csv")
# fsdr_bench = pd.read_csv("results_bench_iter_fsdr_pairing.csv")

# # Forward Secure Certificateless Ring Signature
# fsclrs_ring = pd.read_csv("results_ring_size_dynamic.csv")
# fsclrs_epoch = pd.read_csv("results_epochs_dynamic.csv")
# fsclrs_bench = pd.read_csv("results_bench_iter_dynamic.csv")

# # Chow-Yap Certificateless Ring Signature (Pairing Based)
# clrs_ring = pd.read_csv("results_ring_size_clrs_pbc.csv")
# clrs_epoch = pd.read_csv("results_epochs_clrs_pbc.csv")
# clrs_bench = pd.read_csv("results_bench_iter_clrs_pbc.csv")


# # ==========================================================
# # PLOT FUNCTION
# # ==========================================================

# def plot_comparison(
#         x1, y1,
#         x2, y2,
#         x3, y3,
#         xlabel,
#         ylabel,
#         title,
#         output_file,
#         log_scale=False):

#     plt.figure(figsize=(10, 6))

#     plt.plot(
#         x1, y1,
#         marker='o',
#         linewidth=2,
#         label='FSDR Pairing-Based'
#     )

#     plt.plot(
#         x2, y2,
#         marker='s',
#         linewidth=2,
#         label='FS-CLRS'
#     )

#     plt.plot(
#         x3, y3,
#         marker='^',
#         linewidth=2,
#         label='CLRS-PBC'
#     )

#     plt.xlabel(xlabel, fontsize=12)
#     plt.ylabel(ylabel, fontsize=12)
#     plt.title(title, fontsize=14, fontweight='bold')

#     if log_scale:
#         plt.yscale("log")

#     plt.grid(True, linestyle='--', alpha=0.5)
#     plt.legend(fontsize=10)

#     plt.tight_layout()

#     filename = "R_" + output_file
#     plt.savefig(filename, dpi=300)

#     print(f"Saved: {filename}")

#     plt.show()


# # ==========================================================
# # GRAPH 1
# # Signing Time vs Ring Size
# # ==========================================================

# plot_comparison(
#     fsdr_ring["ring_size"], fsdr_ring["sign_ms"],
#     fsclrs_ring["ring_size"], fsclrs_ring["sign_ms"],
#     clrs_ring["ring_size"], clrs_ring["sign_ms"],
#     "Ring Size",
#     "Signing Time (ms)",
#     "Signing Time Comparison vs Ring Size",
#     "sign_vs_ring_size.png",
#     log_scale=True
# )

# # ==========================================================
# # GRAPH 2
# # Verification Time vs Ring Size
# # ==========================================================

# plot_comparison(
#     fsdr_ring["ring_size"], fsdr_ring["verify_ms"],
#     fsclrs_ring["ring_size"], fsclrs_ring["verify_ms"],
#     clrs_ring["ring_size"], clrs_ring["verify_ms"],
#     "Ring Size",
#     "Verification Time (ms)",
#     "Verification Time Comparison vs Ring Size",
#     "verify_vs_ring_size.png",
#     log_scale=True
# )

# # ==========================================================
# # GRAPH 3
# # Signature Size vs Ring Size
# # ==========================================================

# plot_comparison(
#     fsdr_ring["ring_size"], fsdr_ring["sig_bytes"],
#     fsclrs_ring["ring_size"], fsclrs_ring["sig_bytes"],
#     clrs_ring["ring_size"], clrs_ring["sig_bytes"],
#     "Ring Size",
#     "Signature Size (Bytes)",
#     "Signature Size Comparison vs Ring Size",
#     "signature_size_vs_ring_size.png",
#     log_scale=False
# )

# # ==========================================================
# # GRAPH 4
# # Signing Time vs Epoch
# # ==========================================================

# plot_comparison(
#     fsdr_epoch["epoch"], fsdr_epoch["sign_ms"],
#     fsclrs_epoch["epoch"], fsclrs_epoch["sign_ms"],
#     clrs_epoch["epoch"], clrs_epoch["sign_ms"],
#     "Epoch",
#     "Signing Time (ms)",
#     "Signing Time Comparison Across Epochs",
#     "sign_vs_epoch.png",
#     log_scale=True
# )

# # ==========================================================
# # GRAPH 5
# # Verification Time vs Epoch
# # ==========================================================

# plot_comparison(
#     fsdr_epoch["epoch"], fsdr_epoch["verify_ms"],
#     fsclrs_epoch["epoch"], fsclrs_epoch["verify_ms"],
#     clrs_epoch["epoch"], clrs_epoch["verify_ms"],
#     "Epoch",
#     "Verification Time (ms)",
#     "Verification Time Comparison Across Epochs",
#     "verify_vs_epoch.png",
#     log_scale=True
# )

# # ==========================================================
# # GRAPH 6
# # Signing Time vs Benchmark Iterations
# # ==========================================================

# plot_comparison(
#     fsdr_bench["bench_iters"], fsdr_bench["sign_ms"],
#     fsclrs_bench["bench_iters"], fsclrs_bench["sign_ms"],
#     clrs_bench["bench_iters"], clrs_bench["sign_ms"],
#     "Benchmark Iterations",
#     "Signing Time (ms)",
#     "Signing Time Comparison vs Benchmark Iterations",
#     "sign_vs_benchmark_iterations.png",
#     log_scale=True
# )

# # ==========================================================
# # GRAPH 7
# # Verification Time vs Benchmark Iterations
# # ==========================================================

# plot_comparison(
#     fsdr_bench["bench_iters"], fsdr_bench["verify_ms"],
#     fsclrs_bench["bench_iters"], fsclrs_bench["verify_ms"],
#     clrs_bench["bench_iters"], clrs_bench["verify_ms"],
#     "Benchmark Iterations",
#     "Verification Time (ms)",
#     "Verification Time Comparison vs Benchmark Iterations",
#     "verify_vs_benchmark_iterations.png",
#     log_scale=True
# )

# print("\n===================================")
# print("ALL COMPARISON FIGURES GENERATED")
# print("Files saved with prefix R_")
# print("===================================")


"""
E_Plot_Comparison_Full.py
=========================
Reads ALL CSV files from the Neha2 folder and generates every comparison plot.
All output PNGs are saved with the "E_" prefix in the same directory as this script.

CSV files expected (in same folder as this script):
  Ring Size:
    results_ring_size_dynamic.csv       → FS-CLRS Dynamic
    results_ring_size_fsdr_pairing.csv  → FS-PBC FSDR Pairing
    results_ring_size_clrs_pbc.csv      → CLRS PBC

  Epochs:
    results_epochs_dynamic.csv
    results_epochs_fsdr_pairing.csv
    results_epochs_clrs_pbc.csv

  Bench Iterations:
    results_bench_iter_dynamic.csv
    results_bench_iter_fsdr_pairing.csv
    results_bench_iter_clrs_pbc.csv

Usage:
    python E_Plot_Comparison_Full.py
    python E_Plot_Comparison_Full.py --outdir /path/to/save/plots
"""

import argparse
import os
import sys

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import matplotlib.ticker as mticker
import numpy as np
import pandas as pd

# ── CLI ────────────────────────────────────────────────────────────────────
parser = argparse.ArgumentParser(description="Generate all comparison plots.")
parser.add_argument(
    "--datadir",
    default=os.path.dirname(os.path.abspath(__file__)),
    help="Folder that contains all CSV files (default: same folder as this script)",
)
parser.add_argument(
    "--outdir",
    default=os.path.dirname(os.path.abspath(__file__)),
    help="Folder to save output PNGs (default: same folder as this script)",
)
args = parser.parse_args()

DATA = args.datadir
OUT  = args.outdir
os.makedirs(OUT, exist_ok=True)

# ── Scheme metadata ────────────────────────────────────────────────────────
SCHEMES = {
    "clrs":  {"label": "FS-CLRS Dynamic",       "color": "#2196F3", "marker": "o", "ls": "-"},
    "fsdr":  {"label": "FS-PBC FSDR Pairing",    "color": "#FF6B35", "marker": "s", "ls": "--"},
    "pbc":   {"label": "CLRS PBC (Chow & Yap)",  "color": "#4CAF50", "marker": "^", "ls": "-."},
}

CSV = {
    "ring": {
        "clrs": "results_ring_size_dynamic.csv",
        "fsdr": "results_ring_size_fsdr_pairing.csv",
        "pbc":  "results_ring_size_clrs_pbc.csv",
    },
    "epoch": {
        "clrs": "results_epochs_dynamic.csv",
        "fsdr": "results_epochs_fsdr_pairing.csv",
        "pbc":  "results_epochs_clrs_pbc.csv",
    },
    "bench": {
        "clrs": "results_bench_iter_dynamic.csv",
        "fsdr": "results_bench_iter_fsdr_pairing.csv",
        "pbc":  "results_bench_iter_clrs_pbc.csv",
    },
}

# ── Load CSVs ──────────────────────────────────────────────────────────────
def load(sweep, key):
    path = os.path.join(DATA, CSV[sweep][key])
    if not os.path.exists(path):
        print(f"  [WARN] File not found: {path}")
        return None
    df = pd.read_csv(path, skipinitialspace=True)
    df.columns = df.columns.str.strip().str.lower().str.replace(" ", "_")
    return df

ring  = {k: load("ring",  k) for k in SCHEMES}
epoch = {k: load("epoch", k) for k in SCHEMES}
bench = {k: load("bench", k) for k in SCHEMES}

# ── Global style ───────────────────────────────────────────────────────────
plt.rcParams.update({
    "figure.facecolor":  "#0F1117",
    "axes.facecolor":    "#1A1D27",
    "axes.edgecolor":    "#3A3D4D",
    "axes.labelcolor":   "#E0E0E0",
    "xtick.color":       "#A0A0A0",
    "ytick.color":       "#A0A0A0",
    "text.color":        "#E0E0E0",
    "grid.color":        "#2A2D3A",
    "grid.linewidth":    0.7,
    "legend.facecolor":  "#1A1D27",
    "legend.edgecolor":  "#3A3D4D",
    "legend.labelcolor": "#E0E0E0",
    "font.family":       "monospace",
    "axes.titlesize":    13,
    "axes.labelsize":    11,
})

saved = []

def save(name):
    path = os.path.join(OUT, f"E_{name}.png")
    plt.savefig(path, dpi=150, bbox_inches="tight",
                facecolor=plt.gcf().get_facecolor())
    plt.close()
    saved.append(f"E_{name}.png")
    print(f"  ✓  E_{name}.png")

def style_ax(ax, title, xlabel, ylabel, log_x=False, log_y=False):
    ax.set_title(title, pad=10, fontweight="bold", color="#FFFFFF")
    ax.set_xlabel(xlabel)
    ax.set_ylabel(ylabel)
    ax.grid(True, which="both", alpha=0.4)
    if log_x: ax.set_xscale("log", base=2)
    if log_y: ax.set_yscale("log")
    ax.spines[:].set_color("#3A3D4D")

def lines(ax, x_col, y_col, dfs, title, xlabel, ylabel,
          log_x=False, log_y=False):
    for k, m in SCHEMES.items():
        df = dfs[k]
        if df is None:
            continue
        ax.plot(df[x_col], df[y_col],
                color=m["color"], marker=m["marker"], linestyle=m["ls"],
                linewidth=1.8, markersize=5, label=m["label"])
    style_ax(ax, title, xlabel, ylabel, log_x, log_y)
    ax.legend(fontsize=8)

def moving_avg(arr, w=5):
    return np.convolve(arr, np.ones(w) / w, mode="valid")


# ══════════════════════════════════════════════════════════════════════════
#  SWEEP 1 — RING SIZE
# ══════════════════════════════════════════════════════════════════════════
print("\n── Sweep 1: Ring Size ──────────────────────────────────────────────")

# 1. Sign time — log-log line
fig, ax = plt.subplots(figsize=(8, 5))
lines(ax, "ring_size", "sign_ms", ring,
      "[Sweep 1] Sign Time vs Ring Size",
      "Ring Size (n)", "Sign Time (ms)", log_x=True, log_y=True)
save("sign_vs_ring_size")

# 2. Verify time — log-log line
fig, ax = plt.subplots(figsize=(8, 5))
lines(ax, "ring_size", "verify_ms", ring,
      "[Sweep 1] Verify Time vs Ring Size",
      "Ring Size (n)", "Verify Time (ms)", log_x=True, log_y=True)
save("verify_vs_ring_size")

# 3. Signature bytes — log-log line
fig, ax = plt.subplots(figsize=(8, 5))
lines(ax, "ring_size", "sig_bytes", ring,
      "[Sweep 1] Signature Size vs Ring Size",
      "Ring Size (n)", "Signature Size (bytes)", log_x=True, log_y=True)
save("signature_size_vs_ring_size")

# 4. Sign + Verify on same axes (two y-scales per scheme would be cluttered;
#    use a 1×2 subplot instead)
fig, axes = plt.subplots(1, 2, figsize=(14, 5))
for ax, op, col in zip(axes,
                        ["Sign", "Verify"],
                        ["sign_ms", "verify_ms"]):
    lines(ax, "ring_size", col, ring,
          f"[Sweep 1] {op} Time vs Ring Size",
          "Ring Size (n)", f"{op} Time (ms)", log_x=True, log_y=True)
plt.tight_layout()
save("sign_verify_vs_ring_size_duo")

# 5. Bar chart at snapshot ring sizes n=4, 64, 1024
snap_ns = [4, 64, 1024]
bar_w   = 0.25
x       = np.arange(len(snap_ns))
keys    = list(SCHEMES.keys())

fig, axes = plt.subplots(1, 2, figsize=(13, 5))
for ax, (op, col) in zip(axes, [("Sign","sign_ms"), ("Verify","verify_ms")]):
    for bi, k in enumerate(keys):
        df = ring[k]
        if df is None:
            continue
        vals = [float(df.loc[df["ring_size"]==n, col].values[0]) for n in snap_ns]
        bars = ax.bar(x + bi * bar_w, vals, bar_w,
                      label=SCHEMES[k]["label"], color=SCHEMES[k]["color"], alpha=0.85)
        for bar, val in zip(bars, vals):
            ax.text(bar.get_x() + bar.get_width()/2, bar.get_height()*1.04,
                    f"{val:.1f}", ha="center", va="bottom",
                    fontsize=7, color="#E0E0E0")
    ax.set_yscale("log")
    ax.set_xticks(x + bar_w)
    ax.set_xticklabels([f"n={n}" for n in snap_ns])
    style_ax(ax,
             f"[Sweep 1] {op} Time at n=4, 64, 1024",
             "Ring Size", f"{op} Time (ms, log)")
    ax.legend(fontsize=8)
plt.tight_layout()
save("ring_bar_snapshots")

# 6. Sign / Verify ratio
fig, ax = plt.subplots(figsize=(8, 5))
for k, m in SCHEMES.items():
    df = ring[k]
    if df is None:
        continue
    ratio = df["sign_ms"] / df["verify_ms"]
    ax.plot(df["ring_size"], ratio,
            color=m["color"], marker=m["marker"], linestyle=m["ls"],
            linewidth=1.8, markersize=5, label=m["label"])
ax.axhline(1.0, color="#888", linewidth=0.9, linestyle=":")
style_ax(ax,
         "[Sweep 1] Sign / Verify Time Ratio vs Ring Size",
         "Ring Size (n)", "Sign / Verify", log_x=True)
ax.legend(fontsize=8)
save("ring_sign_verify_ratio")

# 7. Throughput (ops / sec)
fig, axes = plt.subplots(1, 2, figsize=(14, 5))
for ax, (op, col) in zip(axes, [("Sign","sign_ms"), ("Verify","verify_ms")]):
    for k, m in SCHEMES.items():
        df = ring[k]
        if df is None:
            continue
        tps = 1000.0 / df[col]
        ax.plot(df["ring_size"], tps,
                color=m["color"], marker=m["marker"], linestyle=m["ls"],
                linewidth=1.8, markersize=5, label=m["label"])
    style_ax(ax,
             f"[Sweep 1] {op} Throughput vs Ring Size",
             "Ring Size (n)", "Throughput (ops/sec)", log_x=True, log_y=True)
    ax.legend(fontsize=8)
plt.tight_layout()
save("ring_throughput")

# 8. Bytes per ring member
fig, ax = plt.subplots(figsize=(8, 5))
for k, m in SCHEMES.items():
    df = ring[k]
    if df is None:
        continue
    bpr = df["sig_bytes"] / df["ring_size"]
    ax.plot(df["ring_size"], bpr,
            color=m["color"], marker=m["marker"], linestyle=m["ls"],
            linewidth=1.8, markersize=5, label=m["label"])
style_ax(ax,
         "[Sweep 1] Signature Bytes per Ring Member vs Ring Size",
         "Ring Size (n)", "Bytes per Member", log_x=True)
ax.legend(fontsize=8)
save("ring_bytes_per_member")

# 9. Relative sign-time heatmap (normalised to fastest scheme)
ring_sizes_all = ring["clrs"]["ring_size"].tolist() if ring["clrs"] is not None else []
if ring_sizes_all:
    sign_matrix = np.array([
        ring[k]["sign_ms"].tolist() for k in keys
        if ring[k] is not None
    ])
    valid_keys = [k for k in keys if ring[k] is not None]
    norm_matrix = sign_matrix / sign_matrix.min(axis=0, keepdims=True)

    fig, ax = plt.subplots(figsize=(12, 3.5))
    im = ax.imshow(norm_matrix, aspect="auto", cmap="RdYlGn_r",
                   vmin=1, vmax=norm_matrix.max())
    ax.set_xticks(range(len(ring_sizes_all)))
    ax.set_xticklabels([str(int(n)) for n in ring_sizes_all], fontsize=9)
    ax.set_yticks(range(len(valid_keys)))
    ax.set_yticklabels([SCHEMES[k]["label"] for k in valid_keys], fontsize=9)
    for i in range(len(valid_keys)):
        for j in range(len(ring_sizes_all)):
            ax.text(j, i, f"{norm_matrix[i,j]:.1f}×",
                    ha="center", va="center", fontsize=7.5,
                    color="black" if norm_matrix[i,j] < 80 else "white")
    fig.colorbar(im, ax=ax, label="Relative Sign Time  (1× = fastest)")
    ax.set_title("[Sweep 1] Relative Sign-Time Heatmap  (normalised to fastest per ring size)",
                 fontweight="bold", color="#FFFFFF", pad=8)
    ax.spines[:].set_color("#3A3D4D")
    save("ring_sign_heatmap")

# 10. Linear-scale ring size (small rings only, n ≤ 64)
fig, axes = plt.subplots(1, 2, figsize=(14, 5))
for ax, (op, col) in zip(axes, [("Sign","sign_ms"), ("Verify","verify_ms")]):
    for k, m in SCHEMES.items():
        df = ring[k]
        if df is None:
            continue
        sub = df[df["ring_size"] <= 64]
        ax.plot(sub["ring_size"], sub[col],
                color=m["color"], marker=m["marker"], linestyle=m["ls"],
                linewidth=1.8, markersize=6, label=m["label"])
    style_ax(ax,
             f"[Sweep 1] {op} Time vs Ring Size  (n ≤ 64, linear scale)",
             "Ring Size (n)", f"{op} Time (ms)")
    ax.legend(fontsize=8)
plt.tight_layout()
save("ring_linear_small")


# ══════════════════════════════════════════════════════════════════════════
#  SWEEP 2 — EPOCH
# ══════════════════════════════════════════════════════════════════════════
print("\n── Sweep 2: Epoch ──────────────────────────────────────────────────")

# Detect epoch column name (may be "epoch" or "epochs")
def epoch_col(df):
    for c in df.columns:
        if "epoch" in c:
            return c
    return df.columns[0]

# 11. Sign & verify vs epoch (side-by-side)
fig, axes = plt.subplots(1, 2, figsize=(14, 5))
for ax, (op, col) in zip(axes, [("Sign","sign_ms"), ("Verify","verify_ms")]):
    for k, m in SCHEMES.items():
        df = epoch[k]
        if df is None:
            continue
        ax.plot(df[epoch_col(df)], df[col],
                color=m["color"], marker=m["marker"], linestyle=m["ls"],
                linewidth=1.8, markersize=5, label=m["label"])
    style_ax(ax,
             f"[Sweep 2] {op} Time vs Epoch",
             "Epoch", f"{op} Time (ms)")
    ax.legend(fontsize=8)
plt.tight_layout()
save("sign_verify_vs_epoch")

# 12. Sign only
fig, ax = plt.subplots(figsize=(8, 5))
for k, m in SCHEMES.items():
    df = epoch[k]
    if df is None:
        continue
    ax.plot(df[epoch_col(df)], df["sign_ms"],
            color=m["color"], marker=m["marker"], linestyle=m["ls"],
            linewidth=1.8, markersize=5, label=m["label"])
style_ax(ax, "[Sweep 2] Sign Time vs Epoch", "Epoch", "Sign Time (ms)")
ax.legend(fontsize=8)
save("sign_vs_epoch")

# 13. Verify only
fig, ax = plt.subplots(figsize=(8, 5))
for k, m in SCHEMES.items():
    df = epoch[k]
    if df is None:
        continue
    ax.plot(df[epoch_col(df)], df["verify_ms"],
            color=m["color"], marker=m["marker"], linestyle=m["ls"],
            linewidth=1.8, markersize=5, label=m["label"])
style_ax(ax, "[Sweep 2] Verify Time vs Epoch", "Epoch", "Verify Time (ms)")
ax.legend(fontsize=8)
save("verify_vs_epoch")

# 14. Coefficient of variation (stability bar)
fig, ax = plt.subplots(figsize=(9, 5))
bar_labels, cv_sign, cv_verify = [], [], []
for k, m in SCHEMES.items():
    df = epoch[k]
    if df is None:
        continue
    bar_labels.append(m["label"])
    cv_sign.append(df["sign_ms"].std() / df["sign_ms"].mean() * 100)
    cv_verify.append(df["verify_ms"].std() / df["verify_ms"].mean() * 100)

x = np.arange(len(bar_labels))
w = 0.35
bars1 = ax.bar(x - w/2, cv_sign,   w, label="Sign CV",   color="#2196F3", alpha=0.85)
bars2 = ax.bar(x + w/2, cv_verify, w, label="Verify CV", color="#FF6B35", alpha=0.85)
for b in list(bars1) + list(bars2):
    ax.text(b.get_x() + b.get_width()/2, b.get_height() + 0.005,
            f"{b.get_height():.2f}%", ha="center", va="bottom", fontsize=8)
ax.set_xticks(x)
ax.set_xticklabels(bar_labels, rotation=10, ha="right", fontsize=9)
ax.set_ylabel("Coefficient of Variation (%)")
ax.set_title("[Sweep 2] Epoch Stability — Coefficient of Variation",
             fontweight="bold", color="#FFFFFF", pad=10)
ax.legend(fontsize=9)
ax.grid(axis="y", alpha=0.4)
ax.spines[:].set_color("#3A3D4D")
save("epoch_cv_bar")

# 15. Mean ± std band per scheme across epochs
fig, axes = plt.subplots(1, 2, figsize=(14, 5))
for ax, (op, col) in zip(axes, [("Sign","sign_ms"), ("Verify","verify_ms")]):
    for k, m in SCHEMES.items():
        df = epoch[k]
        if df is None:
            continue
        ep = df[epoch_col(df)].values
        vals = df[col].values
        mean_v = vals.mean()
        std_v  = vals.std()
        ax.plot(ep, vals, color=m["color"], marker=m["marker"],
                linestyle=m["ls"], linewidth=1.5, markersize=4, label=m["label"])
        ax.axhline(mean_v, color=m["color"], linewidth=0.8, linestyle=":")
        ax.fill_between(ep, mean_v - std_v, mean_v + std_v,
                        color=m["color"], alpha=0.10)
    style_ax(ax,
             f"[Sweep 2] {op} Time vs Epoch  (shaded = ±1σ)",
             "Epoch", f"{op} Time (ms)")
    ax.legend(fontsize=8)
plt.tight_layout()
save("epoch_mean_std_band")


# ══════════════════════════════════════════════════════════════════════════
#  SWEEP 3 — BENCHMARK ITERATIONS
# ══════════════════════════════════════════════════════════════════════════
print("\n── Sweep 3: Benchmark Iterations ───────────────────────────────────")

def bench_iter_col(df):
    for c in df.columns:
        if "iter" in c or "bench" in c:
            return c
    return df.columns[0]

# 16. Sign vs iterations
fig, ax = plt.subplots(figsize=(8, 5))
for k, m in SCHEMES.items():
    df = bench[k]
    if df is None:
        continue
    ax.plot(df[bench_iter_col(df)], df["sign_ms"],
            color=m["color"], linestyle=m["ls"],
            linewidth=1.5, label=m["label"], alpha=0.85)
style_ax(ax,
         "[Sweep 3] Sign Time vs Benchmark Iterations",
         "Benchmark Iterations", "Sign Time (ms)")
ax.legend(fontsize=8)
save("sign_vs_benchmark_iterations")

# 17. Verify vs iterations
fig, ax = plt.subplots(figsize=(8, 5))
for k, m in SCHEMES.items():
    df = bench[k]
    if df is None:
        continue
    ax.plot(df[bench_iter_col(df)], df["verify_ms"],
            color=m["color"], linestyle=m["ls"],
            linewidth=1.5, label=m["label"], alpha=0.85)
style_ax(ax,
         "[Sweep 3] Verify Time vs Benchmark Iterations",
         "Benchmark Iterations", "Verify Time (ms)")
ax.legend(fontsize=8)
save("verify_vs_benchmark_iterations")

# 18. Sign + Verify side-by-side
fig, axes = plt.subplots(1, 2, figsize=(14, 5))
for ax, (op, col) in zip(axes, [("Sign","sign_ms"), ("Verify","verify_ms")]):
    for k, m in SCHEMES.items():
        df = bench[k]
        if df is None:
            continue
        ax.plot(df[bench_iter_col(df)], df[col],
                color=m["color"], linestyle=m["ls"],
                linewidth=1.5, label=m["label"], alpha=0.85)
    style_ax(ax,
             f"[Sweep 3] {op} Time vs Benchmark Iterations",
             "Benchmark Iterations", f"{op} Time (ms)")
    ax.legend(fontsize=8)
plt.tight_layout()
save("sign_verify_vs_bench_iter_duo")

# 19. Moving-average smoothed (MA-5)
fig, axes = plt.subplots(1, 2, figsize=(14, 5))
for ax, (op, col) in zip(axes, [("Sign","sign_ms"), ("Verify","verify_ms")]):
    for k, m in SCHEMES.items():
        df = bench[k]
        if df is None:
            continue
        raw  = df[col].values
        xs   = df[bench_iter_col(df)].values
        ma   = moving_avg(raw, 5)
        x_ma = xs[2:-2]          # valid indices after MA-5
        ax.plot(xs, raw,
                color=m["color"], alpha=0.25, linewidth=0.8)
        ax.plot(x_ma, ma,
                color=m["color"], linestyle=m["ls"],
                linewidth=2.2, label=m["label"])
    style_ax(ax,
             f"[Sweep 3] {op} Time — Smoothed (MA-5)  (faded = raw)",
             "Benchmark Iterations", f"{op} Time (ms)")
    ax.legend(fontsize=8)
plt.tight_layout()
save("bench_smoothed_ma5")

# 20. Box plots
fig, axes = plt.subplots(1, 2, figsize=(12, 5))
for ax, (op, col) in zip(axes, [("Sign","sign_ms"), ("Verify","verify_ms")]):
    valid = [(k, bench[k]) for k in keys if bench[k] is not None]
    data  = [df[col].values for _, df in valid]
    bps   = ax.boxplot(data, patch_artist=True, notch=False, widths=0.5,
                       medianprops={"color": "#FFFFFF", "linewidth": 2})
    for patch, (k, _) in zip(bps["boxes"], valid):
        patch.set_facecolor(SCHEMES[k]["color"])
        patch.set_alpha(0.75)
    for comp in ("whiskers", "caps", "fliers"):
        for item in bps[comp]:
            item.set_color("#888888")
    ax.set_xticks(range(1, len(valid)+1))
    ax.set_xticklabels([SCHEMES[k]["label"] for k, _ in valid],
                       rotation=12, ha="right", fontsize=8)
    style_ax(ax,
             f"[Sweep 3] {op} Time Distribution (box plot)",
             "Scheme", f"{op} Time (ms)")
plt.tight_layout()
save("bench_boxplot")

# 21. Violin plots
fig, axes = plt.subplots(1, 2, figsize=(12, 5))
for ax, (op, col) in zip(axes, [("Sign","sign_ms"), ("Verify","verify_ms")]):
    valid = [(k, bench[k]) for k in keys if bench[k] is not None]
    data  = [df[col].values for _, df in valid]
    pos   = list(range(1, len(valid)+1))
    vp    = ax.violinplot(data, positions=pos, showmedians=True)
    for body, (k, _) in zip(vp["bodies"], valid):
        body.set_facecolor(SCHEMES[k]["color"])
        body.set_alpha(0.65)
    for part in ("cmedians", "cmaxes", "cmins", "cbars"):
        vp[part].set_color("#CCCCCC")
        vp[part].set_linewidth(1.2)
    ax.set_xticks(pos)
    ax.set_xticklabels([SCHEMES[k]["label"] for k, _ in valid],
                       rotation=12, ha="right", fontsize=8)
    style_ax(ax,
             f"[Sweep 3] {op} Time Violin Distribution",
             "Scheme", f"{op} Time (ms)")
plt.tight_layout()
save("bench_violin")

# 22. Scatter: sign_ms vs verify_ms (bench iters as point cloud)
fig, ax = plt.subplots(figsize=(8, 6))
for k, m in SCHEMES.items():
    df = bench[k]
    if df is None:
        continue
    ax.scatter(df["sign_ms"], df["verify_ms"],
               color=m["color"], alpha=0.55, s=30,
               marker=m["marker"], label=m["label"])
style_ax(ax,
         "[Sweep 3] Sign Time vs Verify Time  (scatter, all iterations)",
         "Sign Time (ms)", "Verify Time (ms)")
ax.legend(fontsize=8)
save("bench_scatter_sign_vs_verify")


# ══════════════════════════════════════════════════════════════════════════
#  CROSS-SWEEP SUMMARY
# ══════════════════════════════════════════════════════════════════════════
print("\n── Summary Plots ───────────────────────────────────────────────────")

# 23. Mean performance bar chart (ring sweep)
fig, axes = plt.subplots(1, 3, figsize=(15, 5))
metrics   = [("sign_ms","Sign Time (ms)"), ("verify_ms","Verify Time (ms)"), ("sig_bytes","Sig Size (bytes)")]
for ax, (col, ylabel) in zip(axes, metrics):
    vals  = []
    clrs_ = []
    lbls  = []
    for k, m in SCHEMES.items():
        df = ring[k]
        if df is None or col not in df.columns:
            continue
        vals.append(df[col].mean())
        clrs_.append(m["color"])
        lbls.append(m["label"])
    bars = ax.bar(range(len(vals)), vals, color=clrs_, alpha=0.85)
    for b, v in zip(bars, vals):
        ax.text(b.get_x() + b.get_width()/2, b.get_height()*1.03,
                f"{v:.1f}", ha="center", va="bottom", fontsize=8)
    ax.set_xticks(range(len(lbls)))
    ax.set_xticklabels(lbls, rotation=12, ha="right", fontsize=8)
    ax.set_yscale("log")
    ax.set_ylabel(ylabel + " (log)")
    ax.set_title(f"[Summary] Mean {ylabel}\n(across all ring sizes)",
                 fontweight="bold", color="#FFFFFF")
    ax.grid(axis="y", alpha=0.4)
    ax.spines[:].set_color("#3A3D4D")
plt.tight_layout()
save("summary_mean_bar")

# 24. Speed-up of FS-CLRS over others (ring sweep — sign time)
fig, ax = plt.subplots(figsize=(8, 5))
df_base = ring["clrs"]
if df_base is not None:
    for k, m in SCHEMES.items():
        if k == "clrs":
            continue
        df = ring[k]
        if df is None:
            continue
        speedup = df["sign_ms"].values / df_base["sign_ms"].values
        ax.plot(df_base["ring_size"], speedup,
                color=m["color"], marker=m["marker"], linestyle=m["ls"],
                linewidth=1.8, markersize=5,
                label=f"{m['label']}  ÷  FS-CLRS Dynamic")
    ax.axhline(1.0, color="#888", linewidth=0.8, linestyle=":")
    style_ax(ax,
             "[Summary] Speed-up of FS-CLRS Dynamic over Other Schemes (Sign)",
             "Ring Size (n)", "Speed-up Factor (×)", log_x=True, log_y=True)
    ax.legend(fontsize=8)
save("summary_speedup_ring_sign")

# 25. Radar chart — multi-dimension summary
cats   = ["Sign\n(n=16)", "Verify\n(n=16)", "Sig Size\n(n=16)",
          "Epoch\nStability", "Bench\nJitter"]
N_cat  = len(cats)
angles = np.linspace(0, 2*np.pi, N_cat, endpoint=False).tolist()
angles += angles[:1]

def get_val(df, col, n=None):
    if df is None:
        return np.nan
    if n is not None:
        row = df[df.apply(lambda r: any(r.astype(str).str.strip() == str(n)), axis=1)]
        return float(row[col].values[0]) if len(row) else np.nan
    return float(df[col].mean())

def radar_score(k):
    r16_s  = get_val(ring[k],  "sign_ms",   16)
    r16_v  = get_val(ring[k],  "verify_ms", 16)
    r16_b  = get_val(ring[k],  "sig_bytes", 16)
    ep_cv  = (epoch[k]["sign_ms"].std() / epoch[k]["sign_ms"].mean()
              if epoch[k] is not None else np.nan)
    bj     = (bench[k]["sign_ms"].std()
              if bench[k] is not None else np.nan)
    # Collect all values to normalise together later
    return [r16_s, r16_v, r16_b, ep_cv, bj]

raw_scores = {k: radar_score(k) for k in SCHEMES}
# Normalise: 0=worst, 1=best (inverted — lower raw = higher score)
for dim in range(N_cat):
    vals = [raw_scores[k][dim] for k in SCHEMES if not np.isnan(raw_scores[k][dim])]
    if not vals:
        continue
    lo, hi = min(vals), max(vals)
    rng = hi - lo if hi != lo else 1
    for k in SCHEMES:
        v = raw_scores[k][dim]
        raw_scores[k][dim] = 1 - (v - lo) / rng if not np.isnan(v) else 0

fig, ax = plt.subplots(figsize=(7, 7), subplot_kw={"polar": True})
fig.patch.set_facecolor("#0F1117")
ax.set_facecolor("#1A1D27")
ax.set_theta_offset(np.pi / 2)
ax.set_theta_direction(-1)
ax.set_thetagrids(np.degrees(angles[:-1]), cats, fontsize=9)
ax.set_ylim(0, 1)
ax.yaxis.set_visible(False)
ax.grid(color="#3A3D4D", linewidth=0.8)
for k, m in SCHEMES.items():
    vals = raw_scores[k] + [raw_scores[k][0]]
    ax.plot(angles, vals, color=m["color"], linewidth=2,
            linestyle=m["ls"], label=m["label"])
    ax.fill(angles, vals, color=m["color"], alpha=0.12)
ax.set_title("[Summary] Radar — Relative Performance  (higher = better)",
             y=1.08, fontweight="bold", color="#FFFFFF", fontsize=12)
ax.legend(loc="lower left", bbox_to_anchor=(-0.30, -0.18), fontsize=8)
save("summary_radar")

# 26. Dashboard — 2×3 grid of key plots
fig, axes = plt.subplots(2, 3, figsize=(18, 10))
fig.suptitle("Ring Signature Schemes — Performance Dashboard",
             fontsize=16, fontweight="bold", color="#FFFFFF", y=1.01)

panels = [
    ("ring",  "ring_size",  "sign_ms",   True,  True,
     "[1] Sign Time vs Ring Size",      "Ring Size (n)", "Sign (ms)"),
    ("ring",  "ring_size",  "verify_ms", True,  True,
     "[2] Verify Time vs Ring Size",    "Ring Size (n)", "Verify (ms)"),
    ("ring",  "ring_size",  "sig_bytes", True,  True,
     "[3] Signature Size vs Ring Size", "Ring Size (n)", "Bytes"),
    ("epoch", None,         "sign_ms",   False, False,
     "[4] Sign Time vs Epoch",          "Epoch",         "Sign (ms)"),
    ("epoch", None,         "verify_ms", False, False,
     "[5] Verify Time vs Epoch",        "Epoch",         "Verify (ms)"),
    ("bench", None,         "sign_ms",   False, False,
     "[6] Sign Time vs Bench Iters",    "Iterations",    "Sign (ms)"),
]

for ax, (sweep, xcol, ycol, lx, ly, title, xlabel, ylabel) in zip(
        axes.flat, panels):
    src = {"ring": ring, "epoch": epoch, "bench": bench}[sweep]
    x_fn = (lambda df: df[xcol]) if xcol else \
           (lambda df: df[epoch_col(df)] if sweep=="epoch"
            else df[bench_iter_col(df)])
    for k, m in SCHEMES.items():
        df = src[k]
        if df is None:
            continue
        ax.plot(x_fn(df), df[ycol],
                color=m["color"], marker=m["marker"], linestyle=m["ls"],
                linewidth=1.5, markersize=4, label=m["label"])
    style_ax(ax, title, xlabel, ylabel, lx, ly)
    ax.legend(fontsize=7)

plt.tight_layout()
save("dashboard_all_sweeps")


# ══════════════════════════════════════════════════════════════════════════
#  DONE
# ══════════════════════════════════════════════════════════════════════════
print(f"\n{'─'*60}")
print(f"  Total plots saved : {len(saved)}")
print(f"  Output folder     : {OUT}")
print(f"{'─'*60}")
for f in saved:
    print(f"    {f}")