#pragma once
#include <map>
#include <memory>
#include "feed_parser.h"

enum class Side { BID, ASK };
enum class OrderStatus { New, Filled, PartiallyFilled, Cancelled };

struct Order {
    int id;
    Side side;
    double price;
    int quantity;
    int filled = 0;
    OrderStatus status = OrderStatus::New;

    int place_order(Side side, double price, int qty);
    void cancel(int id);
    void handle_fill(int id, int filled_qty);
    inline std::string to_string() const;
};
inline std::string Order::to_string() const {
    return "";  // todo
}

class OrderManager {
    private:
        std::map<int, std::unique_ptr<Order>> orders;

    public:
        void update(const FeedEvent& event);
        inline std::string to_string() const;
};
inline std::string OrderManager::to_string() const {
    return ""; // todo
}
