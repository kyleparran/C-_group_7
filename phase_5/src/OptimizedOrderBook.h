#pragma once
#include <vector>
#include <atomic>
#include "OrderBook.h"

class OptimizedOrderBook {
private:
    std::vector<Order> orderPool;
public:
    std::atomic<int> orderCount{0};
    OptimizedOrderBook(size_t size);
    void addOrder(const std::string& id, double price, int quantity, bool isBuy);
    void processOrdersUnrolled(std::vector<Order>& orders);
};
