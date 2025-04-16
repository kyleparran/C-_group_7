#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>

enum class FeedType {
    BID,
    ASK,
    EXECUTION,
    UNKNOWN
};

struct FeedEvent {
    FeedType type = FeedType::UNKNOWN;
    double price = 0.0;
    int quantity = 0;
    int order_id = -1; // used for EXECUTION only

    inline std::string to_string() const;
};
inline std::string FeedEvent::to_string() const {
    std::stringstream s;
    s << std::fixed << std::setprecision(2);
    switch (type) {
        case FeedType::BID:
            s << "[BID] " << price << "x" << quantity;
            break;
        case FeedType::ASK:
            s << "[ASK] " << price << "x" << quantity;
            break;
        case FeedType::EXECUTION:
            s << "[EXECUTION] Order " << order_id << " filled: " << quantity;
            break;
        default:
            s << "[UNKNOWN]";
    }
    return s.str();
}

std::vector<FeedEvent> load_feed(const std::string& filename);