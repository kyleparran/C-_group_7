#pragma once
#include <string>
#include <memory>

template <typename PriceType, typename OrderIdType>
struct Order {
    OrderIdType id;
    std::string symbol;
    PriceType price;
    int quantity;
    bool is_buy;

    Order(OrderIdType id, std::string sym, PriceType pr, int qty, bool buy)
        : id(id), symbol(std::move(sym)), price(pr), quantity(qty), is_buy(buy) {}

    bool operator<(const Order& other) const {
        return price < other.price;
    }
    bool operator>(const Order& other) const {
        return price > other.price;
    }
    bool operator<=(const Order& other) const {
        return price <= other.price;
    }
    bool operator>=(const Order& other) const {
        return price >= other.price;
    }
    bool operator==(const Order& other) const {
        return id == other.id;
    }
    bool operator!=(const Order& other) const {
        return id != other.id;
    }
};

