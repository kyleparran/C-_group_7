## 
Group 7
Scott Turro Kyle Parran

## Overview

The HFT prototype includes the following modular components:

- **Market Data Simulator:** Generates realistic simulated market ticks with bid/ask prices.
- **Order Book:** Efficiently stores buy and sell orders, exploring various memory allocation strategies.
- **Order Management System :** Manages order states including creation, cancellation, partial fills, and complete fills.
- **Matching Engine:** Matches buy and sell orders based on price priority to execute trades rapidly.
- **Trade Logger:** Logs matched trades into `output.log` in batches.

## Project Structure

```
trading_system/
├── include/
│   ├── MarketData.hpp
│   ├── MatchingEngine.hpp
│   ├── Order.hpp
│   ├── OrderBook.hpp
│   ├── OrderManager.hpp
│   ├── Timer.hpp
│   └── TradeLogger.hpp
├── src/
│   └── TradeLogger.cpp
├── main.cpp
├── CMakeLists.txt
└── README.md
```

- `include/`: Contains header files defining interfaces and data structures.
- `src/`: Implementation files , currently contains `TradeLogger.cpp`.
- `main.cpp`: Entry point and scenario management.

## Build Instructions

```bash
mkdir build
cd build
cmake ..
make
```

After a successful build, the executable will typically be found at:

```
build/trading_system/TradingSystem
```

To execute the trading system, run:

```bash
./trading_system/TradingSystem
```

Trades are logged to `output.log`, and latency statistics are printed to your terminal.

## Experimentation and Configurations

Several experimental configurations are provided in `main.cpp` to benchmark different memory and container strategies:

- **Baseline:** Uses smart pointers, custom memory allocator, aligned memory (`alignas(64)`), and `std::multimap`.
- **Raw Pointers:** Tests system performance with raw pointers in place of smart pointers.
- **New/Delete Allocator:** Uses standard `new/delete` instead of a custom memory pool.
- **Flat Array Order Book:** Uses contiguous arrays (`std::vector`) for order storage to test cache locality performance.

Select scenarios in `main.cpp` by commenting or uncommenting as follows:

```cpp
int main() {
    runBaseline();        // Baseline scenario
    // runRawPointers();  // Uncomment to test raw pointers
    // runNewDelete();    // Uncomment for standard allocation
    // runFlatArray();    // Uncomment to test flat-array storage
    return 0;
}
```

## Measuring Performance

The key metric for performance is **tick-to-trade latency**—the time between receiving market data and executing a trade. Metrics provided include:

- Minimum latency
- Maximum latency
- Average latency
- Standard deviation
- 99th-percentile latency (P99)

You can vary the experiment load by modifying the `TICK_COUNT` macro in `main.cpp`.

```cpp
#define TICK_COUNT 10000  // Adjust this value for scalability tests
```

## Log File Details

Executed trades are recorded into `output.log` in clear, readable format, indicating matched order IDs:

```
Trade: 102 <--> 205
Trade: 109 <--> 306
...
```

## Performance Analysis
The latency results clearly demonstrate how different memory management strategies affect performance in our trading system. In the baseline tests, the mean latency varied notably with workload size, ranging from approximately 11 microseconds at 1,000 ticks, increasing slightly to about 13.5 microseconds at 10,000 ticks, but dropping significantly to around 3.2 microseconds under heavier load (100,000 ticks), suggesting greater efficiency at scale. Remarkably, the flat-array approach delivered the best overall latency, averaging roughly 0.79 microseconds—a substantial improvement highlighting the benefit of contiguous memory for cache performance. Conversely, the raw-pointer implementation resulted in the highest mean latency at approximately 18.5 microseconds, reflecting inefficiencies from manual memory handling and allocation overhead. The standard new/delete scenario produced moderate performance, averaging about 13.8 microseconds, slightly better than raw pointers but still behind the optimized approaches. Across scenarios, maximum latencies occasionally spiked significantly, indicating possible memory allocation delays or cache misses. Overall, these results emphasize the considerable performance advantage provided by optimized, cache-friendly data structures like flat arrays.

## Architecture Diagram
+---------------------+ | Market Data Feed | | (MarketData.hpp) | +----------+----------+ | v +---------------------+ | Order Management | | (OrderManager.hpp) | +----------+----------+ | v +---------------------+ +---------------------+ | Order Book | <-----> | Matching Engine | | (OrderBook.hpp) | | (MatchingEngine.hpp)| +----------+----------+ +----------+----------+ | | +---------------+---------------+ | v +---------------------+ | Trade Logger | | (TradeLogger.hpp) | +---------------------+ | v +---------------+ | output.log | +---------------+

## Overall Observations
During performance testing, we observed several interesting differences across the various setups. Using raw pointers instead of smart pointers clearly led to higher and less predictable latency, showing the additional overhead and complexity from manually managing memory. When we tested memory alignment by using the alignas(64) directive, latency improved noticeably and was more stable, likely because the CPU cache was utilized more effectively. Removing this alignment resulted in frequent latency spikes and increased variability due to less efficient cache usage. Our tests also showed that using a custom memory pool allocator improved allocation speed, significantly reducing latency compared to relying on standard new/delete operations. Additionally, switching from multimap-based order books to flat-array structures dramatically reduced latency, emphasizing the performance advantage gained from contiguous memory and better cache behavior. Lastly, scaling the workload to larger tick volumes (from 1,000 to 100,000) demonstrated the system's ability to handle heavy loads effectively, as average latency improved under higher workloads, although occasional spikes persisted due to factors like cache misses and allocation overhead.
