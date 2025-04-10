#include "src.h"

int main() {
    std::vector<MarketData> feed;
    MarketDataFeed generator(feed);
    uint64_t start_ns = getCurrentTimeNs();
    generator.generateData(100000);
    TradeEngine engine(feed);
    engine.process();
    uint64_t end_ns = getCurrentTimeNs();
    uint64_t runtime_ms = (end_ns - start_ns) / 1000000ULL;
    engine.reportStats();
    engine.exportOrderHistory("order_history.csv");
    std::cout << "Total Runtime (ms): " << runtime_ms << std::endl;
    return 0;
}





