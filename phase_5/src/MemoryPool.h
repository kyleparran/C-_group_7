#pragma once
#include <cstddef>
#include <new>
#include <utility>

template<typename T>
class FixedMemoryPool {
    T* buffer;
    std::size_t cap;
    std::size_t used;
public:
    explicit FixedMemoryPool(std::size_t n) : buffer(static_cast<T*>(::operator new[](sizeof(T) * n))),
                                              cap(n), used(0) {}
    ~FixedMemoryPool() { ::operator delete[](buffer); }
    template<typename... Args>
    T* create(Args&&... args) {
        if (used >= cap) return nullptr;
        T* p = buffer + used++;
        ::new (p) T(std::forward<Args>(args)...);
        return p;
    }
};
