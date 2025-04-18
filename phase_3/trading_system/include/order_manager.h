#pragma once
#include <map>
#include <memory>
#include <string>
#include "feed_parser.h"

enum class Side { Buy, Sell };
enum class OrderStatus { New, Filled, PartiallyFilled, Cancelled };

struct Order {
    int id = 0;
    Side side = Side::Buy;
    double price = 0.0;
    int quantity = 0;
    int filled = 0;
    OrderStatus status = OrderStatus::New;
};

struct OMEvent {
    std::string msg;
    bool completed = false;
    int order_id = 0;
};

class OrderManager {
    std::map<int, std::unique_ptr<Order>> orders;
    int next_id = 1;
public:
    OMEvent place_order(Side side, double price, int qty);
    void cancel(int id);
    OMEvent handle_fill(int id, int qty);
    OMEvent update(const FeedEvent& ev);
};
