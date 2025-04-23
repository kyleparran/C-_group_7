#pragma once
#include <cmath>
#include <type_traits>

constexpr int factorial(int n) {
    return n <= 1 ? 1 : n * factorial(n - 1);
}

constexpr int fibonacci(int n) {
    return n <= 1 ? n : fibonacci(n - 1) + fibonacci(n - 2);
}

constexpr int square(int x) {
    return x * x;
}

constexpr double price_bucket(double price) {
    int scaled = static_cast<int>(price * 20);
    return static_cast<double>(scaled) / 20.0;
}
