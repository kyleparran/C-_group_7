#pragma once
#include <unordered_map>
#include <map>
#include <vector>
#include <algorithm>

struct OBOrder {
    int id;
    double price;
    int quantity;
    char side;
};

class OrderBook {
    std::unordered_map<int, OBOrder> byId;
    std::map<double, std::vector<int>> bids;
    std::map<double, std::vector<int>> asks;

    std::map<double, std::vector<int>>& refBook(char s) { return s == 'B' ? bids : asks; }
    const std::map<double, std::vector<int>>& refBook(char s) const { return s == 'B' ? bids : asks; }

public:
    bool add(const OBOrder& o) {
        if (byId.find(o.id) != byId.end()) return false;
        byId[o.id] = o;
        refBook(o.side)[o.price].push_back(o.id);
        return true;
    }
    bool modify(int id, int q) {
        auto it = byId.find(id);
        if (it == byId.end()) return false;
        it->second.quantity = q;
        return true;
    }
    bool erase(int id) {
        auto it = byId.find(id);
        if (it == byId.end()) return false;
        auto& vec = refBook(it->second.side)[it->second.price];
        vec.erase(std::remove(vec.begin(), vec.end(), id), vec.end());
        if (vec.empty()) refBook(it->second.side).erase(it->second.price);
        byId.erase(it);
        return true;
    }
    std::vector<OBOrder> atPrice(double p, char s) const {
        std::vector<OBOrder> out;
        const auto& bk = refBook(s);
        auto it = bk.find(p);
        if (it == bk.end()) return out;
        for (int id : it->second) out.push_back(byId.at(id));
        return out;
    }
    bool bestBid(OBOrder& o) const {
        if (bids.empty()) return false;
        o = byId.at(bids.rbegin()->second.front());
        return true;
    }
    bool bestAsk(OBOrder& o) const {
        if (asks.empty()) return false;
        o = byId.at(asks.begin()->second.front());
        return true;
    }
};
