#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

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

    // Debug print
    void print() const;
};

std::vector<FeedEvent> load_feed(const std::string& filename);