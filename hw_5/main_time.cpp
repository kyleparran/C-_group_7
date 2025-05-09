#include <fstream>
#include <vector>
#include <chrono>
#include <filesystem>
#include "time_series.hpp"

int main() {
    std::filesystem::create_directories("data");
    std::ofstream out("data/time_bench.csv");
    out << "pts,win,SIMD,scalar,diff\n";
    auto bench=[&](int pts,int w){
        std::vector<double> p(pts);
        for(int i=0;i<pts;++i) p[i]=100+(i%100)*0.01;
        auto t=std::chrono::high_resolution_clock::now();
        auto simdRes=movingAverageSIMD(p,w);
        auto t1=std::chrono::high_resolution_clock::now();
        auto scalarRes=movingAverageNormal(p,w);
        auto t2=std::chrono::high_resolution_clock::now();
        auto simdTime=(t1-t).count();
        auto scalarTime=(t2-t1).count();
        out << pts << ',' << w << ',' << simdTime << ',' << scalarTime << ',' << (simdTime - scalarTime) << '\n';
    };
    bench(10000,100);
    bench(10000,1000);
    bench(100000,100);
    bench(100000,1000);
}


