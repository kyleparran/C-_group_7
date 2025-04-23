#pragma once
#include <cstddef>
#include <stdexcept>

template<typename T, std::size_t N>
class StaticVector {
    T data_[N];
    std::size_t size_ = 0;

public:
    void push_back(const T& value) {
        if(size_ < N) {
            data_[size_++] = value;
        }
    }

    T& operator[](std::size_t idx) {
        return data_[idx];
    }

    const T& operator[](std::size_t idx) const {
        return data_[idx];
    }

    std::size_t size() const {
        return size_;
    }

    T* begin() {
        return data_;
    }

    T* end() {
        return data_ + size_;
    }

    const T* begin() const {
        return data_;
    }

    const T* end() const {
        return data_ + size_;
    }
};
