#include "market_snapshot.h"
#include <map>
#include <memory>
#include <iostream>
#include "feed_parser.h"



PriceLevel::PriceLevel(double p, int q) : price(p), quantity(q) {}

// todo optimize
std::string PriceLevel::to_string() const {
    return std::to_string(this->price) + "x" + std::to_string(this->quantity);
}

inline void PriceLevel::print() const {
    std::cout << this->to_string() << std::endl;
}

// // Instead of 'print()', define operator<<:
// friend std::ostream& operator<<(std::ostream& os, const PriceLevel& pl) {
//     // Print to the output stream directly:
//     return os << pl.price << "x" << pl.quantity;
// }

void MarketSnapshot::update(const FeedEvent& event){
    if (event.type == FeedType::EXECUTION || event.type == FeedType::UNKNOWN){ return; }

    if (event.type == FeedType::BID){
        this->bids[event.price] = std::make_unique<PriceLevel>(
            event.price, event.quantity
        );
        return;
    }
    if (event.type == FeedType::ASK){
        this->asks[event.price] = std::make_unique<PriceLevel>(
            event.price, event.quantity
        );
        return;
    }
}

const PriceLevel* MarketSnapshot::get_best_bid() const {
    const std::unique_ptr<PriceLevel>& temp = this->bids.begin()->second;
    return temp.get();
}
const PriceLevel* MarketSnapshot::get_best_ask() const {
    const std::unique_ptr<PriceLevel>& temp = this->asks.begin()->second;
    return temp.get();
}

void MarketSnapshot::print() const {
    std::cout << "Best bid: " << this->get_best_bid()->to_string() << "    " 
              << "Best ask: " << this->get_best_ask()->to_string() << std::endl;
}