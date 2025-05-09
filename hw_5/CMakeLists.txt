cmake_minimum_required(VERSION 3.10)
project(Main LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})


if(MSVC)
    add_compile_options(/O2)
else()
    add_compile_options(-O3 -march=native)
endif()

include_directories(include)

add_executable(MainHash main_hash.cpp)
add_executable(MainQueue main_queue.cpp)
add_executable(MainTime main_time.cpp)
add_executable(main_order_book main_order_book.cpp)

foreach(target IN ITEMS MainHash MainQueue MainTime main_order_book)
    set_target_properties(${target}
        PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
endforeach()
