cmake_minimum_required(VERSION 3.22.0)
add_library(TradingSystem 
    src/feed_parser.cpp
    src/market_snapshot.cpp
    src/order_manager.cpp
    src/config.cpp
    src/logger.cpp
)

target_include_directories(TradingSystem
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
)