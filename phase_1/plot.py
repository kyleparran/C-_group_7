import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv("benchmark_results.csv")

functions = ["multiply_mv_row_major",
             "multiply_mv_col_major",
             "multiply_mm_naive",
             "multiply_mm_transposed_b"
             ]



num_plots = len(functions)
num_cols = 2
for idx, func_name in enumerate(functions):
    plt.subplot(num_plots // num_cols, num_cols, idx + 1)
    
    mask = df["function"] == func_name
    plt.errorbar(df.loc[mask, "size"], 
                 df.loc[mask, "mean"], 
                 yerr=df.loc[mask, "std"], 
                 capsize=3, 
                 label="Unoptimized")
    mask = df["function"] == func_name + "_opt"
    plt.errorbar(df.loc[mask, "size"], 
                df.loc[mask, "mean"], 
                yerr=df.loc[mask, "std"], 
                capsize=3, 
                label="Optimized")
    plt.title(func_name)
    plt.xlabel("Matrix Size (N x N)")
    plt.ylabel("Time (microseconds)")
    plt.grid()
    plt.legend()
    
plt.tight_layout()
plt.show()
