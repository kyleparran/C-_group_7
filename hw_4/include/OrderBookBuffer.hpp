#pragma once
#include <iostream>
#include <mutex>
#include <cstddef>

// ----------------- HeapAllocator -------------------
struct HeapAllocator {
    // Accept size_t but ignore it
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

// ----------------- StackAllocator -------------------
struct StackAllocator {
    char* buffer;
    std::size_t capacity;
    std::size_t offset;

    StackAllocator(std::size_t cap)
        : buffer(nullptr), capacity(cap), offset(0)
    {
        buffer = new char[cap];
    }

    ~StackAllocator() {
        delete[] buffer;
    }

    template<typename U>
    U* allocate(std::size_t n) {
        std::size_t bytes = n * sizeof(U);
        if(offset + bytes <= capacity) {
            U* ptr = reinterpret_cast<U*>(buffer + offset);
            offset += bytes;
            return ptr;
        }
        return nullptr;
    }

    template<typename U>
    void deallocate(U*, std::size_t) {
        // do nothing for stack-based
    }
};

// ----------------- NoLock & MutexLock ---------------
struct NoLock {
    void lock() {}
    void unlock() {}
};

struct MutexLock {
    std::mutex m;
    void lock() { m.lock(); }
    void unlock() { m.unlock(); }
};

// ----------- Policy-Based OrderBookBuffer -----------
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

