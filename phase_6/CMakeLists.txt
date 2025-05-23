cmake_minimum_required(VERSION 3.20)
project(HFTClient LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

# Download and extract the boost library from GitHub
message(STATUS "Downloading and extracting boost library sources. This will take some time...")
set(BOOST_INCLUDE_LIBRARIES asio)
# set(BOOST_INCLUDE_LIBRARIES thread filesystem system)
set(BOOST_ENABLE_CMAKE ON)
Set(FETCHCONTENT_QUIET FALSE) # Print download progress
FetchContent_Declare(
    Boost
    URL https://github.com/boostorg/boost/releases/download/boost-1.84.0/boost-1.84.0.7z
    # URL https://github.com/boostorg/boost/releases/download/boost-1.88.0/boost-1.88.0-cmake.7z
    USES_TERMINAL_DOWNLOAD TRUE 
    GIT_PROGRESS TRUE   
    DOWNLOAD_NO_EXTRACT FALSE
)
FetchContent_MakeAvailable(Boost)


# Client
add_executable(Client client.cpp)

# Server
add_executable(Server main_server.cpp)
target_link_libraries(Server PUBLIC Boost::asio)