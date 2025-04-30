#pragma once
#include <vector>
#include <utility>
#include <fstream> 

template <typename OrderIdType>
class TradeLogger {
public:
    TradeLogger();
    ~TradeLogger();
    void logTrade(OrderIdType buy_id, OrderIdType sell_id);
    void flush();
private:
    size_t batchSize = 10;
    std::vector<std::pair<OrderIdType, OrderIdType>> trades;
    std::ofstream outFile;  
};


