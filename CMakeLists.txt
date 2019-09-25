cmake_minimum_required(VERSION 3.10)
project(bombarding)

set(CMAKE_CXX_STANDARD 17)

find_package(Threads REQUIRED)
link_libraries(${CMAKE_THREAD_LIBS_INIT})

add_subdirectory(yaml-cpp)
add_executable(blomb src/main.cpp)
target_link_libraries(blomb PRIVATE yaml-cpp)