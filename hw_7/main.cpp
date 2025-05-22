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
#include <iterator>
#include <cmath>

template <typename T, std::size_t Capacity>
class ConcurrentQueue {
public:
    ConcurrentQueue() : head{0}, tail{0} {}

    // push_back an item into the queue. Returns false if the queue is full.
    bool push_back(const T& item) {
        while(true) {
            size_t currentTail = tail.load(std::memory_order_acquire);
            size_t nextTail = (currentTail + 1) % Capacity;
            if (nextTail == head.load(std::memory_order_acquire)) return false;

            // If tail hasnt been changed by another thread then update to nextTail and do logic
            if (tail.compare_exchange_weak(
                    currentTail, 
                    nextTail, 
                    std::memory_order_acq_rel, 
                    std::memory_order_acquire
                )) {
                buffer[currentTail] = item;
                return true;
            }
        }
    }

    // Pop an item from the queue. Returns false if the queue is empty.
    bool pop(T& item) {
        while (true) { 
            size_t currentHead = head.load(std::memory_order_relaxed);
            if (currentHead == tail.load(std::memory_order_acquire)) return false;

            // If head hasnt been changed by another thread then update to nextHead and do logic
            size_t nextHead = (currentHead + 1) % Capacity;
            if (head.compare_exchange_weak(
                    currentHead, 
                    nextHead, 
                    std::memory_order_acq_rel, 
                    std::memory_order_acquire
                )) {
                item = buffer[currentHead];
                return true;
            }
        }
    }

    // Iterator class
    class Iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = T*;
        using reference = T&;

        Iterator(ConcurrentQueue& queue, size_t position)
            : queue_(queue), position_(position) {}

        reference operator*() const {
            return queue_.buffer[position_];
        }

        pointer operator->() {
            return &queue_.buffer[position_];
        }

        // Prefix increment
        Iterator& operator++() {
            position_ = (position_ + 1) % Capacity;
            return *this;
        }

        // Postfix increment
        Iterator operator++(int) {
            Iterator temp = *this;
            ++(*this);
            return temp;
        }

        friend bool operator==(const Iterator& a, const Iterator& b) {
            return a.position_ == b.position_;
        }

        friend bool operator!=(const Iterator& a, const Iterator& b) {
            return a.position_ != b.position_;
        }

