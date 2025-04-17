#include <iostream>
#include <string>
using namespace std;
struct Trade {
    string symbol;
    double price;

    Trade(const std::string& sym, double p)
        : symbol(sym), price(p) {}
};
int main(){
    Trade* single = new Trade("AAPL", 150.25);
    cout << single->symbol<< ' ' << single->price <<'\n';
    Trade* basket = new Trade[5]{
    {"GOOG", 2800.10},
    {"MSFT", 310.00},
    {"AMZN", 3300.00},
    {"TSLA", 780.52},
    {"NFLX", 590.03}
};
    for (size_t i =0; i<5; i++)
        cout<< basket[i].symbol << ' ' << basket[i].price << '\n';
    

    delete single;
    delete[] basket;
    return 0;

    






}








