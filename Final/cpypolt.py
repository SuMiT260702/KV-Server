#!/usr/bin/env python3
"""
cpupplot_fixed.py

Usage:
  python3 cpupplot_fixed.py /path/to/folder

Looks for files named like: 1cpu_core2.log
  - leading number = workload (case)
  - trailing number = thread_count

Parses mpstat-style logs robustly (handles timestamp columns, repeated headers
and variant layouts) and plots median CPU metrics per workload vs thread_count.

Outputs:
  - threads_vs_cpu_combined_plot.png   (combined usr+sys+soft)
  - threads_vs_cpu_usage_plot.png      (100 - idle)
  - threads_vs_cpu_both.png            (both subplots)
"""
import re
import sys
import math
from pathlib import Path
from statistics import median
from typing import Dict, List, Tuple, Any

import matplotlib.pyplot as plt

FILENAME_RE = re.compile(r'^(?P<workload>\d+)cpu_core(?P<threads>\d+)\.log$', re.IGNORECASE)

# canonical mpstat order (right-to-left mapping will use trailing tokens)
MPSTAT_FIELDS = ['cpu','usr','nice','sys','iowait','irq','soft','steal','guest','gnice','idle']

def infer_workload_threads(fname: str):
    m = FILENAME_RE.match(fname)
    if not m:
        return None, None
    return m.group('workload'), int(m.group('threads'))

def try_parse_lines_right_align(lines):
    """
    Strategy:
      - For each candidate data line, extract all numeric tokens.
      - If >=4 numeric tokens found (common case), map the trailing numeric tokens
        to the rightmost MPSTAT_FIELDS. Example: if 11 numeric tokens -> map full list.
      - Collect rows (dicts) with whichever MPSTAT_FIELDS are present.
    Returns list of dict rows and a sample parsed row (for diagnostics).
    """
    rows = []
    sample = None
    float_re = re.compile(r'^-?\d+(\.\d+)?$')
    for line in lines:
        parts = re.split(r'\s+', line.strip())
        if not parts:
            continue
        nums = [p for p in parts if float_re.match(p)]
        if len(nums) < 4:
            # not enough numbers to be a data row
            continue
        # we'll take last up to len(MPSTAT_FIELDS) tokens
        take = min(len(nums), len(MPSTAT_FIELDS))
        trailing = nums[-take:]
        # align trailing values to the rightmost fields of MPSTAT_FIELDS
        mapped = {}
        start_idx = len(MPSTAT_FIELDS) - take
        for i, val in enumerate(trailing):
            field = MPSTAT_FIELDS[start_idx + i]
            try:
                mapped[field] = float(val)
            except:
                mapped[field] = math.nan
        rows.append(mapped)
        if sample is None:
            sample = mapped
    return rows, sample

def try_parse_with_header(lines):
    """
    Try to find explicit header and then align data tokens to header by token index.
    This handles headers like:
      05:06:52 AM  CPU    %usr   %nice    %sys ...
    It returns parsed rows if successful (list of dicts).
    """
    header_idx = None
    header_cols = None
    for i, line in enumerate(lines):
        low = line.lower()
        if 'cpu' in low and ('usr' in low or '%usr' in low or 'idle' in low):
            header_cols = re.split(r'\s+', line.strip())
            header_idx = i
            break
    if header_idx is None:
        return [], None
    # normalize header
    def norm(tok):
        t = tok.lower().strip().lstrip('%')
        mapping = {
            'usr':'usr','nice':'nice','sys':'sys','iowait':'iowait','irq':'irq','soft':'soft',
            'steal':'steal','guest':'guest','gnice':'gnice','idle':'idle','cpu':'cpu'
        }
        return mapping.get(t, t)
    header_norm = [norm(h) for h in header_cols]
    rows = []
    sample = None
    float_re = re.compile(r'^-?\d+(\.\d+)?$')
    for line in lines[header_idx+1:]:
        if not line.strip():
            continue
        parts = re.split(r'\s+', line.strip())
        # If parts align with header length, we can map directly
        if len(parts) >= len(header_norm):
            # try various alignments: right-align then left-align
            # first try direct alignment of last N tokens to header (handles timestamps before header)
            trailing = parts[-len(header_norm):]
            if all(float_re.match(p) or not p for p in trailing):
                mapped = {}
                for h, val in zip(header_norm, trailing):
                    if float_re.match(val):
                        mapped[h] = float(val)
                if mapped:
                    rows.append(mapped)
                    if sample is None:
                        sample = mapped
                    continue
        # else, find any window where all tokens numeric for header length
        found = False
        for start in range(0, max(1, len(parts) - len(header_norm) + 1)):
            block = parts[start:start+len(header_norm)]
            if all(float_re.match(p) for p in block):
                mapped = {}
                for h, val in zip(header_norm, block):
                    mapped[h] = float(val)
                rows.append(mapped)
                if sample is None:
                    sample = mapped
                found = True
                break
        if found:
            continue
        # otherwise skip
    return rows, sample

def parse_mpstat_file(path: Path):
    text = path.read_text(encoding='utf-8', errors='ignore').splitlines()
    # First try header-based parsing
    rows, sample = try_parse_with_header(text)
    if rows:
        return rows, sample, 'header'
    # fallback: right-align numeric tokens
    rows, sample = try_parse_lines_right_align(text)
    if rows:
        return rows, sample, 'right-align'
    return [], None, None