    private:
        ConcurrentQueue& queue_;
        size_t position_;
    };

    // Begin iterator
    Iterator begin() {
        return Iterator(*this, head.load(std::memory_order_acquire));
    }

    // End iterator
    Iterator end() {
        return Iterator(*this, tail.load(std::memory_order_acquire));
    }

    int size() const {
        return (tail.load(std::memory_order_acquire) - head.load(std::memory_order_acquire) + Capacity) % Capacity;
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

constexpr int numProducers=50;
constexpr int ordersPerProducer=100;
constexpr int numConsumers=40;
constexpr int totalOrders = numProducers * ordersPerProducer;


constexpr std::size_t QUEUE_CAPACITY = totalOrders + 1; // Implementation uses blank space to to determine emptiness
ConcurrentQueue<Order, QUEUE_CAPACITY> orderQueue;

std::unordered_map<std::string,std::pair<int,int>> orderBook;
std::shared_mutex orderBookMutex;

// std::mutex completedOrdersMutex;
// std::vector<Order> completedOrders;
ConcurrentQueue<Order, QUEUE_CAPACITY> completedOrders;

// std::counting_semaphore<30> processingSemaphore(30);
// std::barrier syncBarrier(40);
std::latch startLatch(1);

// Producer thread function: simulates incoming orders.
void orderProducer(int producerId, int numOrders) {
    startLatch.wait();  // Wait until all threads are ready to start
    for (int i = 0; i < numOrders; ++i) {
        Order order;
        order.orderId = producerId * 1000 + i;
        order.symbol = (i % 2 == 0) ? "APPL" : "GOOGL";
        order.quantity = 100 + i;
        order.price = (order.symbol == "APPL") ? 146.23 + i * 0.01 : 2800.5 + i * 0.05;
        order.isBuy = (i % 2 == 0);
        order.enqueueTime = std::chrono::high_resolution_clock::now();
        // Try to push_back; if full, keep trying (busy-wait for simplicity)
        while (!orderQueue.push_back(order)) {
            std::this_thread::yield();
        }
    }
}

void orderConsumer(){
    startLatch.wait();
    while(true){
        Order order;
        if(!orderQueue.pop(order)){
            if(completedOrders.size()>=totalOrders)break;
            std::this_thread::yield();
            continue;
        }
        order.dequeueTime=std::chrono::high_resolution_clock::now();
        // processingSemaphore.acquire();
        std::this_thread::sleep_for(std::chrono::microseconds(100));
        
        // Update global order book (exclusive write via unique_lock)
        {
            std::scoped_lock<std::shared_mutex> w(orderBookMutex);
            auto& entry = orderBook[order.symbol];
            if (order.isBuy)
                entry.first += order.quantity;   // Update buy volume
            else
                entry.second += order.quantity;  // Update sell volume
        }

        // Update completed orders
        completedOrders.push_back(order);
        // {
        //     std::scoped_lock<std::mutex> w(completedOrdersMutex);
        //     completedOrders.push_back(order);
        // }
        
        
        // processingSemaphore.release();
        // syncBarrier.arrive_and_wait();  // todo
    }
}

void runAnalytics() {
    std::vector<double> latencies;
    latencies.reserve(completedOrders.size());
    for (const auto& o : completedOrders) {
        latencies.push_back(std::chrono::duration_cast<std::chrono::microseconds>(o.dequeueTime - o.enqueueTime).count());
    }
    
    if (latencies.empty()) {
        std::cout << "Average latency: 0.0 µs\n";
        std::cout << "Variance: 0.0 µs²\n";
        std::cout << "Q1: 0.0 µs\n";
        std::cout << "Q2: 0.0 µs\n";
        std::cout << "Q3: 0.0 µs\n";
        return;
    }

    double totalLatency = std::reduce(std::execution::par, latencies.begin(), latencies.end(), 0.0);
    double avgLatency = totalLatency / latencies.size();

    double variance = std::transform_reduce(
        std::execution::par,
        latencies.begin(),
        latencies.end(),
        0.0,
        std::plus<>(),
        [avgLatency](double latency) {
            return (latency - avgLatency) * (latency - avgLatency);
        }
    ) / latencies.size();

    // Sort the latencies to calculate quartiles
    std::sort(latencies.begin(), latencies.end());

    // Calculate quartiles
    auto quartileIndex = [](size_t n, double p) {
        return static_cast<size_t>(p * (n - 1));
    };

    double q1 = latencies[quartileIndex(latencies.size(), 0.25)];
    double q2 = latencies[quartileIndex(latencies.size(), 0.50)];
    double q3 = latencies[quartileIndex(latencies.size(), 0.75)];

    std::cout << "\n--- Analytics ---\n";
    std::cout << "Avg Latency:  " << avgLatency << " µs\n";
    std::cout << "Std Latency:  " << std::sqrt(variance) << " µs\n";
    std::cout << "Q1 | Q2 | Q3: " << q1 << " | " << q2 << " | " << q3 << " µs²\n";
}

int main(){
    std::vector<std::thread> producers,consumers;
    for(int i=0;i<numProducers;++i)producers.emplace_back(orderProducer,i+1,ordersPerProducer);
    for(int i=0;i<numConsumers;++i)consumers.emplace_back(orderConsumer);

    std::cout << "Starting order processing...\n";
    startLatch.count_down();
    auto t0=std::chrono::high_resolution_clock::now();
    for(auto& p:producers)p.join();;
    for(auto& c:consumers)c.join();
    auto t1=std::chrono::high_resolution_clock::now();
    std::cout << "Total processed: " << completedOrders.size() << "/" << totalOrders << "\n";
    std::cout<<"Total Time Elapsed: " << std::chrono::duration_cast<std::chrono::milliseconds>(t1-t0).count()<<" ms\n";

    runAnalytics();
    
    // Display order book summary
    {
        std::shared_lock<std::shared_mutex> r(orderBookMutex);
        std::cout<<"\n--- Order Book ---\n";
        for(const auto& [sym,vol]:orderBook){
            std::cout<<sym<<" | Buy "<<vol.first<<" | Sell "<<vol.second<<"\n";
        }
    }

    // syncBarrier.arrive_and_drop();
    return 0;
}

