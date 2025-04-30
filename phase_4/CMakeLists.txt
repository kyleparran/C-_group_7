cmake_minimum_required(VERSION 3.22.0)
project(main)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(main main.cpp)




# Add trading system code
add_subdirectory(trading_system)
               
# config.yaml to build folder
configure_file(${CMAKE_SOURCE_DIR}/config.yaml
               ${CMAKE_BINARY_DIR}/config.yaml
               COPYONLY)

target_link_libraries(main PRIVATE TradingSystem)