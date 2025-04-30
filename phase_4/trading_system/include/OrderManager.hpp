#pragma once
#include <memory>
#include <type_traits>
#include <string>
#include "Order.hpp"

enum class OrderState {
    New,
    Cancel,
    Partial,
    Filled
};

template<typename PriceType, typename OrderIdType>
class OrderManager {
    using OrderClassType = Order<PriceType, OrderIdType>;
    using OrderBookClassType = std::shared_ptr<OrderBook<PriceType, OrderIdType>>;
    using OrderPtr = std::shared_ptr<OrderClassType>;
    
    OrderBookClassType orderBook;
    std::unordered_map<OrderIdType, OrderState> orderStates;

    void updateState(OrderIdType id, OrderState state) {
        orderStates[id] = state;
    }
    
public:
    static_assert(std::is_integral<OrderIdType>::value, "Order ID must be an integer");

    OrderManager(OrderBookClassType orderBookPtr) : orderBook(orderBookPtr) {}

    OrderPtr createOrder(OrderIdType id, const std::string &symbol, PriceType price, int qty, bool is_buy) {
        OrderPtr order = std::make_shared<OrderClassType>(id, symbol, price, qty, is_buy);
        orderStates[id] = OrderState::New;

        orderBook->createAndAddOrder(order->id, order->symbol, order->price, order->quantity, order->is_buy);
        return order;
    }

    void cancelOrder(OrderIdType id) {
        orderBook->deleteOrder(id);
        updateState(id, OrderState::Cancel);
    }

    void fillOrder(OrderIdType id) {
        orderBook->deleteOrder(id);
        updateState(id, OrderState::Filled);
    }

    void partialFillOrder(OrderIdType id, int filledQty) {
        orderBook->partialFillOrder(id, filledQty);
        updateState(id, OrderState::Partial);
    }

    OrderState getState(OrderIdType id) const {
        auto it = orderStates.find(id);
        return (it != orderStates.end()) ? it->second : OrderState::New;
    }

    OrderBookClassType getOrderBook() const {
        return orderBook;
    }
};