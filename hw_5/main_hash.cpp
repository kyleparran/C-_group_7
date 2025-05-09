#include <fstream>
#include <vector>
#include <chrono>
#include <unordered_map>
#include <filesystem>
#include <string>
#include <iostream>
#include "hash_table.hpp"

struct Ord {
    int    id = 0;
    double p  = 0.0;
    int    q  = 0;
    bool operator==(const Ord& o) const { return id == o.id; }
};

size_t symHash(const std::string& s) {
    uint32_t h = 2166136261u;
    for (char c : s) { h ^= (c & 0x1F); h *= 16777619u; }
    return h;
}

int main() {
    try {
        auto dataDir = std::filesystem::current_path() / "data";
        std::filesystem::create_directories(dataDir);
        std::ofstream out(dataDir / "hash_bench.csv");
        out << "ops,cap,op,my,std,diff\n";

        auto run = [&](int ops, int cap) {
            HashTable<std::string, Ord> my(cap, symHash);
            std::unordered_map<std::string, Ord> st; st.reserve(cap);

            std::vector<std::string> k(ops);
            for (int i = 0; i < ops; ++i) k[i] = "S" + std::to_string(i);

            auto t = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < ops; ++i) my.insert(k[i], {i, 99.0, 1});
            auto t1 = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < ops; ++i) st[k[i]] = {i, 99.0, 1};
            auto t2 = std::chrono::high_resolution_clock::now();
            out << ops << ',' << cap << ",insert,"
                << (t1 - t).count() << ','
                << (t2 - t1).count() << ','
                << (t1 - t2).count() << '\n';

            t = std::chrono::high_resolution_clock::now();
            for (auto& x : k) my.lookup(x);
            t1 = std::chrono::high_resolution_clock::now();
            for (auto& x : k) st.find(x);
            t2 = std::chrono::high_resolution_clock::now();
            out << ops << ',' << cap << ",lookup,"
                << (t1 - t).count() << ','
                << (t2 - t1).count() << ','
                << (t1 - t2).count() << '\n';

            t = std::chrono::high_resolution_clock::now();
            for (auto& x : k) my.remove(x);
            t1 = std::chrono::high_resolution_clock::now();
            for (auto& x : k) st.erase(x);
            t2 = std::chrono::high_resolution_clock::now();
            out << ops << ',' << cap << ",erase,"
                << (t1 - t).count() << ','
                << (t2 - t1).count() << ','
                << (t1 - t2).count() << '\n';
        };

        run(10000, 10000);
        run(10000, 100000);
        run(10000, 1000000);
        std::cout << "Done main_hash\n";
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
    }
}

