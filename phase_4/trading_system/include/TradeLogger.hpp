#pragma once
#include <vector>
#include <iostream>

template <typename OrderIdType>
class TradeLogger {
public:
    void logTrade(OrderIdType buy_id, OrderIdType sell_id) {
        trades.emplace_back(buy_id, sell_id);
        if(trades.size() >= batchSize) {
            flush();
        }
    }

    void flush() {
        for(auto &trade : trades)
            std::cout << "Trade: " << trade.first << " <--> " << trade.second << "\n";
        trades.clear();
    }
private:
    size_t batchSize = 10;
    std::vector<std::pair<OrderIdType, OrderIdType>> trades;
};
