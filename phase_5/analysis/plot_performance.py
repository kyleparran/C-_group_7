import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv('../data/performance_results.csv')
plt.plot(df['orders'], df['plain'], marker='o', label='Plain')
plt.plot(df['orders'], df['optimized'], marker='s', label='Optimized')
plt.plot(df['orders'], df['pool'], marker='^', label='Pooled')
plt.xlabel('Number of Orders')
plt.ylabel('Execution Time (seconds)')
plt.title('HFT Order Book Performance')
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.savefig('../analysis/performance_chart.png', dpi=300, bbox_inches='tight')
plt.close()

lat = df.copy()
lat['plain']     = lat['plain']     / lat['orders'] * 1e6
lat['optimized'] = lat['optimized'] / lat['orders'] * 1e6
lat['pool']      = lat['pool']      / lat['orders'] * 1e6
plt.plot(lat['orders'], lat['plain'], marker='o', label='Plain')
plt.plot(lat['orders'], lat['optimized'], marker='s', label='Optimized')
plt.plot(lat['orders'], lat['pool'], marker='^', label='Pooled')
plt.xlabel('Number of Orders')
plt.ylabel('Latency per Order (µs)')
plt.title('Per-Order Latency')
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.savefig('../analysis/per_order_latency.png', dpi=300, bbox_inches='tight')
plt.close()

speed = df.copy()
speed['speed_opt']  = speed['plain'] / speed['optimized']
speed['speed_pool'] = speed['plain'] / speed['pool']
print(speed[['orders','speed_opt','speed_pool']].to_string(index=False))

