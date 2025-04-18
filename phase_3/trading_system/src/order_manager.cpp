#include "order_manager.h"
#include <spdlog/fmt/bundled/format.h>

OMEvent OrderManager::place_order(Side side, double price, int qty) {
    int id = next_id++;
    auto o = std::make_unique<Order>();
    o->id       = id;
    o->side     = side;
    o->price    = price;
    o->quantity = qty;
    orders[id]  = std::move(o);

    std::string side_str = (side == Side::Buy ? "BUY" : "SELL");
    std::string msg = fmt::format(
        "[Strategy] Placing {} order at {:.2f} x {} (ID = {})",
        side_str, price, qty, id);

    return {msg, false, id};
}

void OrderManager::cancel(int id) {
    orders.erase(id);
}

OMEvent OrderManager::handle_fill(int id, int qty) {
    auto it = orders.find(id);
    if (it == orders.end()) return {};

    Order& o = *it->second;
    o.filled += qty;

    std::string exec_msg = fmt::format("[Execution] Order {} filled: {}", id, qty);

    if (o.filled >= o.quantity) {
        std::string done_msg = fmt::format(
            "[Order] Order {} completed ({} / {}) and removed",
            id, o.filled, o.quantity);

        orders.erase(it);
        return {exec_msg + "\n" + done_msg, true, id};
    }

    o.status = OrderStatus::PartiallyFilled;
    std::string partial_msg = fmt::format(
        "[Order] Order {} partially filled: {} / {}",
        id, o.filled, o.quantity);

    return {exec_msg + "\n" + partial_msg, false, id};
}

OMEvent OrderManager::update(const FeedEvent& ev) {
    if (ev.type == FeedType::EXECUTION)
        return handle_fill(ev.order_id, ev.quantity);
    return {};
}






