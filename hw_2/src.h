#ifndef SRC_H
#define SRC_H

#include <windows.h>
#include <iostream>
#include <vector>
#include <random>
#include <unordered_map>
#include <deque>
#include <fstream>
#include <cmath>
#include <cstdint>

static inline uint64_t getCurrentTimeNs() {
    static LARGE_INTEGER freq = { 0 };
    LARGE_INTEGER counter;
    if (!freq.QuadPart) QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&counter);
    return (uint64_t)((1000000000ULL * counter.QuadPart) / freq.QuadPart);
}

struct alignas(64) MarketData {
    int instrument_id;
    double price;
    uint64_t timestamp;
};

class MarketDataFeed {
public:
    MarketDataFeed(std::vector<MarketData>& ref);
    void generateData(int num_ticks);
private:
    std::vector<MarketData>& data;
};

struct alignas(64) Order {
    int instrument_id;
    double price;
    bool is_buy;
    uint64_t timestamp;
    bool from_signal1;
    bool from_signal2;
    bool from_signal3;
    bool from_signal4;
};

struct PriceStats {
    std::deque<double> window;
    double sum = 0.0;
    double sum_of_squares = 0.0;
};

class TradeEngine {
public:
    TradeEngine(const std::vector<MarketData>& feed);
    void process();
    void reportStats();
    void exportOrderHistory(const std::string& filename);
private:
    const std::vector<MarketData>& market_data;
    std::vector<Order> orders;
    std::vector<uint64_t> latencies;
    std::unordered_map<int, PriceStats> price_history;
    long long s1, s2, s3, s4;
    void updateHistory(const MarketData& tick);
    double getAvg(int id);
    double getStdDev(int id);
    bool signal1(const MarketData& tick);
    bool signal2(const MarketData& tick);
    bool signal3(const MarketData& tick);
    bool signal4(const MarketData& tick);
};

#endif
