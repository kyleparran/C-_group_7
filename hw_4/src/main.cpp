#include <iostream>
#include "metaprogramming.hpp"
#include "constexpr_math.hpp"
#include "StaticVector.hpp"
#include "generic_algorithms.hpp"
#include "OrderBookBuffer.hpp"

struct Order {
    int id;
    double price;
    int qty;
};

int main() {
    // Part 1
    print_all("------PART 1------");
    static_assert(Factorial<5>::value == 120);
    static_assert(Fibonacci<7>::value == 13);
    static_assert(IsOdd<int, 5>::value);
    print_if_odd(5);
    print_all(1, 2, 3, "test");

    // // Part 2
    // print_all("\n------PART 2------");
    static_assert(factorial(5) == 120);
    static_assert(fibonacci(7) == 13);
    static_assert(square(5) == 25);
    static_assert(price_bucket(101.73) == 101.70);

    constexpr int Size = square(5);
    int arr[Size] = {0};

    // Part 3
    print_all("\n------PART 3------");
    StaticVector<Order, 5> vec;
    vec.push_back({1, 99.5, 5});
    vec.push_back({2, 101.0, 10});
    vec.push_back({3, 105.5, 15});

    print_all("All Prices geq 100:");
    auto itPrice = find_if(vec.begin(), vec.end(), [](const Order& o){ return o.price > 100.0; });
    while(itPrice != vec.end()) {
        print_all(itPrice->id, " ", itPrice->price, " ", itPrice->qty);
        itPrice = find_if(itPrice+1, vec.end(), [](const Order& o){ return o.price > 100.0; });
    }

    print_all("First Qty % 10:");
    auto itQty = find_if(vec.begin(), vec.end(), [](const Order& o){ return o.qty % 10 == 0; });
    if(itQty != vec.end()) {
        print_all(itQty->id, " ", itQty->price, " ", itQty->qty);
    }

    // Part 4
    print_all("\n------PART 4------");
    OrderBookBuffer<Order, StackAllocator, NoLock> book1(10);
    book1.add_order({10, 50.0, 2});
    book1.add_order({11, 70.0, 3});
    book1.print_orders();

    OrderBookBuffer<Order, HeapAllocator, MutexLock> book2(10);
    book2.add_order({20, 120.0, 5});
    book2.add_order({21, 150.0, 10});
    book2.print_orders();

    return 0;
}
