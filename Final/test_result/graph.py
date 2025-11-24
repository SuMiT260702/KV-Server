#!/usr/bin/env python3
"""
plot_3cases_twoplots.py

Reads exactly 3 CSV files (each file = one case) and draws:
  - Plot A: thread_count vs throughput (three cases overlaid)
  - Plot B: thread_count vs latency    (three cases overlaid)

Also saves:
  - throughput_plot.png
  - latency_plot.png
  - And optionally ONE combined PNG using --out

CSV must contain:
  thread_count, throughput (or thought_put), latency

Usage:
  python plot_3cases_twoplots.py case1.csv case2.csv case3.csv
  python plot_3cases_twoplots.py case1.csv case2.csv case3.csv --out combined.png
"""

import sys
import os
import argparse
from typing import Tuple, Dict
import pandas as pd
import matplotlib.pyplot as plt

def detect_columns(df: pd.DataFrame) -> Tuple[str, str, str]:
    cols = {c.lower(): c for c in df.columns}

    # thread_count
    if "thread_count" in cols:
        tc = cols["thread_count"]
    elif "threads" in cols:
        tc = cols["threads"]
    else:
        raise ValueError("Missing 'thread_count' column.")

    # throughput (accept typo)
    if "throughput" in cols:
        tp = cols["throughput"]
    elif "thought_put" in cols:
        tp = cols["thought_put"]
    else:
        raise ValueError("Missing 'throughput' or 'thought_put' column.")

    # latency
    if "latency" in cols:
        lt = cols["latency"]
    else:
        raise ValueError("Missing 'latency' column.")

    return tc, tp, lt

def load_case(path: str, case_name: str = None) -> pd.DataFrame:
    df = pd.read_csv(path)
    tc, tp, lt = detect_columns(df)

    df = df[[tc, tp, lt]].copy()
    df = df.rename(columns={tc: "thread_count", tp: "throughput", lt: "latency"})

    df["thread_count"] = pd.to_numeric(df["thread_count"], errors="coerce")
    df["throughput"] = pd.to_numeric(df["throughput"], errors="coerce")
    df["latency"] = pd.to_numeric(df["latency"], errors="coerce")

    df = df.dropna().sort_values("thread_count")

    if case_name is None:
        case_name = os.path.splitext(os.path.basename(path))[0]

    df["case"] = case_name
    return df

def plot_three_cases(df_map: Dict[str, pd.DataFrame], out_combined=None):
    if len(df_map) != 3:
        raise ValueError("Exactly 3 cases required.")

    markers = ["o", "s", "D"]
    linestyles = ["-", "--", "-."]

    # -------------------
    # Plot 1: Throughput
    # -------------------
    plt.figure(figsize=(7, 5))
    for (name, df), m, ls in zip(df_map.items(), markers, linestyles):
        plt.plot(df["thread_count"], df["throughput"], marker=m, linestyle=ls, label=name)

    plt.title("Threads vs Throughput")
    plt.xlabel("thread_count")
    plt.ylabel("throughput")
    plt.grid(True, linestyle=":")
    plt.legend()
    plt.savefig("throughput_plot.png", dpi=180)
    print("Saved: throughput_plot.png")
    plt.close()

    # -------------------
    # Plot 2: Latency
    # -------------------
    plt.figure(figsize=(7, 5))
    for (name, df), m, ls in zip(df_map.items(), markers, linestyles):
        plt.plot(df["thread_count"], df["latency"], marker=m, linestyle=ls, label=name)

    plt.title("Threads vs Latency")
    plt.xlabel("thread_count")
    plt.ylabel("latency (ms)")
    plt.grid(True, linestyle=":")
    plt.legend()
    plt.savefig("latency_plot.png", dpi=180)
    print("Saved: latency_plot.png")
    plt.close()

    # -------------------
    # Combined plot (optional)
    # -------------------
    if out_combined:
        fig, ax = plt.subplots(1, 2, figsize=(14, 5))

        # Throughput
        for (name, df), m, ls in zip(df_map.items(), markers, linestyles):
            ax[0].plot(df["thread_count"], df["throughput"], marker=m, linestyle=ls, label=name)
        ax[0].set_title("Threads vs Throughput")
        ax[0].set_xlabel("thread_count")
        ax[0].set_ylabel("throughput")
        ax[0].grid(True, linestyle=":")
        ax[0].legend()

        # Latency
        for (name, df), m, ls in zip(df_map.items(), markers, linestyles):
            ax[1].plot(df["thread_count"], df["latency"], marker=m, linestyle=ls, label=name)
        ax[1].set_title("Threads vs Latency")
        ax[1].set_xlabel("thread_count")
        ax[1].set_ylabel("latency (ms)")
        ax[1].grid(True, linestyle=":")
        ax[1].legend()

        plt.tight_layout()
        plt.savefig(out_combined, dpi=180)
        print(f"Saved combined figure: {out_combined}")
        plt.close()

def main():
    parser = argparse.ArgumentParser(description="Plot throughput and latency from 3 CSV files.")
    parser.add_argument("files", nargs=3, help="Three CSV files (each one case).")
    parser.add_argument("--out", help="Optional combined output PNG.")
    args = parser.parse_args()

    df_map = {}
    for f in args.files:
        case_name = os.path.splitext(os.path.basename(f))[0]
        df_map[case_name] = load_case(f, case_name)

    plot_three_cases(df_map, out_combined=args.out)

if __name__ == "__main__":
    main()