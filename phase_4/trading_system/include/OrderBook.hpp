#pragma once
#include <memory>
#include <map>
#include <vector>
#include <cstdlib>
#include "Order.hpp"

template <typename T>
struct PoolAllocator {
    using value_type = T;
    
    PoolAllocator() = default;
    
    template <typename U>
    PoolAllocator(const PoolAllocator<U>&) { }
    
    T* allocate(std::size_t n) {
        return static_cast<T*>(::operator new(n * sizeof(T)));
    }
    
    void deallocate(T* p, std::size_t) {
        ::operator delete(p);
    }
};

template <typename T, typename U>
bool operator==(const PoolAllocator<T>&, const PoolAllocator<U>&) { return true; }
template <typename T, typename U>
bool operator!=(const PoolAllocator<T>&, const PoolAllocator<U>&) { return false; }

template<typename PriceType, typename OrderIdType>
class OrderBook {
protected:
    using OrderClassType = Order<PriceType, OrderIdType>;
    using OrderClassPtr = std::unique_ptr<OrderClassType>;
    using OrderPair = std::pair<const PriceType, OrderClassPtr>;

    OrderClassType* data;
    std::size_t capacity;

    std::multimap<PriceType, OrderClassPtr, std::greater<PriceType>, PoolAllocator<OrderPair>> buy_orders;
    std::multimap<PriceType, OrderClassPtr, std::less<PriceType>, PoolAllocator<OrderPair>> sell_orders;
public:
    OrderBook(std::size_t cap)
        : capacity(cap),
          buy_orders(std::greater<PriceType>(), PoolAllocator<OrderPair>()),
          sell_orders(std::less<PriceType>(), PoolAllocator<OrderPair>())
    {
        data = PoolAllocator<OrderClassType>().allocate(cap);
    }

    virtual ~OrderBook() {
        PoolAllocator<OrderClassType>().deallocate(data, capacity);
    }

    virtual void createAndAddOrder(OrderIdType id, const std::string &symbol, PriceType price, int qty, bool is_buy) {
        OrderClassType* order = PoolAllocator<OrderClassType>().allocate(1);
        new (order) OrderClassType(id, symbol, price, qty, is_buy);

        if (is_buy)
            buy_orders.emplace(price, OrderClassPtr(order));
        else
            sell_orders.emplace(price, OrderClassPtr(order));
    }

    virtual void deleteOrder(OrderIdType id) {
        auto it = std::find_if(buy_orders.begin(), buy_orders.end(), 
            [id](const auto& order) { return order.second->id == id; });
        
        if (it != buy_orders.end()) {
            buy_orders.erase(it);
            return;
        }
        
        it = std::find_if(sell_orders.begin(), sell_orders.end(), 
            [id](const auto& order) { return order.second->id == id; });
        
        if (it != sell_orders.end()) {
            sell_orders.erase(it);
        }
    }

    virtual void partialFillOrder(OrderIdType id, int filledQty) {
        auto it = std::find_if(buy_orders.begin(), buy_orders.end(), 
            [id](const auto& order) { return order.second->id == id; });
        
        if (it != buy_orders.end()) {
            it->second->quantity -= filledQty;
            return;
        }
        
        it = std::find_if(sell_orders.begin(), sell_orders.end(), 
            [id](const auto& order) { return order.second->id == id; });
        
        if (it != sell_orders.end()) {
            it->second->quantity -= filledQty;
        }
    }

    auto& getBuyOrders() { return buy_orders; }
    auto& getSellOrders() { return sell_orders; }
};

template<typename PriceType, typename OrderIdType>
class OrderBookNewPtrs : public OrderBook<PriceType, OrderIdType> {
    using OrderBookClassType = OrderBook<PriceType, OrderIdType>;
    using OrderClassType = Order<PriceType, OrderIdType>;
    using OrderClassPtr = OrderClassType*;
    using OrderPair = std::pair<const PriceType, OrderClassPtr>;

