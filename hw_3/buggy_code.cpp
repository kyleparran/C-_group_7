#include <iostream>
#include <string>

struct Trade {
    std::string symbol;
    double price;
    Trade(const std::string& s, double p) : symbol(s), price(p) {
        std::cout << "Trade created: " << symbol << '\n';
    }
    ~Trade() {
        std::cout << "Trade destroyed: " << symbol << '\n';
    }
};

int main() {
    Trade* t1 = new Trade("AAPL", 150.0);
    Trade* t2 = new Trade("GOOG", 2800.0);

    delete t1;              // ✅ matched new / delete
    t1 = nullptr;           
    //You must never delete the same pointer twice. It corrupts memory 
    //allocation and causes crashes, as shown in part 1, it will pop 
    //up an error like "exception" that won't allow the program to run

    delete t2;              // ❌ originally leaked
    t2 = nullptr;

    Trade* t3 = new Trade("MSFT", 300.0);
    delete t3;              // free before reuse
    t3 = new Trade("TSLA", 750.0);
    delete t3;              // free second allocation
    t3 = nullptr;
    //We overwrote pointer t3 without first deleting
    // the original object. The original "MSFT" trade is now unreachable, causing a memory leak.
    Trade* trades = new Trade[3]{
        {"NVDA", 900.0},
        {"AMZN", 3200.0},
        {"META", 250.0}
    };
    delete[] trades;        // ✅ must use delete[] for new[]
    trades = nullptr;
//Arrays allocated with new[] must always be deleted with delete[]. 
//Using delete without brackets causes destructors for array elements 
//beyond the first to not run, creating memory leaks and possible heap corruption.
    return 0;
}







