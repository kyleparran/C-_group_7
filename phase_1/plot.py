import csv
import matplotlib.pyplot as plt

sizes = []
unopt_means = []
unopt_stds = []
opt_means = []
opt_stds = []

with open("benchmark_results.csv", "r") as f:
    reader = csv.DictReader(f)
    for row in reader:
        sizes.append(int(row["Size"]))
        unopt_means.append(float(row["UnoptimizedMean"]))
        unopt_stds.append(float(row["UnoptimizedStddev"]))
        opt_means.append(float(row["OptimizedMean"]))
        opt_stds.append(float(row["OptimizedStddev"]))

plt.errorbar(sizes, unopt_means, yerr=unopt_stds, capsize=3, label="Unoptimized")
plt.errorbar(sizes, opt_means, yerr=opt_stds, capsize=3, label="Optimized")
plt.xlabel("Matrix Size (N x N)")
plt.ylabel("Time (microseconds)")
plt.legend()
plt.title("Matrix-Vector Multiplication Benchmark")
plt.show()
