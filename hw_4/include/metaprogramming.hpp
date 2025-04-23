#pragma once
#include <type_traits>
#include <iostream>

template<int N>
struct Factorial {
    static constexpr int value = N * Factorial<N - 1>::value;
};

template<>
struct Factorial<0> {
    static constexpr int value = 1;
};

template<int N>
struct Fibonacci {
    static constexpr int value = Fibonacci<N - 1>::value + Fibonacci<N - 2>::value;
};

template<>
struct Fibonacci<0> {
    static constexpr int value = 0;
};

template<>
struct Fibonacci<1> {
    static constexpr int value = 1;
};

template<typename T, T N>
struct IsOdd {
    static constexpr bool value = (N % 2 != 0);
};

template<typename T>
typename std::enable_if<std::is_integral<T>::value>::type print_if_odd(T x) {
    if (x % 2 != 0) {
        std::cout << x << "\n";
    }
}

template<typename... Args>
void print_all(Args... args) {
    (std::cout << ... << args) << "\n";
}

