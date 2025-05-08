#include "PooledOrderBook.h"

PooledOrderBook::PooledOrderBook(std::size_t cap) : pool(cap) {}

void PooledOrderBook::addOrder(const std::string& id, double price, int qty, bool isBuy) {
    Order* o = pool.create(id, price, qty, isBuy);
    lvl[price][id] = o;
    look[id] = o;
}
