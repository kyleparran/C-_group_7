#include "TradeLogger.hpp"

template<typename OrderIdType>
TradeLogger<OrderIdType>::TradeLogger() {
    outFile.open("output.log", std::ios::out | std::ios::trunc);
}

template<typename OrderIdType>
TradeLogger<OrderIdType>::~TradeLogger() {
    if (outFile.is_open()) {
        flush();
        outFile.close();
    }
}

template<typename OrderIdType>
void TradeLogger<OrderIdType>::logTrade(OrderIdType buy_id, OrderIdType sell_id) {
    trades.emplace_back(buy_id, sell_id);
    if (trades.size() >= batchSize) {
        flush();
    }
}

template<typename OrderIdType>
void TradeLogger<OrderIdType>::flush() {
    if (!outFile.is_open()) return;
    for (auto &trade : trades) {
        outFile << "Trade: " << trade.first << " <--> " << trade.second << "\n";
    }
    trades.clear();
    outFile.flush();
}

template class TradeLogger<int>;

