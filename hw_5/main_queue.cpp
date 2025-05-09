#include <fstream>
#include <vector>
#include <queue>
#include <chrono>
#include <filesystem>
#include "queue.hpp"

struct Bid {
    double p;
    int ts;
    bool operator<(const Bid&o)const { return p<o.p; }
};

int main(){
    std::filesystem::create_directories("data");
    std::ofstream out("data/queue_bench.csv");
    out<<"n,op,my,std,diff\n";
    auto bench=[&](int n){
        HeapPriorityQueue<Bid> my;
        std::priority_queue<Bid> st;
        std::vector<Bid>d;
        d.reserve(n);
        for(int i=0; i<n; ++i) d.push_back({double(i%1000),i});
        auto t=std::chrono::high_resolution_clock::now();
        for(auto&o:d) my.insert(o);
        auto t1=std::chrono::high_resolution_clock::now();
        for(auto&o:d) st.push(o);
        auto t2=std::chrono::high_resolution_clock::now();
        out<<n<<",insert,"<<(t1-t).count()<<','<<(t2-t1).count()<<','<<(t1-t2).count()<<'\n';
        t=std::chrono::high_resolution_clock::now();
        while(!my.isEmpty()) my.popMax();
        t1=std::chrono::high_resolution_clock::now();
        while(!st.empty()) st.pop();
        t2=std::chrono::high_resolution_clock::now();
        out<<n<<",extract,"<<(t1-t).count()<<','<<(t2-t1).count()<<','<<(t1-t2).count()<<'\n';
    };
    bench(10000);
    bench(100000);
}
