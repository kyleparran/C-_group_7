#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include "Order.hpp"
#include "Timer.hpp"
#include "OrderBook.hpp"
#include "MatchingEngine.hpp"
#include "OrderManager.hpp"
#include "MarketData.hpp"

#define TICK_COUNT 10000
#define PriceType_ double
#define OrderIdType_ int
#define orderDTypes PriceType_, OrderIdType_


// todo: Logs arent outputting for anything but baseline

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



void runBaseline() {
    std::vector<long long> latencies;
    auto orderBook = std::make_shared<OrderBook<PriceType_,OrderIdType_>>(TICK_COUNT);
    MatchingEngine<orderDTypes> matchEngine(orderBook);
    OrderManager<orderDTypes> orderManager(orderBook);

    for (int i = 0; i < TICK_COUNT; ++i) {
        Timer timer; timer.start();

        // Simulate incoming tick data
        MarketData tick = simMarketData();

        // Create an order
        bool is_buy = (i % 2 == 0);
        PriceType_ price = is_buy ? tick.bid_price : tick.ask_price;
        auto order = orderManager.createOrder(i, tick.symbol, price, 100, is_buy);

        // Match orders.
        matchEngine.matchOrders(orderManager);

        latencies.push_back(timer.stop());
    }
    matchEngine.flushLogger();
    analyzeLatencies(latencies);
}

void runRawPointers() {
    std::vector<long long> latencies;
    auto orderBook = std::make_shared<OrderBookNewPtrs<PriceType_,OrderIdType_>>(TICK_COUNT);
    MatchingEngine<orderDTypes> matchEngine(orderBook);
    OrderManager<orderDTypes> orderManager(orderBook);

    for (int i = 0; i < TICK_COUNT; ++i) {
        Timer timer; timer.start();

        // Simulate incoming tick data
        MarketData tick = simMarketData();

        // Create an order
        bool is_buy = (i % 2 == 0);
        PriceType_ price = is_buy ? tick.bid_price : tick.ask_price;
        auto order = orderManager.createOrder(i, tick.symbol, price, 100, is_buy);

        // Match orders.
        matchEngine.matchOrders(orderManager);

        latencies.push_back(timer.stop());
    }
    matchEngine.flushLogger();
    analyzeLatencies(latencies);
}



void runNewDelete() {
    std::vector<long long> latencies;
    auto orderBook = std::make_shared<OrderBookFlat<PriceType_,OrderIdType_>>(TICK_COUNT);
    MatchingEngine<orderDTypes> matchEngine(orderBook);
    OrderManager<orderDTypes> orderManager(orderBook);

    for (int i = 0; i < TICK_COUNT; ++i) {
        Timer timer; timer.start();

        // Simulate incoming tick data
        MarketData tick = simMarketData();

        // Create an order
        bool is_buy = (i % 2 == 0);
        PriceType_ price = is_buy ? tick.bid_price : tick.ask_price;
        auto order = orderManager.createOrder(i, tick.symbol, price, 100, is_buy);

        // Match orders.
        matchEngine.matchOrders(orderManager);

        latencies.push_back(timer.stop());
    }
    matchEngine.flushLogger();
    analyzeLatencies(latencies);
}


void runFlatArray() {
    std::vector<long long> latencies;
    auto orderBook = std::make_shared<OrderBookNewDelete<PriceType_,OrderIdType_>>(TICK_COUNT);
    MatchingEngine<orderDTypes> matchEngine(orderBook);
    OrderManager<orderDTypes> orderManager(orderBook);

    for (int i = 0; i < TICK_COUNT; ++i) {
        Timer timer; timer.start();

        // Simulate incoming tick data
        MarketData tick = simMarketData();

        // Create an order
        bool is_buy = (i % 2 == 0);
        PriceType_ price = is_buy ? tick.bid_price : tick.ask_price;
        auto order = orderManager.createOrder(i, tick.symbol, price, 100, is_buy);

        // Match orders.
        matchEngine.matchOrders(orderManager);

        latencies.push_back(timer.stop());
    }
    matchEngine.flushLogger();
    analyzeLatencies(latencies);
}


int main() {
    // uncomment as needed. Should run one at a time

    // Baseline: smart pointers, MemoryPool, alignment, multimap
    runBaseline();

    // raw pointers instead of unique_ptrs
    runRawPointers();

    // Without memory aligment
    // Probably best to just remove it manually from MarketData.hpp

    
    // raw new/delete instead of memory pool
    runNewDelete();

    // Flat array instead of multimap
    runFlatArray();

    return 0;
}
