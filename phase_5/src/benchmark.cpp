#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <random>
#include <cassert>
#include "OrderBook.h"
#include "OptimizedOrderBook.h"
#include "PooledOrderBook.h"
#include <fmt/base.h>
#include <fmt/os.h>

void testAddOrder()
{
    OrderBook book;
    book.addOrder("ORD001", 50.10, 100, true);
    assert(book.orderLookup.count("ORD001") == 1);
}

void stressTest(OrderBook &book, int numOrders)
{
    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<double> p(50.0, 100.0);
    std::uniform_int_distribution<int> q(1, 500);
    for (int i = 0; i < numOrders; ++i)
        book.addOrder("ORD" + std::to_string(i), p(rng), q(rng), true);
    assert(book.orderLookup.size() == static_cast<std::size_t>(numOrders));
}

void coreUnitTests()
{
    OrderBook b;
    b.addOrder("A", 10.0, 1, true);
    b.modifyOrder("A", 11.0, 2);
    assert(b.orderLookup["A"].price == 11.0);
    b.deleteOrder("A");
    assert(b.orderLookup.empty());
    testAddOrder();
}

void randomizedConsistencyTest(int nOps)
{
    OrderBook b;
    std::mt19937 g(42);
    std::uniform_real_distribution<double> p(50.0, 100.0);
    std::uniform_int_distribution<int> q(1, 500);
    std::uniform_int_distribution<int> op(0, 2);
    for (int i = 0; i < nOps; ++i)
    {
        int k = op(g);
        std::string id = "R" + std::to_string(i);
        if (k == 0)
            b.addOrder(id, p(g), q(g), true);
        else if (k == 1)
            b.modifyOrder(id, p(g), q(g));
        else
            b.deleteOrder(id);
    }
    assert(b.totalOrders() == b.orderLookup.size());
}

std::vector<Order> gen(int n, std::optional<unsigned int> seed = std::nullopt)
{
    std::mt19937 g(seed ? *seed : std::random_device{}());
    std::uniform_real_distribution<double> p(50.0, 100.0);
    std::uniform_int_distribution<int> q(1, 500);
    std::vector<Order> v;
    v.reserve(n);
    for (int i = 0; i < n; ++i)
        v.push_back({"ORD" + std::to_string(i), p(g), q(g), true});
    return v;
}

double time_plain(const std::vector<Order> &v)
{
    OrderBook ob;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (auto &o : v)
        ob.addOrder(o.id, o.price, o.quantity, o.isBuy);
    return std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - t0).count();
}

double time_opt(const std::vector<Order> &v)
{
    OptimizedOrderBook ob(v.size());
    auto t0 = std::chrono::high_resolution_clock::now();
    ob.processOrdersUnrolled(const_cast<std::vector<Order> &>(v));
    return std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - t0).count();
}

double time_pool(const std::vector<Order> &v)
{
    PooledOrderBook pb(v.size());
    auto t0 = std::chrono::high_resolution_clock::now();
    for (auto &o : v)
        pb.addOrder(o.id, o.price, o.quantity, o.isBuy);
    return std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - t0).count();
}

inline std::pair<double, double> calc_stats(std::vector<double> vals) {
    if (vals.empty()) {
        return {0.0, 0.0}; // Return 0 for both mean and stddev if the input is empty
    }

    double avg = 0.0;
    for (double t : vals) {
        avg += t;
    }
    avg /= vals.size();

    double variance = 0.0;
    for (double t : vals) {
        double diff = t - avg;
        variance += diff * diff;
    }
    variance /= vals.size();

    double stddev = std::sqrt(variance);
    return {avg, stddev};
}

std::pair<double, double> multi_time_plain(int numOrders, int numRuns, std::optional<int> seed = std::nullopt)
{
    std::vector<double> times(numRuns);
    for (int i = 0; i < numRuns; ++i)
    {
        auto v = gen(numOrders, seed);
        double to = time_plain(v);
        times[i] = to;
    }
    return calc_stats(times);
}

std::pair<double, double> multi_time_opt(int numOrders, int numRuns, std::optional<int> seed = std::nullopt)
{
    std::vector<double> times(numRuns);
    for (int i = 0; i < numRuns; ++i)
    {
        auto v = gen(numOrders, seed);
        double to = time_opt(v);
        times[i] = to;
    }
    return calc_stats(times);
}

std::pair<double, double> multi_time_pool(int numOrders, int numRuns, std::optional<int> seed = std::nullopt)
{
    std::vector<double> times(numRuns);
    for (int i = 0; i < numRuns; ++i)
    {
        auto v = gen(numOrders, seed);
        double to = time_pool(v);
        times[i] = to;
    }
    return calc_stats(times);
}

int main(int argc, char **argv)
{
    std::vector<int> vols{1000, 5000, 10000, 50000, 100000};

    // Run unit tests
    if (argc > 1 && std::string(argv[1]) == "--unit")
    {   
        coreUnitTests();
        fmt::print("Core unit tests passed!\n");

        int numOrders = 1000;
        while (numOrders <= 10000000) {
            OrderBook tmp;
            stressTest(tmp, numOrders);
            fmt::print("Stress test passed with {} orders!\n", numOrders);
            randomizedConsistencyTest(numOrders);
            fmt::print("Randomized consistency test passed with {} orders!\n", numOrders);
            numOrders = numOrders * 10;
        }
        return 0;
    }

    // Run all implementations
    auto csv = fmt::output_file("../data/performance_results.csv");
    csv.print("orders,plain,optimized,pool\n");
    int numRuns = 30;
    int seed = 50;
    for (int n : vols)
    {
        auto v = gen(n, seed);
        auto [tp, tp_std] = multi_time_plain(n, numRuns);
        auto [to, to_std] = multi_time_opt(n, numRuns);
        auto [tm, tm_std] = multi_time_pool(n, numRuns);

        // Print to std
        fmt::print("{} : ", n);
        fmt::print("plain {:.5f}+/-{:.0f}% s, ", tp, tp_std / tp / std::sqrt(numRuns) * 100);
        fmt::print("opt {:.5f}+/-{:.0f}% s, ", to, to_std / to / std::sqrt(numRuns) * 100);
        fmt::print("pool {:.5f}+/-{:.0f}% s\n", tm, tm_std / tm / std::sqrt(numRuns) * 100);

        // Print to csv
        csv.print("{}, {}, {}, {}\n", n, tp, to, tm);
    }
    return 0;
}
