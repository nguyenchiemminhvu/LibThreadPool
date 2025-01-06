cmake_minimum_required(VERSION 3.10...3.28)
project(TestThreadPool)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(test test_threadpool.cpp)

target_link_libraries(test
    PRIVATE 
        threadpool
        pthread
)
