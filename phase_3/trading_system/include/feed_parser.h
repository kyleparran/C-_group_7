#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>

enum class FeedType { BID, ASK, EXECUTION, UNKNOWN };

struct FeedEvent {
    FeedType type = FeedType::UNKNOWN;
    double   price = 0.0;
    int      quantity = 0;
    int      order_id = -1;

    inline std::string to_string() const {
        std::ostringstream oss;
        switch (type) {
            case FeedType::BID:        oss << "[BID] " << price << " x " << quantity; break;
            case FeedType::ASK:        oss << "[ASK] " << price << " x " << quantity; break;
            case FeedType::EXECUTION:  oss << "[EXECUTION] Order " << order_id << " filled: " << quantity; break;
            default:                   oss << "[UNKNOWN]";
        }
        return oss.str();
    }
};

std::vector<FeedEvent> load_feed(const std::string& filename);

