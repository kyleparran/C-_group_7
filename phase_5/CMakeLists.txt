cmake_minimum_required(VERSION 3.22)

project(Phase5HFT LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SRC_FILES
    src/OrderBook.cpp
    src/OptimizedOrderBook.cpp
    src/PooledOrderBook.cpp   
    src/benchmark.cpp
)

add_executable(benchmark ${SRC_FILES})

target_include_directories(benchmark PRIVATE src)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=native -flto")

enable_testing()
add_test(NAME Unit COMMAND benchmark --unit)
