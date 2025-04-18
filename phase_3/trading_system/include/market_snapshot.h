#pragma once
#include <map>
#include <memory>
#include "feed_parser.h"

struct PriceLevel {
    double price;
    int quantity;
    PriceLevel(double p = 0.0, int q = 0) : price(p), quantity(q) {}
};

enum class BookEventType { None, NewBestBid, NewBestAsk, BestBidRemoved, BestAskRemoved };

struct BookEvent {
    BookEventType type = BookEventType::None;
    double price = 0.0;
    int quantity = 0;
};

class MarketSnapshot {
    std::map<double, std::unique_ptr<PriceLevel>, std::greater<double>> bids;
    std::map<double, std::unique_ptr<PriceLevel>> asks;
public:
    BookEvent update(const FeedEvent& ev);
    const PriceLevel* get_best_bid() const {
        return bids.empty() ? nullptr : bids.begin()->second.get();
    }
    const PriceLevel* get_best_ask() const {
        return asks.empty() ? nullptr : asks.begin()->second.get();
    }
};
