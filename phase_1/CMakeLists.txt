cmake_minimum_required(VERSION 4.0.0)
project(MyProject)  # Sets env variable PROJECT_NAME
add_executable(${PROJECT_NAME} main.cpp)

# Creates library and link from file
add_library(SourceCode src.cpp) 
target_include_directories(SourceCode PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(${PROJECT_NAME} SourceCode)



# Add catch2 package for tests
Include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.4.0 # or a later release
)
FetchContent_MakeAvailable(Catch2)
add_executable(tests tests/test_src.cpp)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain SourceCode)