#pragma once
#include <string>
#include <chrono>
#include <random>
#include <vector>

using namespace std::chrono;

struct alignas(64) MarketData {
    std::string symbol;
    double bid_price;
    double ask_price;
    high_resolution_clock::time_point timestamp;

    MarketData(std::string s, double b, double a, high_resolution_clock::time_point ts)
        : symbol(std::move(s)), bid_price(b), ask_price(a), timestamp(ts) {}
};


inline MarketData simMarketData() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<> priceDist(100.0, 200.0);
    std::uniform_real_distribution<> spreadDist(0.1, 1.0);
    
    auto round = [](double value) {
        return std::round(value * 10.0) / 10.0;
    };

    MarketData data(
        "AAPL",
        round(priceDist(gen) - spreadDist(gen)),
        round(priceDist(gen) + spreadDist(gen)),
        std::chrono::high_resolution_clock::now()
    );
    return data;
}