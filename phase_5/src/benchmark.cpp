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

void testAddOrder() {
    OrderBook book;
    book.addOrder("ORD001", 50.10, 100, true);
    assert(book.orderLookup.count("ORD001") == 1);
}

void stressTest(OrderBook& book, int numOrders) {
    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<double> p(50.0, 100.0);
    std::uniform_int_distribution<int> q(1, 500);
    for (int i = 0; i < numOrders; ++i)
        book.addOrder("ORD" + std::to_string(i), p(rng), q(rng), true);
    assert(book.orderLookup.size() == static_cast<std::size_t>(numOrders));
}

void coreUnitTests() {
    OrderBook b;
    b.addOrder("A", 10.0, 1, true);
    b.modifyOrder("A", 11.0, 2);
    assert(b.orderLookup["A"].price == 11.0);
    b.deleteOrder("A");
    assert(b.orderLookup.empty());
}

void randomizedConsistencyTest(int nOps) {
    OrderBook b;
    std::mt19937 g(42);
    std::uniform_real_distribution<double> p(50.0, 100.0);
    std::uniform_int_distribution<int> q(1, 500);
    std::uniform_int_distribution<int> op(0, 2);
    for (int i = 0; i < nOps; ++i) {
        int k = op(g);
        std::string id = "R" + std::to_string(i);
        if (k == 0) b.addOrder(id, p(g), q(g), true);
        else if (k == 1) b.modifyOrder(id, p(g), q(g));
        else b.deleteOrder(id);
    }
    assert(b.totalOrders() == b.orderLookup.size());
}

std::vector<Order> gen(int n) {
    std::mt19937 g(std::random_device{}());
    std::uniform_real_distribution<double> p(50.0, 100.0);
    std::uniform_int_distribution<int> q(1, 500);
    std::vector<Order> v;
    v.reserve(n);
    for (int i = 0; i < n; ++i)
        v.push_back({"ORD" + std::to_string(i), p(g), q(g), true});
    return v;
}

double time_plain(const std::vector<Order>& v) {
    OrderBook ob;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (auto& o : v) ob.addOrder(o.id, o.price, o.quantity, o.isBuy);
    return std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - t0).count();
}

double time_opt(const std::vector<Order>& v) {
    OptimizedOrderBook ob(v.size());
    auto t0 = std::chrono::high_resolution_clock::now();
    ob.processOrdersUnrolled(const_cast<std::vector<Order>&>(v));
    return std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - t0).count();
}

double time_pool(const std::vector<Order>& v) {
    PooledOrderBook pb(v.size());
    auto t0 = std::chrono::high_resolution_clock::now();
    for (auto& o : v) pb.addOrder(o.id, o.price, o.quantity, o.isBuy);
    return std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - t0).count();
}

int main(int argc, char** argv) {
    if (argc > 1 && std::string(argv[1]) == "--unit") {
        testAddOrder();
        {
            OrderBook tmp;
            stressTest(tmp, 1000);
        }
        coreUnitTests();
        randomizedConsistencyTest(10000);
        std::cout << "unit tests passed\n";
        return 0;
    }

    std::vector<int> vols{1000, 5000, 10000, 50000, 100000};
    std::ofstream csv("../data/performance_results.csv");
    csv << "orders,plain,optimized,pool\n";
    for (int n : vols) {
        auto v = gen(n);
        double tp = time_plain(v);
        double to = time_opt(v);
        double tm = time_pool(v);
        csv << n << ',' << tp << ',' << to << ',' << tm << '\n';
        std::cout << n << " : plain " << tp << " s, opt " << to << " s, pool " << tm << " s\n";
    }
    return 0;
}
