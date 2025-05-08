#include <iostream>
#include <queue>
#include <vector>
#include <functional>
#include <chrono>
#include "queue.hpp"

struct BidOrder {
    double price;
    int timestamp;

    bool operator<(const BidOrder& other) const {
        return price < other.price;
    }
};

void testPriorityQueue() {
    HeapPriorityQueue<BidOrder> pq;

    // Insert orders
    pq.insert(BidOrder{100.0, 1});
    pq.insert(BidOrder{200.0, 2});
    pq.insert(BidOrder{150.0, 3});

    // Check the top order (should be the highest price)
    BidOrder topOrder = pq.getMax();
    std::cout << "Top order price: " << topOrder.price << "\n";

    // Extract max and verify order extraction
    BidOrder extractedOrder = pq.popMax();
    std::cout << "Extracted order price: " << extractedOrder.price << "\n";
    
    std::cout <<"\n";
}

void benchmarkPriorityQueue(int numOperations) {
    std::cout << "Benchmarking HeapPriorityQueue with " << numOperations << " operations\n";

    // Create containers
    HeapPriorityQueue<BidOrder> custContainer;
    std::priority_queue<BidOrder> stdContainer;

    // Create orders
    std::vector<BidOrder> orders;
    orders.reserve(numOperations);
    for (int i = 0; i < numOperations; ++i) {
        orders.push_back(BidOrder{ static_cast<double>(i % 1000), i });
    }

    // Print headers
    std::cout << "operation,HeapPriorityQueue,priority_queue,diff\n";

    // Benchmark insertion
    auto start = std::chrono::high_resolution_clock::now();
    for (const auto& order : orders) {
        custContainer.insert(order);
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed1 = end - start;
    // Compare to priority_queue
    start = std::chrono::high_resolution_clock::now();
    for (const auto& order : orders) {
        stdContainer.push(order);
    }
    end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed2 = end - start;
    // Print results
    std::cout << "insertion," <<
                 elapsed1.count() << "," <<
                 elapsed2.count() << "," << 
                 elapsed1.count() - elapsed2.count() << "\n";    

    // Benchmark extraction
    start = std::chrono::high_resolution_clock::now();
    while (!custContainer.isEmpty()){
        custContainer.popMax();
    }
    end = std::chrono::high_resolution_clock::now();
    elapsed1 = end - start;
    // Compare to priority_queue
    start = std::chrono::high_resolution_clock::now();
    while (!stdContainer.empty()){
        stdContainer.pop();
    }
    end = std::chrono::high_resolution_clock::now();
    elapsed2 = end - start;
    std::cout << "extraction," <<
                 elapsed1.count() << "," <<
                 elapsed2.count() << "," << 
                 elapsed1.count() - elapsed2.count() << "\n";    

    std::cout << "\n";
}

int main() {
    testPriorityQueue();
    benchmarkPriorityQueue(10000);
    benchmarkPriorityQueue(100000);
    return 0;
}
