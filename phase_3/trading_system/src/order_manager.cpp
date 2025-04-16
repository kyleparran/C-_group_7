#include "order_manager.h"
#include <map>
#include <memory>
#include "feed_parser.h"

// todo

int Order::place_order(Side side, double price, int qty){
    return -1;
}
void Order::cancel(int id){
    
}
void Order::handle_fill(int id, int filled_qty){
    
}
void Order::print() const {
    
}

void OrderManager::update(const FeedEvent& event){
    
}

void OrderManager::print() const {
}