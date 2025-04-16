#include "market_snapshot.h"
#include <map>
#include <memory>
#include <iostream>
#include "feed_parser.h"



PriceLevel::PriceLevel(double p, int q) : price(p), quantity(q) {}



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