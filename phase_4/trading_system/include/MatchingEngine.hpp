#pragma once
#include "OrderBook.hpp"
#include <vector>
#include "TradeLogger.hpp"
#include "OrderManager.hpp"

template<typename InputIt, typename Predicate>
InputIt find_if(InputIt first, InputIt last, Predicate pred) {
    for(; first != last; ++first) {
        if(pred(*first)) {
            return first;
        }
    }
    return last;
}


template<typename PriceType, typename OrderIdType>
class MatchingEngine {
    using OrderClassType = Order<PriceType, OrderIdType>;
    using OrderBookClassType = std::shared_ptr<OrderBook<PriceType, OrderIdType>>;
    using OrderPtr = std::shared_ptr<OrderClassType>;
    
    OrderBookClassType orderBook;
    TradeLogger<OrderIdType> logger;
public:
    MatchingEngine(OrderBookClassType orderBookPtr) : orderBook(orderBookPtr) {}

    std::vector<std::pair<OrderIdType, OrderIdType>> matchOrders(OrderManager<PriceType, OrderIdType>& orderMgr) {

        std::vector<std::pair<OrderIdType, OrderIdType>> trades;
        
        auto bestBid = orderBook->getBuyOrders().begin();
        auto endBid = orderBook->getBuyOrders().end();
        auto bestSell = orderBook->getSellOrders().begin();
        auto endSell = orderBook->getSellOrders().end();

        while(*bestBid >= *bestSell && 
              *bestBid != *endBid &&
              *bestSell != *endSell
        ){ 
            int bidId = bestBid->second->id;
            int sellId = bestSell->second->id;

            if (bestBid->second->quantity > bestSell->second->quantity) {
                ++bestSell;
                orderMgr.fillOrder(sellId);
                orderMgr.partialFillOrder(sellId, bestBid->second->quantity);
            } else if (bestBid->second->quantity < bestSell->second->quantity) {
                ++bestBid;
                orderMgr.fillOrder(bidId);
                orderMgr.partialFillOrder(bidId, bestSell->second->quantity);
            } else {
                ++bestSell; ++bestBid;
                orderMgr.fillOrder(bidId);
                orderMgr.fillOrder(sellId);
            }
            
            // Push back trade
            trades.emplace_back(bidId, sellId);
            logger.logTrade(bidId, sellId);
        }
        
        return trades;
    }

    void flushLogger() {
        logger.flush();
    }
};