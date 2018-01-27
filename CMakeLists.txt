cmake_minimum_required(VERSION 3.9)
project(yueWebServer)

set(CMAKE_CXX_STANDARD 17)

add_executable(yueWebServer main.cpp head/lock.h head/pool.h head/task.h)

find_package(Threads REQUIRED)
target_link_libraries(yueWebServer Threads::Threads)