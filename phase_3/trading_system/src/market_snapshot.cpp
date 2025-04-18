#include "market_snapshot.h"

BookEvent MarketSnapshot::update(const FeedEvent& ev) {
    BookEvent out;
    if (ev.type == FeedType::BID) {
        if (ev.quantity == 0) {
            bool was_best = !bids.empty() && bids.begin()->first == ev.price;
            bids.erase(ev.price);
            if (was_best) out.type = BookEventType::BestBidRemoved;
        } else {
            bids[ev.price] = std::make_unique<PriceLevel>(ev.price, ev.quantity);
            if (bids.begin()->first == ev.price) {
                out.type = BookEventType::NewBestBid;
                out.price = ev.price;
                out.quantity = ev.quantity;
            }
        }
    } else if (ev.type == FeedType::ASK) {
        if (ev.quantity == 0) {
            bool was_best = !asks.empty() && asks.begin()->first == ev.price;
            asks.erase(ev.price);
            if (was_best) out.type = BookEventType::BestAskRemoved;
        } else {
            asks[ev.price] = std::make_unique<PriceLevel>(ev.price, ev.quantity);
            if (asks.begin()->first == ev.price) {
                out.type = BookEventType::NewBestAsk;
                out.price = ev.price;
                out.quantity = ev.quantity;
            }
        }
    }
    return out;
}

