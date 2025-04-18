#include <spdlog/spdlog.h>
#include <spdlog/fmt/bundled/format.h>
#include <vector>
#include <string>
#include "feed_parser.h"
#include "market_snapshot.h"
#include "order_manager.h"
#include "config.h"
#include "logger.h"

int main() {
    AppConfig cfg = loadAppConfig("config.yaml");
    initLogger(cfg.logger);

    MarketSnapshot snap;
    OrderManager om;
    auto feed = load_feed("sample_feed.txt");

    std::vector<std::string> marketLogs;
    std::vector<std::string> otherLogs;

    double best_bid = 0.0;
    double best_ask = 0.0;
    double prev_ask = 0.0;

    bool ask_order_sent = false;
    bool bid_order_sent = false;
    int  ask_order_id   = 0;
    int  bid_order_id   = 0;

    constexpr double BID_INCREMENT = 0.02;
    constexpr double EPS = 1e-9;

    for (const auto& ev : feed) {
        OMEvent execEvt = om.update(ev);
        if (!execEvt.msg.empty()) otherLogs.push_back(execEvt.msg);
        if (execEvt.completed) {
            if (execEvt.order_id == ask_order_id) { ask_order_sent = false; ask_order_id = 0; }
            if (execEvt.order_id == bid_order_id) { bid_order_sent = false; bid_order_id = 0; }
        }

        if (ev.type == FeedType::BID) {
            if (ev.quantity > 0) {
                bool improved = (best_bid == 0.0 || ev.price > best_bid);
                if (improved) {
                    if (!bid_order_sent && best_bid > 0.0 &&
                        std::abs((ev.price - best_bid) - BID_INCREMENT) < EPS) {
                        OMEvent o = om.place_order(Side::Sell, ev.price, 100);
                        if (!o.msg.empty()) otherLogs.push_back(o.msg);
                        bid_order_sent = true;
                        bid_order_id = o.order_id;
                    }
                    marketLogs.push_back(fmt::format("[Market] New Best Bid: {:.2f} x {}", ev.price, ev.quantity));
                    best_bid = ev.price;
                }
            } else if (ev.price == best_bid) {
                marketLogs.push_back(fmt::format("[Market] Best Bid: {:.2f} removed", ev.price));
                best_bid = 0.0;
            }
        }

        if (ev.type == FeedType::ASK) {
            if (ev.quantity > 0) {
                bool first = best_ask == 0.0;
                bool better = !first && ev.price < best_ask;
                bool worse = !first && ev.price > best_ask;
                bool reopen_worse = first && prev_ask > 0.0 && ev.price > prev_ask;

                if (worse || reopen_worse) {
                    marketLogs.push_back(fmt::format("[Market] New Best Ask: {:.2f} x {}", ev.price, ev.quantity));
                    if (!ask_order_sent) {
                        double old_px = first ? prev_ask : best_ask;
                        OMEvent o = om.place_order(Side::Sell, old_px, 50);
                        if (!o.msg.empty()) otherLogs.push_back(o.msg);
                        ask_order_sent = true;
                        ask_order_id = o.order_id;
                    }
                    prev_ask = best_ask;
                    best_ask = ev.price;
                } else if (first || better) {
                    marketLogs.push_back(fmt::format("[Market] New Best Ask: {:.2f} x {}", ev.price, ev.quantity));
                    prev_ask = best_ask;
                    best_ask = ev.price;
                }
            } else if (ev.price == best_ask) {
                marketLogs.push_back(fmt::format("[Market] Best Ask: {:.2f} removed", ev.price));
                prev_ask = best_ask;
                best_ask = 0.0;
            }
        }

        snap.update(ev);
    }

    for (const auto& m : marketLogs) spdlog::info(m);
    spdlog::info("");
    for (const auto& o : otherLogs) {
        size_t start = 0;
        while (true) {
            size_t nl = o.find('\n', start);
            if (nl == std::string::npos) { spdlog::info(o.substr(start)); break; }
            spdlog::info(o.substr(start, nl - start));
            start = nl + 1;
        }
    }
    return 0;
}







