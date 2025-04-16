#include "feed_parser.h"
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <spdlog/spdlog.h>


std::vector<FeedEvent> load_feed(const std::string& filename) {
    std::ifstream file(filename);
    std::vector<FeedEvent> events;

    if (!file.is_open()) {
        spdlog::error("Error: could not open file {}", filename);
        return events;
    }

    std::string line;
    while (std::getline(file, line)) {
        // Ignore comments and empty lines
        if (line.empty() || line[0] == '#') continue;

        std::istringstream iss(line);
        std::string type;
        iss >> type;

        if (type == "BID") {
            double price;
            int qty;
            if (iss >> price >> qty) {
                events.push_back({FeedType::BID, price, qty});
            }
        } else if (type == "ASK") {
            double price;
            int qty;
            if (iss >> price >> qty) {
                events.push_back({FeedType::ASK, price, qty});
            }
        } else if (type == "EXECUTION") {
            int order_id;
            int filled;
            if (iss >> order_id >> filled) {
                events.push_back({FeedType::EXECUTION, 0.0, filled, order_id});
            }
        } else {
            spdlog::error("Unknown event type: {}", type);
        }
    }

    return events;
}