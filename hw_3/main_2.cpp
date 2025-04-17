#include <iostream>
#include <string>
#include <utility>

struct Trade {
    std::string symbol;
    double price;
    Trade(const std::string& s, double p) : symbol(s), price(p) {}
};

class TradeHandle {
    Trade* ptr;
public:
    explicit TradeHandle(Trade* p = nullptr) : ptr(p) {}
    ~TradeHandle() { delete ptr; }
    TradeHandle(const TradeHandle&) = delete;
    TradeHandle& operator=(const TradeHandle&) = delete;
    TradeHandle(TradeHandle&& other) : ptr(other.ptr) { other.ptr = nullptr; }
    TradeHandle& operator=(TradeHandle&& other) {
        if (this != &other) { delete ptr; ptr = other.ptr; other.ptr = nullptr; }
        return *this;
    }
    Trade* operator->() { return ptr; }
    Trade& operator*()  { return *ptr; }
};

int main() {
    {
        TradeHandle h{ new Trade("IBM", 140.0) };
        std::cout << h->symbol << ' ' << h->price << '\n';
    }   // automatic cleanup
}








