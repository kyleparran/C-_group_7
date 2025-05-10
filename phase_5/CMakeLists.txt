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

# Add fmt library for printing
include(FetchContent)
FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG        11.2.0
)
FetchContent_MakeAvailable(fmt)



add_executable(benchmark ${SRC_FILES})

target_include_directories(benchmark PRIVATE src)
target_link_libraries(benchmark PRIVATE fmt::fmt)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=native -flto")

enable_testing()
add_test(NAME Unit COMMAND benchmark --unit)
