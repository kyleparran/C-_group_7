#pragma once
#include <map>
#include <unordered_map>
#include <string>
#include <cstddef>

struct Order {
    std::string id;
    double price;
    int quantity;
    bool isBuy;
};

class OrderBook {
    std::map<double, std::unordered_map<std::string, Order>> orderLevels;
public:
    std::unordered_map<std::string, Order> orderLookup;
    void addOrder(const std::string& id, double price, int qty, bool isBuy);
    void modifyOrder(const std::string& id, double newPrice, int newQty);
    void deleteOrder(const std::string& id);
    std::size_t totalOrders() const;
};

