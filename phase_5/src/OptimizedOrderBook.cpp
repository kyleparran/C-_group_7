#include "OptimizedOrderBook.h"

OptimizedOrderBook::OptimizedOrderBook(size_t size) {
    orderPool.reserve(size);
}

void OptimizedOrderBook::addOrder(const std::string& id, double price, int quantity, bool isBuy) {
    Order order = {id, price, quantity, isBuy};
    orderPool.push_back(order);
    orderCount.fetch_add(1, std::memory_order_relaxed);
}

void OptimizedOrderBook::processOrdersUnrolled(std::vector<Order>& orders) {
    for (size_t i = 0; i < orders.size(); i += 2) {
        orderPool.push_back(orders[i]);
        if (i + 1 < orders.size()) orderPool.push_back(orders[i + 1]);
    }
}
