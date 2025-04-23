# 🧠 HFT Template Programming Homework

This project demonstrates advanced C++ template features for low-latency financial systems.

## Structure

- `metaprogramming.hpp`: Compile-time recursion, traits, SFINAE
- `constexpr_math.hpp`: Compile-time functions using `constexpr`
- `StaticVector.hpp`: Fixed-size container with no dynamic allocation
- `generic_algorithms.hpp`: Custom `find_if`, `print_all`
- `OrderBookBuffer.hpp`: Policy-based design (allocators, threading)

## Building


mkdir build
cd build
cmake ..
make
./HFTTemplateHomework