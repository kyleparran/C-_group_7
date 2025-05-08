#include "OrderBook.h"

void OrderBook::addOrder(const std::string& id, double price, int qty, bool isBuy) {
    Order o{id, price, qty, isBuy};
    orderLevels[price][id] = o;
    orderLookup[id] = o;
}

void OrderBook::modifyOrder(const std::string& id, double newPrice, int newQty) {
    auto it = orderLookup.find(id);
    if (it == orderLookup.end()) return;
    Order old = it->second;
    orderLevels[old.price].erase(id);
    if (orderLevels[old.price].empty()) orderLevels.erase(old.price);
    addOrder(id, newPrice, newQty, old.isBuy);
}

void OrderBook::deleteOrder(const std::string& id) {
    auto it = orderLookup.find(id);
    if (it == orderLookup.end()) return;
    Order o = it->second;
    orderLevels[o.price].erase(id);
    if (orderLevels[o.price].empty()) orderLevels.erase(o.price);
    orderLookup.erase(it);
}

std::size_t OrderBook::totalOrders() const {
    std::size_t n = 0;
    for (auto& lvl : orderLevels) n += lvl.second.size();
    return n;
}
