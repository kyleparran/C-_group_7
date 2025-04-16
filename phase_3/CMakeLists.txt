cmake_minimum_required(VERSION 3.22.0)
project(main)
add_executable(main main.cpp)

# Add trading system code
add_subdirectory(trading_system)

# sample_feed.txt to build folder
configure_file(${CMAKE_SOURCE_DIR}/sample_feed.txt
               ${CMAKE_BINARY_DIR}/sample_feed.txt
               COPYONLY)
               
# config.yaml to build folder
configure_file(${CMAKE_SOURCE_DIR}/config.yaml
               ${CMAKE_BINARY_DIR}/config.yaml
               COPYONLY)

# Add spdlog library for loggings
include(FetchContent)
FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG        v1.15.2  # You can specify any released tag here
)
FetchContent_MakeAvailable(spdlog)

# Add yaml-cpp library for loggings
include(FetchContent)
FetchContent_Declare(
  yaml-cpp
  GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
  GIT_TAG        0.8.0  # You can specify any released tag here
)
FetchContent_MakeAvailable(yaml-cpp)
target_link_libraries(TradingSystem PUBLIC spdlog::spdlog yaml-cpp::yaml-cpp) 
target_link_libraries(${PROJECT_NAME} PRIVATE TradingSystem) 