def compute_medians_from_rows(rows: List[Dict[str, float]]):
    """
    rows = list of dicts where keys are mpstat fields present.
    Return medians for usr, sys, soft, idle, combined, usage_from_idle.
    """
    def col_median(col):
        vals = [r[col] for r in rows if col in r and r[col] is not None and not math.isnan(r[col])]
        return float(median(vals)) if vals else math.nan

    usr_m = col_median('usr')
    sys_m = col_median('sys')
    soft_m = col_median('soft')
    idle_m = col_median('idle')
    combined = math.nan
    if not math.isnan(usr_m) or not math.isnan(sys_m) or not math.isnan(soft_m):
        u = 0.0 if math.isnan(usr_m) else usr_m
        s = 0.0 if math.isnan(sys_m) else sys_m
        so = 0.0 if math.isnan(soft_m) else soft_m
        combined = u + s + so
    usage_from_idle = 100.0 - idle_m if not math.isnan(idle_m) else math.nan
    return {
        'usr': usr_m, 'sys': sys_m, 'soft': soft_m, 'idle': idle_m,
        'combined': combined, 'usage_from_idle': usage_from_idle
    }

def plot_workloads_series(workload_map: Dict[str, List[Tuple[int, dict]]], out_prefix: str = "threads_vs_cpu"):
    workloads = sorted(workload_map.keys(), key=lambda x: int(x) if x.isdigit() else x)
    series_combined = {}
    series_usage = {}
    for w in workloads:
        pts = sorted(workload_map[w], key=lambda x: x[0])
        threads = [p[0] for p in pts]
        combined = [p[1].get('combined', math.nan) for p in pts]
        usage = [p[1].get('usage_from_idle', math.nan) for p in pts]
        series_combined[w] = (threads, combined)
        series_usage[w] = (threads, usage)

    # Combined plot
    plt.figure(figsize=(8,5))
    for w in workloads:
        t, v = series_combined[w]
        if all(math.isnan(x) for x in v): continue
        plt.plot(t, v, marker='o', linestyle='-', label=f"workload {w}")
    plt.title("Thread count vs median combined CPU (usr+sys+soft)")
    plt.xlabel("thread_count")
    plt.ylabel("%")
    plt.grid(True, linestyle=':')
    plt.legend()
    out1 = f"{out_prefix}_combined_plot.png"
    plt.tight_layout(); plt.savefig(out1, dpi=180); plt.close()
    print(f"Saved: {out1}")

    # Usage plot
    plt.figure(figsize=(8,5))
    for w in workloads:
        t, v = series_usage[w]
        if all(math.isnan(x) for x in v): continue
        plt.plot(t, v, marker='s', linestyle='--', label=f"workload {w}")
    plt.title("Thread count vs median CPU usage (100 - idle)")
    plt.xlabel("thread_count")
    plt.ylabel("%")
    plt.grid(True, linestyle=':')
    plt.legend()
    out2 = f"{out_prefix}_usage_plot.png"
    plt.tight_layout(); plt.savefig(out2, dpi=180); plt.close()
    print(f"Saved: {out2}")

    # Both subplots
    fig, ax = plt.subplots(1,2, figsize=(14,5))
    for w in workloads:
        t, v = series_combined[w]; 
        if not all(math.isnan(x) for x in v):
            ax[0].plot(t, v, marker='o', linestyle='-', label=f"workload {w}")
    ax[0].set_title("combined (usr+sys+soft)"); ax[0].set_xlabel("thread_count"); ax[0].set_ylabel("%"); ax[0].grid(True); ax[0].legend()

    for w in workloads:
        t, v = series_usage[w]
        if not all(math.isnan(x) for x in v):
            ax[1].plot(t, v, marker='s', linestyle='--', label=f"workload {w}")
    ax[1].set_title("usage (100 - idle)"); ax[1].set_xlabel("thread_count"); ax[1].set_ylabel("%"); ax[1].grid(True); ax[1].legend()

    plt.suptitle("Thread count vs median CPU metrics by workload")
    out3 = f"{out_prefix}_both.png"
    plt.tight_layout(rect=[0,0,1,0.95]); plt.savefig(out3, dpi=180); plt.close()
    print(f"Saved: {out3}")

def main(argv):
    import argparse
    parser = argparse.ArgumentParser(description="Parse folder of mpstat logs named like 1cpu_core2.log")
    parser.add_argument("folder", help="Path to folder containing logs")
    parser.add_argument("--out-prefix", default="threads_vs_cpu", help="Output filename prefix")
    args = parser.parse_args(argv[1:])

    folder = Path(args.folder)
    if not folder.exists() or not folder.is_dir():
        print(f"Error: not a folder: {folder}"); sys.exit(1)

    workload_map: Dict[str, List[Tuple[int, dict]]] = {}

    for f in sorted(folder.iterdir()):
        if not f.is_file(): continue
        workload, threads = infer_workload_threads(f.name)
        if workload is None:
            print(f"Skipping (pattern mismatch): {f.name}")
            continue
        rows, sample, method = parse_mpstat_file(f)
        if not rows:
            print(f"Skipping (no data parsed): {f.name}")
            continue
        med = compute_medians_from_rows(rows)
        print(f"Parsed {f.name}: workload={workload}, threads={threads}, method={method}, sample_row={sample}, medians={med}")
        workload_map.setdefault(workload, []).append((threads, med))

    if not workload_map:
        print("No matching files parsed. Exiting.")
        sys.exit(1)

    plot_workloads_series(workload_map, out_prefix=args.out_prefix)

if __name__ == "__main__":
    main(sys.argv)
