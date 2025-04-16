#include "feed_parser.h"
#include "market_snapshot.h"
#include "order_manager.h"

int main() {
    MarketSnapshot mktDB;
    OrderManager orderManager;
    
    auto feed = load_feed("sample_feed.txt");
    for (const auto& event : feed) {
        event.print();

        // Update current order book
        mktDB.update(event);
        mktDB.print();

        // Update orderManager
        orderManager.update(event);
        orderManager.print();

        // // todo
        // // Act on new data
        // if (should_trade(mktDB)) {
        //     int id = orderManager.place_order(Side::Buy, mktDB.get_best_bid()->price, 10);
        // }
    }
    return 0;
}