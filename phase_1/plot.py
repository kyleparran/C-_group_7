import pandas as pd
import matplotlib.pyplot as plt

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
