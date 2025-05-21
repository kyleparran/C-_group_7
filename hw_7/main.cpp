#include <array>
#include <atomic>
#include <barrier>
#include <chrono>
#include <execution>
#include <iostream>
#include <latch>
#include <mutex>
#include <numeric>
#include <semaphore>
#include <shared_mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

template<typename T,std::size_t Capacity>
class ConcurrentQueue{
public:
    ConcurrentQueue():head(0),tail(0){}
    bool push(const T& item){
        size_t currentTail=tail.load(std::memory_order_relaxed);
        size_t nextTail=(currentTail+1)%Capacity;
        if(nextTail==head.load(std::memory_order_acquire))return false;
        buffer[currentTail]=item;
        std::atomic_thread_fence(std::memory_order_release);
        tail.store(nextTail,std::memory_order_release);
        return true;
    }
    bool pop(T& item){
        size_t currentHead=head.load(std::memory_order_relaxed);
        if(currentHead==tail.load(std::memory_order_acquire))return false;
        item=buffer[currentHead];
        std::atomic_thread_fence(std::memory_order_release);
        head.store((currentHead+1)%Capacity,std::memory_order_release);
        return true;
    }
private:
    std::array<T,Capacity> buffer;
    std::atomic<size_t> head;
    std::atomic<size_t> tail;
};

struct Order{
    int orderId;
    std::string symbol;
    int quantity;
    double price;
    bool isBuy;
    std::chrono::high_resolution_clock::time_point enqueueTime;
    std::chrono::high_resolution_clock::time_point dequeueTime;
};

constexpr std::size_t QUEUE_CAPACITY=1024;
ConcurrentQueue<Order,QUEUE_CAPACITY> orderQueue;
std::unordered_map<std::string,std::pair<int,int>> orderBook;
std::shared_mutex orderBookMutex;
std::atomic<int> ordersProcessed{0};
std::counting_semaphore<3> processingSemaphore(3);
std::barrier syncBarrier(4);
std::latch startLatch(1);
std::vector<Order> completedOrders;
std::mutex completedOrdersMutex;

void orderProducer(int producerId,int numOrders){
    startLatch.wait();
    for(int i=0;i<numOrders;++i){
        Order order;
        order.orderId=producerId*1000+i;
        order.symbol=(i%2==0)?"APPL":"GOOGL";
        order.quantity=100+i;
        order.price=(order.symbol=="APPL")?146.23+i*0.01:2800.5+i*0.05;
        order.isBuy=(i%2==0);
        order.enqueueTime=std::chrono::high_resolution_clock::now();
        while(!orderQueue.push(order))std::this_thread::yield();
    }
}

void orderConsumer(){
    startLatch.wait();
    while(true){
        Order order;
        if(!orderQueue.pop(order)){
            if(ordersProcessed.load(std::memory_order_acquire)>=50)break;
            std::this_thread::yield();
            continue;
        }
        order.dequeueTime=std::chrono::high_resolution_clock::now();
        processingSemaphore.acquire();
        std::this_thread::sleep_for(std::chrono::microseconds(100));
        {
            std::unique_lock<std::shared_mutex> w(orderBookMutex);
            auto& e=orderBook[order.symbol];
            if(order.isBuy)e.first+=order.quantity;else e.second+=order.quantity;
        }
        {
            std::lock_guard<std::mutex> g(completedOrdersMutex);
            completedOrders.push_back(order);
        }
        ordersProcessed.fetch_add(1,std::memory_order_relaxed);
        processingSemaphore.release();
        syncBarrier.arrive_and_wait();
    }
}

void runAnalytics(){
    std::vector<double> latencies;
    latencies.reserve(completedOrders.size());
    for(const auto& o:completedOrders){
        latencies.push_back(std::chrono::duration_cast<std::chrono::microseconds>(o.dequeueTime-o.enqueueTime).count());
    }
    double totalLatency=std::reduce(std::execution::par,latencies.begin(),latencies.end(),0.0);
    double avgLatency=latencies.empty()?0.0:totalLatency/latencies.size();
    std::cout<< "Average latency: " << avgLatency << " µs\n";
}

int main(){
    const int numProducers=5;
    const int ordersPerProducer=10;
    const int numConsumers=4;
    std::vector<std::thread> producers,consumers;
    for(int i=0;i<numProducers;++i)producers.emplace_back(orderProducer,i+1,ordersPerProducer);
    for(int i=0;i<numConsumers;++i)consumers.emplace_back(orderConsumer);
    std::cout<<"Starting order processing...\n";
    startLatch.count_down();
    auto t0=std::chrono::high_resolution_clock::now();
    for(auto& p:producers)p.join();
    for(auto& c:consumers)c.join();
    auto t1=std::chrono::high_resolution_clock::now();
    std::cout<<"Total processed: "<<ordersProcessed.load()<<"\n";
    runAnalytics();
    {
        std::shared_lock<std::shared_mutex> r(orderBookMutex);
        std::cout<<"--- Order Book ---\n";
        for(const auto& [sym,vol]:orderBook){
            std::cout<<sym<<" | Buy "<<vol.first<<" | Sell "<<vol.second<<"\n";
        }
    }
    std::cout<<"Elapsed: "<<std::chrono::duration_cast<std::chrono::milliseconds>(t1-t0).count()<<" ms\n";
    syncBarrier.arrive_and_drop();
    return 0;
}

