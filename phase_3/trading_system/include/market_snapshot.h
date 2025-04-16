#pragma once
#include <map>
#include <memory>
#include <sstream>
#include <iomanip>
#include "feed_parser.h"



struct PriceLevel {
    double price;
    int quantity;

    PriceLevel(double p, int q);
    inline std::string to_string() const;
};
inline std::string PriceLevel::to_string() const{
    std::stringstream s;
    s << std::fixed << std::setprecision(2);
    s << price << "x" << quantity;
    return s.str();
}


class MarketSnapshot {
    private:
        std::map<double, std::unique_ptr<PriceLevel>> bids; // price -> quantity (sorted descending)
        std::map<double, std::unique_ptr<PriceLevel>> asks; // price -> quantity (sorted ascending)

    public:
        void update(const FeedEvent& event);
        inline const PriceLevel* get_best_bid() const;
        inline const PriceLevel* get_best_ask() const;
        inline std::string to_string() const;
};

inline std::string MarketSnapshot::to_string() const {
    std::stringstream s;
    s << std::fixed << std::setprecision(2);
    s << "Best bid: " << this->get_best_bid()->to_string() << "    " 
      << "Best ask: " << this->get_best_ask()->to_string();
    return s.str();
}

inline const PriceLevel* MarketSnapshot::get_best_bid() const {
    const std::unique_ptr<PriceLevel>& temp = this->bids.begin()->second;
    return temp.get();
}
inline const PriceLevel* MarketSnapshot::get_best_ask() const {
    const std::unique_ptr<PriceLevel>& temp = this->asks.begin()->second;
    return temp.get();
}