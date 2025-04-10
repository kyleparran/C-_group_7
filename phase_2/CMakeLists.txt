cmake_minimum_required(VERSION 3.22.0)

# Client
project(HFTClient)
add_executable(HFTClient hft_client.cpp)

# Server
add_executable(HFTServer hft_server.cpp)


# # Add src code
# add_library(SourceCode src.cpp)
# target_include_directories(SourceCode PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
# target_link_libraries(${PROJECT_NAME} SourceCode)

# # Add benchmarking code
# add_library(Benchmarking benchmarking.cpp)
# target_include_directories(Benchmarking PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
# target_link_libraries(${PROJECT_NAME} Benchmarking)


# # Add testing library Catch2
# include(FetchContent)
# FetchContent_Declare(
#   Catch2
#   GIT_REPOSITORY https://github.com/catchorg/Catch2.git
#   GIT_TAG        v3.4.0
# )
# FetchContent_MakeAvailable(Catch2)

# include(CTest)
# enable_testing()

# add_executable(tests tests/test_src.cpp)
# target_link_libraries(tests PRIVATE Catch2::Catch2WithMain SourceCode)

# # No longer do add_test(NAME MyTests COMMAND tests), instead:
# include(Catch)
# catch_discover_tests(tests)
