cmake_minimum_required(VERSION 3.10)
project(Main LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add AVX2 support
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-mavx2)
endif()

include_directories(include)

add_executable(MainHash main_hash.cpp)
add_executable(MainQueue main_queue.cpp)
add_executable(MainTime main_time.cpp)