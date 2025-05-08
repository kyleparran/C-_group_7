#include <iostream>
#include <functional>
#include <unordered_map>
#include <chrono>
#include "hash_table.hpp"

#define MAX_CAPACITY 30

struct Order {
    int id;
    double price;
    int qty;

    bool operator==(const Order& other) const {
        return id == other.id && price == other.price && qty == other.qty;
    }
};

size_t hashFunction(std::string key) {
    return std::hash<std::string>{}(key);
}

void testHashTable() {
    HashTable<std::string, Order> custContainer(MAX_CAPACITY, hashFunction);

    // Insert
    custContainer.insert("order1", Order{1, 100.0, 2});
    custContainer.insert("order2", Order{2, 200.0, 3});
    custContainer.insert("order3", Order{3, 300.0, 4});

    // Lookup
    Order* order1 = custContainer.lookup("order1");
    if (order1) std::cout << "Found order1: " << order1->id << std::endl;
    else std::cout << "Order1 not found" << std::endl;

    // Remove
    custContainer.remove("order1");
    order1 = custContainer.lookup("order1");
    if (!order1) std::cout << "Order1 successfully removed" << std::endl;

    std::cout <<"\n";
}

void benchmark(int numOperations, int maxCapacity) {
    std::cout << "Benchmarkings with " << numOperations << " operations, and " << maxCapacity << " max Cap\n";

    // Create containers
    HashTable<std::string, Order> custContainer(maxCapacity, hashFunction);
    std::unordered_map<std::string, Order> stdContainer;
    stdContainer.reserve(maxCapacity);
    
    // Create keys
    std::vector<std::string> keys(numOperations);
    for (int i = 0; i < numOperations; ++i) {
        keys[i] = "order" + std::to_string(i);
    }
    
    // Print headers
    std::cout << "operation,HashTable,unordered_map,diff\n";

    // Benchmark insertion
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < numOperations; ++i) {
        custContainer.insert(keys[i], Order{i, 100.0, 2});
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed1 = end - start;
    // Compare to unordered_map
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < numOperations; ++i) {
        stdContainer[keys[i]] = Order{i, 100.0, 2};
    }
    end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed2 = end - start;
    // Print results
    std::cout << "insertion," <<
                 elapsed1.count() << "," <<
                 elapsed2.count() << "," << 
                 elapsed1.count() - elapsed2.count() << "\n";    

    // Benchmark lookup
    start = std::chrono::high_resolution_clock::now();
    for (const auto& key : keys) {
        custContainer.lookup(key);
    }
    end = std::chrono::high_resolution_clock::now();
    elapsed1 = end - start;
    // Compare to unordered_map
    start = std::chrono::high_resolution_clock::now();
    for (const auto& key : keys) {
        stdContainer.find(key);
    }
    end = std::chrono::high_resolution_clock::now();
    elapsed2 = end - start;
    // Print results
    std::cout << "lookup," << 
                 elapsed1.count() << "," <<
                 elapsed2.count() << "," << 
                 elapsed1.count() - elapsed2.count() << "\n";

    // Benchmark removal
    start = std::chrono::high_resolution_clock::now();
    for (const auto& key : keys) {
        custContainer.remove(key);
    }
    end = std::chrono::high_resolution_clock::now();
    elapsed1 = end - start;
    // Compare to unordered_map
    start = std::chrono::high_resolution_clock::now();
    for (const auto& key : keys) {
        stdContainer.erase(key);
    }
    end = std::chrono::high_resolution_clock::now();
    elapsed2 = end - start;
    // Print results
    std::cout << "removal," << 
                 elapsed1.count() << "," <<
                 elapsed2.count() << "," << 
                 elapsed1.count() - elapsed2.count() << "\n";
    
    std::cout <<"\n";
}

int main() {
    testHashTable();
    benchmark(10000, 10000);
    benchmark(10000, 100000);
    benchmark(10000, 1000000);
    return 0;
}