    std::multimap<PriceType, OrderClassPtr, std::greater<PriceType>, PoolAllocator<OrderPair>> buy_orders;
    std::multimap<PriceType, OrderClassPtr, std::less<PriceType>, PoolAllocator<OrderPair>> sell_orders;

public:
    OrderBookNewPtrs(std::size_t cap)
        : OrderBookClassType(cap),
          buy_orders(std::greater<PriceType>(), PoolAllocator<OrderPair>()),
          sell_orders(std::less<PriceType>(), PoolAllocator<OrderPair>())
    { }

    virtual void createAndAddOrder(OrderIdType id, const std::string &symbol, PriceType price, int qty, bool is_buy) override {
        OrderClassType* order = PoolAllocator<OrderClassType>().allocate(1);
        new (order) OrderClassType(id, symbol, price, qty, is_buy);

        if (is_buy)
            buy_orders.emplace(price, order);
        else
            sell_orders.emplace(price, order);
    }
};



template<typename PriceType, typename OrderIdType>
class OrderBookNewDelete : public OrderBook<PriceType, OrderIdType> {
    using OrderBookClassType = OrderBook<PriceType, OrderIdType>;
    using OrderClassType = Order<PriceType, OrderIdType>;
    using OrderClassPtr = OrderClassType*;
    using OrderPair = std::pair<const PriceType, OrderClassPtr>;

    std::multimap<PriceType, OrderClassPtr, std::greater<PriceType>> buy_orders;
    std::multimap<PriceType, OrderClassPtr, std::less<PriceType>> sell_orders;

public:
    OrderBookNewDelete(std::size_t cap)
        : OrderBookClassType(cap),
          buy_orders(std::greater<PriceType>()),
          sell_orders(std::less<PriceType>())
    { }
};

template<typename PriceType, typename OrderIdType>
class OrderBookFlat : public OrderBook<PriceType, OrderIdType> {
    using OrderBookClassType = OrderBook<PriceType, OrderIdType>;
    using OrderClassType = Order<PriceType, OrderIdType>;
    using OrderClassPtr = std::unique_ptr<OrderClassType>;
    using OrderPair = std::pair<const PriceType, OrderClassPtr>;

    std::vector<OrderClassType> buy_orders;
    std::vector<OrderClassType> sell_orders;
    std::size_t buyIdx;
    std::size_t sellIdx;

public:
    OrderBookFlat(std::size_t capacity)
        : OrderBookClassType(capacity), buyIdx(0), sellIdx(0)
    {
        buy_orders.reserve(capacity);
        sell_orders.reserve(capacity);
    }

    virtual void createAndAddOrder(OrderIdType id, const std::string &symbol,
                                    PriceType price, int qty, bool is_buy) override
    {
        OrderClassType newOrder(id, symbol, price, qty, is_buy);
        
        if (is_buy) {
            buy_orders.push_back(newOrder);
            buyIdx = buy_orders.size();
        } else {
            sell_orders.push_back(newOrder);
            sellIdx = sell_orders.size();
        }
    }

    virtual void deleteOrder(OrderIdType id) override
    {
        auto it = std::find_if(buy_orders.begin(), buy_orders.end(),
            [id](const OrderClassType &order) { return order.id == id; });
        if (it != buy_orders.end()) {
            buy_orders.erase(it);
            buyIdx = buy_orders.size();
            return;
        }
        
        it = std::find_if(sell_orders.begin(), sell_orders.end(),
            [id](const OrderClassType &order) { return order.id == id; });
        if (it != sell_orders.end()) {
            sell_orders.erase(it);
            sellIdx = sell_orders.size();
        }
    }

    virtual void partialFillOrder(OrderIdType id, int filledQty) override
    {
        auto it = std::find_if(buy_orders.begin(), buy_orders.end(),
            [id](const OrderClassType &order) { return order.id == id; });
        if (it != buy_orders.end()) {
            it->quantity -= filledQty;
            return;
        }
        
        it = std::find_if(sell_orders.begin(), sell_orders.end(),
            [id](const OrderClassType &order) { return order.id == id; });
        if (it != sell_orders.end()) {
            it->quantity -= filledQty;
        }
    }
};