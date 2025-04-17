cmake_minimum_required(VERSION 3.30.0)
project(MainProject)

add_executable(${PROJECT_NAME} main.cpp)

add_executable(main_2_executable main_2.cpp)

add_executable(buggy_code_executable buggy_code.cpp)



# Compiler optimization flags
if(MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /Zi")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /DEBUG")
endif()

# Add testing library Catch2
include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.4.0
)
FetchContent_MakeAvailable(Catch2)





