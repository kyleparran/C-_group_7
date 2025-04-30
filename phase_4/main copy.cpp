#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <cmath>
#include "TradeLogger.hpp"
#include "OrderBook.hpp"
#include "Timer.hpp"
#include "MarketData.hpp"

using OrderType = Order<double, int>;

void analyzeLatencies(const std::vector<long long>& latencies) {
    if (latencies.empty()) return;

    auto min = *std::min_element(latencies.begin(), latencies.end());
    auto max = *std::max_element(latencies.begin(), latencies.end());
    double mean = std::accumulate(latencies.begin(), latencies.end(), 0.0) / latencies.size();
    double variance = 0.0;
    for (auto l : latencies) variance += (l - mean) * (l - mean);
    double stddev = std::sqrt(variance / latencies.size());
    long long p99 = latencies[static_cast<int>(latencies.size() * 0.99)];

    std::cout << "Tick-to-Trade Latency (nanoseconds):\n";
    std::cout << "Min: " << min << "\nMax: " << max << "\nMean: " << mean
              << "\nStdDev: " << stddev << "\nP99: " << p99 << "\n";
}

int main() {
    AppConfig cfg = loadAppConfig("config.yaml");
    initLogger(cfg.logger);

    std::vector<long long> latencies;
    const int num_ticks = 10000;

    // Init Order book
    OrderBook<PoolAllocator<Order<>>> orderBook;
    orderBook.addOrder(OrderType(1, "AAPL", 150.0, 100, true));
    orderBook.addOrder(OrderType(2, "AAPL", 151.0, 200, false));
    orderBook.addOrder(OrderType(3, "AAPL", 152.0, 150, true));
    // orderBook.printOrders();

    for (int i = 0; i < num_ticks; ++i) {
        Timer timer;
        timer.start();

        // Simulate incoming tick data
        MarketData mkData = simMarketData(i);


        
        // Create a new order if needed
        // Delete a new order if needed

        // Update order book
        // Create new order
        
        // Order Match

        OrderType order(i, "AAPL", 150.0 + (i % 5), 100, i % 2 == 0);
        // simulate match logic here

        latencies.push_back(timer.stop());
    }

    // Analyze latency
    analyzeLatencies(latencies);
}

int main() {
    // --- Baseline

    runBaseline(1000);
    // runSmartPointerExperiment(10000);
    // runSmartPointerExperiment(100000);

    // --- Experiment 1: Smart vs. Raw Pointers ---

    // runSmartPointerExperiment(1000);
    // runSmartPointerExperiment(10000);
    // runSmartPointerExperiment(100000);

    // runRawPointerExperiment(1000);
    // runRawPointerExperiment(10000);
    // runRawPointerExperiment(100000);

    // --- Experiment 2: Memory Pool vs. new/delete ---
    // runMemoryPoolNewExperiment(1000);
    // runMemoryPoolNewExperiment(10000);
    // runMemoryPoolNewExperiment(100000);

    // runMemoryPoolPoolExperiment(1000);
    // runMemoryPoolPoolExperiment(10000);
    // runMemoryPoolPoolExperiment(100000);

    // --- Experiment 3: Container Layout (Multimap vs. Flat Array) ---
    // runContainerMultimapExperiment(1000);
    // runContainerMultimapExperiment(10000);
    // runContainerMultimapExperiment(100000);

    // runContainerFlatArrayExperiment(1000);
    // runContainerFlatArrayExperiment(10000);
    // runContainerFlatArrayExperiment(100000);

    return 0;
}