cmake_minimum_required(VERSION 3.22.0)
add_library(TradingSystem src/TradeLogger.cpp)

target_include_directories(TradingSystem
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
)
target_compile_features(TradingSystem PRIVATE cxx_std_17)