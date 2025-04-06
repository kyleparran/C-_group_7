import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
df = pd.read_csv("benchmark_results.csv")

functions = [
    "multiply_mv_row_major",
    "multiply_mv_col_major",
    "multiply_mm_naive",
    "multiply_mm_transposed_b"
]

benchmark_name = "Simple Benchmark"
separate_plots = True
save_plots = True

def plot_function(df, func_name):
    for suffix, label in [("", "Unoptimized"), ("_opt", "Optimized")]:
        mask = df["function"] == func_name + suffix
        plt.errorbar(
            df.loc[mask, "size"],
            df.loc[mask, "mean"],
            yerr=df.loc[mask, "std"],
            capsize=3,
            label=label
        )
    plt.title(f"{benchmark_name} ({func_name})")
    plt.xlabel("Matrix Size (N x N)")
    plt.ylabel("Time (microseconds)")
    plt.grid(True)
    plt.legend()

if separate_plots:
    for idx, func_name in enumerate(functions, start=1):
        if func_name not in df["function"].values:
            print(f"Function {func_name} not found in dataframe.")
            continue
        
        plt.figure()
        plot_function(df, func_name)
        if save_plots:
            plt.savefig(f"imgs/{benchmark_name} - {func_name}.png")
else:
    num_plots = len(functions)
    num_cols = 2
    plt.figure(figsize=(10,5))
    for idx, func_name in enumerate(functions, start=1):
        plt.subplot(num_plots // num_cols, num_cols, idx)
        plot_function(df, func_name)
    plt.tight_layout()
    if save_plots:
        plt.savefig(f"imgs/{benchmark_name} - all_functions.png")

plt.show()

rename_map = {
    "multiply_mv_row_major": "MV Row Major",
    "multiply_mv_row_major_opt": "MV Row Major (Opt)",
    "multiply_mv_col_major": "MV Col Major",
    "multiply_mv_col_major_opt": "MV Col Major (Opt)",
    "multiply_mm_naive": "MM Naive",
    "multiply_mm_naive_opt": "MM Naive (Opt)",
    "multiply_mm_transposed_b": "MM Transposed B",
    "multiply_mm_transposed_b_opt": "MM Transposed B (Opt)"
}

df["Function"] = df["function"].replace(rename_map)

plt.rcParams["font.family"] = "Arial"


def summary_table(df):
    pivot = df.pivot_table(index="Function", columns="size", values=["mean","std"])
    pivot = pivot.swaplevel(0,1, axis=1)
    pivot = pivot.sort_index(axis=1, level=0)
    pivot = pivot.round(0).astype(int)
    pivot.columns = [f"Size {sz}\n(µs, {metric.upper()})" for sz, metric in pivot.columns]

    fig, ax = plt.subplots(figsize=(12, 8))
    ax.axis("off")
    ax.axis("tight")

    data_vals = pivot.values
    table = ax.table(
        cellText=data_vals,
        colLabels=pivot.columns,
        rowLabels=pivot.index,
        loc="center",
        cellLoc="center"
    )
    table.scale(1.6, 1.6)
    table.set_fontsize(12)

    for (row, col), cell in table.get_celld().items():
        cell.set_linewidth(1)
        cell.set_edgecolor("#000000")
        if row == 0:
            cell.set_facecolor("#4F81BD")
            cell.set_text_props(color="#FFFFFF", weight="bold")
        elif col == -1 and row > 0:
            cell.set_facecolor("#D9E1F2")
            cell.set_text_props(weight="bold")
        elif row % 2 == 0 and row > 0:
            cell.set_facecolor("#F2F2F2")

    title_obj = plt.title("Summary Table (Microseconds), Matrix Size = Dim X Dim", fontsize=16)
    title_obj.set_y(.1)

    fig.subplots_adjust(top=0.85)

    plt.tight_layout()
    plt.savefig("imgs/summary_table.png", bbox_inches="tight")
    plt.show()

summary_table(df)

plt.style.use("ggplot")
              
df = pd.read_csv("aligned_unaligned_results.csv")

unaligned_df = df[df["function"] == "MV_RowMajor_Unaligned"]
aligned_df   = df[df["function"] == "MV_RowMajor_Aligned"]

plt.figure(figsize=(8,5))

plt.errorbar(
    unaligned_df["size"],
    unaligned_df["mean"],
    yerr=unaligned_df["std"],
    fmt="-o",
    capsize=3,
    label="Unaligned"
)

plt.errorbar(
    aligned_df["size"],
    aligned_df["mean"],
    yerr=aligned_df["std"],
    fmt="--s",
    capsize=3,
    label="Aligned"
)

plt.title("Comparison: Aligned vs. Unaligned (Row-Major MV)")
plt.xlabel("Matrix Size (N x N)")
plt.ylabel("Runtime (µs)")
plt.legend()
plt.grid(True)

plt.tight_layout()
plt.savefig("aligned_vs_unaligned_comparison.png", dpi=300)
plt.show()