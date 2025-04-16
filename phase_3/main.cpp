#include <spdlog/spdlog.h>
#include "feed_parser.h"
#include "market_snapshot.h"
#include "order_manager.h"
#include "config.h"
#include "logger.h"

int main() {
    AppConfig appConfig = loadAppConfig("config.yaml");
    MarketSnapshot mktDB;
    OrderManager orderManager;
    
    initLogger(appConfig.logger);
    
    auto feed = load_feed("sample_feed.txt");
    for (const auto& event : feed) {
        spdlog::info("[Feed Event] {}", event.to_string());

        // Update current order book
        mktDB.update(event);
        spdlog::info("[Market] {}", mktDB.to_string());

        // // Update orderManager
        // orderManager.update(event);
        // spdlog::info("[Order Book] {}", orderManager.to_string());

        // // todo
        // // Act on new data
        // if (should_trade(mktDB)) {
        //     int id = orderManager.place_order(Side::Buy, mktDB.get_best_bid()->price, 10);
        //     spdlog::info("[Order] {}", order.to_string());
        // }
    }
    return 0;
}