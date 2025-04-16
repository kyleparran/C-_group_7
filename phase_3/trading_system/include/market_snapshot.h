#pragma once
#include <map>
#include <memory>
#include "feed_parser.h"



struct PriceLevel {
    double price;
    int quantity;

    PriceLevel(double p, int q);
    inline void print() const;
    inline std::string to_string() const;
};



class MarketSnapshot {
    private:
        std::map<double, std::unique_ptr<PriceLevel>> bids; // price -> quantity (sorted descending)
        std::map<double, std::unique_ptr<PriceLevel>> asks; // price -> quantity (sorted ascending)

    public:
        void update(const FeedEvent& event);
        inline const PriceLevel* get_best_bid() const;
        inline const PriceLevel* get_best_ask() const;
        void print() const;
};
