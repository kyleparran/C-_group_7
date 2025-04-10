#include "src.h"

MarketDataFeed::MarketDataFeed(std::vector<MarketData>& ref) : data(ref) {}

void MarketDataFeed::generateData(int num_ticks) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist(100.0, 200.0);
    data.reserve(num_ticks);
    for (int i = 0; i < num_ticks; ++i) {
        MarketData md;
        md.instrument_id = i % 10;
        md.price = dist(gen);
        md.timestamp = getCurrentTimeNs();
        data.push_back(md);
    }
}

TradeEngine::TradeEngine(const std::vector<MarketData>& feed)
    : market_data(feed), s1(0), s2(0), s3(0), s4(0) {}

void TradeEngine::process() {
    for (const auto& tick : market_data) {
        updateHistory(tick);
        bool buy = false, sell = false;
        bool sig1 = signal1(tick);
        bool sig2 = signal2(tick);
        bool sig3 = signal3(tick);
        bool sig4 = signal4(tick);
        if (sig1) {
            buy = true;
            s1++;
        }
        if (sig2) {
            double avg = getAvg(tick.instrument_id);
            if (tick.price < avg) buy = true; else sell = true;
            s2++;
        }
        if (sig3) {
            buy = true;
            s3++;
        }
        if (sig4) {
            double avg = getAvg(tick.instrument_id);
            if (tick.price < avg) buy = true; else sell = true;
            s4++;
        }
        if (buy || sell) {
            uint64_t now_ns = getCurrentTimeNs();
            Order o{tick.instrument_id, tick.price + (buy ? 0.01 : -0.01), buy, now_ns, sig1, sig2, sig3, sig4};
            orders.push_back(o);
            latencies.push_back(now_ns - tick.timestamp);
        }
    }
}

void TradeEngine::reportStats() {
    unsigned long long sum = 0, max_val = 0;
    for (auto l : latencies) {
        sum += l;
        if (l > max_val) max_val = l;
    }
    std::cout << "\n--- Performance Report ---\n";
    std::cout << "Total Market Ticks Processed: " << market_data.size() << "\n";
    std::cout << "Total Orders Placed: " << orders.size() << "\n";
    std::cout << "Signal1 Orders: " << s1 << "\n";
    std::cout << "Signal2 Orders: " << s2 << "\n";
    std::cout << "Signal3 Orders: " << s3 << "\n";
    std::cout << "Signal4 Orders: " << s4 << "\n";
    std::cout << "Average Tick-to-Trade Latency (ns): " 
              << (latencies.empty() ? 0 : sum / latencies.size()) << "\n";
    std::cout << "Maximum Tick-to-Trade Latency (ns): " << max_val << "\n";
}

void TradeEngine::exportOrderHistory(const std::string& filename) {
    std::ofstream file(filename);
    file << "instrument_id,price,is_buy,timestamp,signal1,signal2,signal3,signal4\n";
    for (auto &o : orders) {
        file << o.instrument_id << "," << o.price << "," << (o.is_buy ? 1 : 0) << ","
             << o.timestamp << "," << (o.from_signal1 ? 1 : 0) << ","
             << (o.from_signal2 ? 1 : 0) << "," << (o.from_signal3 ? 1 : 0) << ","
             << (o.from_signal4 ? 1 : 0) << "\n";
    }
}

void TradeEngine::updateHistory(const MarketData& tick) {
    auto& ps = price_history[tick.instrument_id];
    ps.window.push_back(tick.price);
    ps.sum += tick.price;
    ps.sum_of_squares += tick.price * tick.price;
    if (ps.window.size() > 10) {
        double old_price = ps.window.front();
        ps.window.pop_front();
        ps.sum -= old_price;
        ps.sum_of_squares -= old_price * old_price;
    }
}

double TradeEngine::getAvg(int id) {
    auto& ps = price_history[id];
    if (ps.window.empty()) return 0;
    return ps.sum / ps.window.size();
}

double TradeEngine::getStdDev(int id) {
    auto& ps = price_history[id];
    if (ps.window.size() < 2) return 0;
    double mean = ps.sum / ps.window.size();
    double mean_sq = ps.sum_of_squares / ps.window.size();
    return std::sqrt(mean_sq - mean * mean);
}

bool TradeEngine::signal1(const MarketData& tick) {
    return tick.price < 105.0 || tick.price > 195.0;
}

bool TradeEngine::signal2(const MarketData& tick) {
    if (price_history[tick.instrument_id].window.size() < 5) return false;
    double avg = getAvg(tick.instrument_id);
    return tick.price < avg * 0.98 || tick.price > avg * 1.02;
}

bool TradeEngine::signal3(const MarketData& tick) {
    auto& w = price_history[tick.instrument_id].window;
    if (w.size() < 3) return false;
    double diff1 = w[w.size() - 2] - w[w.size() - 3];
    double diff2 = w[w.size() - 1] - w[w.size() - 2];
    return diff1 > 0 && diff2 > 0;
}

bool TradeEngine::signal4(const MarketData& tick) {
    auto& w = price_history[tick.instrument_id].window;
    if (w.size() < 10) return false;
    double avg = getAvg(tick.instrument_id);
    double sd = getStdDev(tick.instrument_id);
    return tick.price > avg + sd * 2.0 || tick.price < avg - sd * 2.0;
}
