#pragma once
#include <iostream>
#include <mutex>
#include <cstddef>

struct HeapAllocator {
    HeapAllocator(std::size_t = 0) { }

    template<typename U>
    U* allocate(std::size_t n) {
        return static_cast<U*>(::operator new(n * sizeof(U)));
    }

    template<typename U>
    void deallocate(U* p, std::size_t) {
        ::operator delete(p);
    }
};

struct StackAllocator {
    static const std::size_t BUFFER_SIZE = 1024;
    char buffer[BUFFER_SIZE];

    StackAllocator(std::size_t = 0) { }

    template<typename U>
    U* allocate(std::size_t n) {
        if(n * sizeof(U) > BUFFER_SIZE) 
            throw std::runtime_error("Requested size exceeds buffer size.");

        std::size_t bytes = n * sizeof(U);
        U* ptr = reinterpret_cast<U*>(buffer);
        return ptr;
    }

    template<typename U>
    void deallocate(U*, std::size_t) {
    }
};

struct NoLock {
    void lock() {}
    void unlock() {}
};

struct MutexLock {
    std::mutex m;
    void lock() { m.lock(); }
    void unlock() { m.unlock(); }
};

template<typename T, typename AllocatorPolicy, typename ThreadPolicy>
class OrderBookBuffer : private AllocatorPolicy, private ThreadPolicy {
    T* data;
    std::size_t capacity;
    std::size_t size_;

public:
    OrderBookBuffer(std::size_t cap)
        : AllocatorPolicy(cap), capacity(cap), size_(0)
    {
        data = this->AllocatorPolicy::template allocate<T>(cap);
    }

    ~OrderBookBuffer() {
        this->AllocatorPolicy::template deallocate<T>(data, capacity);
    }

    void add_order(const T& order) {
        this->lock();
        if(size_ < capacity) {
            data[size_] = order;
            size_++;
        }
        this->unlock();
    }

    void print_orders() {
        this->lock();
        for(std::size_t i = 0; i < size_; i++) {
            std::cout << data[i].id << " " << data[i].price << " " << data[i].qty << "\n";
        }
        this->unlock();
    }
};

