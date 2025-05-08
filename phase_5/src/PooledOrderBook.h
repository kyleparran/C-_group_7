#pragma once
#include <map>
#include <unordered_map>
#include <string>
#include "OrderBook.h"
#include "MemoryPool.h"

class PooledOrderBook {
    FixedMemoryPool<Order> pool;
    std::map<double, std::unordered_map<std::string, Order*>> lvl;
    std::unordered_map<std::string, Order*> look;
public:
    explicit PooledOrderBook(std::size_t cap);
    void addOrder(const std::string& id, double price, int qty, bool isBuy);
};
