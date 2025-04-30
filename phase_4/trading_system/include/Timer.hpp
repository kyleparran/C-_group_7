#pragma once
#include <chrono>

class Timer {
public:
    void start() {
        m_start = std::chrono::high_resolution_clock::now();
    }

    long long stop() {
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::nanoseconds>(end - m_start).count();
    }

private:
    std::chrono::high_resolution_clock::time_point m_start;
};