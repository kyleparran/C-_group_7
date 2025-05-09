#include <fstream>
#include <vector>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <map>               
#include "order_book.hpp"


int main() {
        std::cout << "\n*** ENTERED main_order_book ***\n"
              << "Current working directory: "
              << std::filesystem::current_path() << "\n\n"
              << "Press any key to exit..." << std::endl;
    try {
        auto dataDir = std::filesystem::current_path() / "data";
        std::filesystem::create_directories(dataDir);
        std::ofstream out(dataDir / "order_book_bench.csv");
        if (!out.is_open()) {
            std::cerr << "Cannot open CSV for writing." << std::endl;
            return 1;
        }

        OrderBook test;
        test.add({1, 101.0, 10, 'B'});
        test.add({2, 101.5,  5, 'S'});
        OBOrder bb, ba;
        test.bestBid(bb);
        test.bestAsk(ba);
        out << bb.price << ',' << ba.price << '\n';

        out << "n,add_hybrid,add_plain,mod_hybrid,mod_plain,del_hybrid,del_plain\n";

        /* ------------------------------------------------- benchmark helper */
        auto bench = [&](int n) {
            /* synthetic order flow */
            std::vector<OBOrder> data;
            data.reserve(n);
            for (int i = 0; i < n; ++i)
                data.push_back({i,
                                100.0 + (i % 1000) * 0.01,
                                1,
                                (i & 1) ? 'B' : 'S'});

            OrderBook h;
            auto t0 = std::chrono::high_resolution_clock::now();
            for (auto &o : data) h.add(o);
            auto t1 = std::chrono::high_resolution_clock::now();
            for (auto &o : data) h.modify(o.id, 2);
            auto t2 = std::chrono::high_resolution_clock::now();
            for (auto &o : data) h.erase(o.id);
            auto t3 = std::chrono::high_resolution_clock::now();

            std::map<double, std::vector<OBOrder>> p;
            for (auto &o : data) p[o.price].push_back(o);
            auto t4 = std::chrono::high_resolution_clock::now();
            for (auto &o : data)
                for (auto &v : p[o.price])
                    if (v.id == o.id) { v.quantity = 2; break; }
            auto t5 = std::chrono::high_resolution_clock::now();
            for (auto &o : data) {
                auto &vec = p[o.price];
                vec.erase(std::remove_if(vec.begin(), vec.end(),
                                         [&](auto &v) { return v.id == o.id; }),
                          vec.end());
                if (vec.empty()) p.erase(o.price);
            }
            auto t6 = std::chrono::high_resolution_clock::now();

            auto addH = std::chrono::duration<double>(t1 - t0).count();
            auto modH = std::chrono::duration<double>(t2 - t1).count();
            auto delH = std::chrono::duration<double>(t3 - t2).count();
            auto addP = std::chrono::duration<double>(t4 - t3).count();
            auto modP = std::chrono::duration<double>(t5 - t4).count();
            auto delP = std::chrono::duration<double>(t6 - t5).count();

            out << n << ',' << addH << ',' << addP << ','
                << modH << ',' << modP << ','
                << delH << ',' << delP << '\n';
        };

        bench(10'000);
        bench(100'000);

        std::cout << "Benchmarks complete. CSV saved to: "
                  << (dataDir / "order_book_bench.csv") << std::endl;
    }
    catch (const std::exception &e) {
        std::cerr << "Unhandled exception: " << e.what() << std::endl;
        return 1;
    }
}
