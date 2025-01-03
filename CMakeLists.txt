cmake_minimum_required(VERSION 3.10...3.28)
project(ThreadPool)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB_RECURSE CPP_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.cpp")

add_library(threadpool SHARED ${CPP_FILES})

set_target_properties(threadpool PROPERTIES OUTPUT_NAME "threadpool")

target_include_directories(threadpool PUBLIC include)

install(TARGETS threadpool
    LIBRARY DESTINATION lib     # For shared libraries
    ARCHIVE DESTINATION lib     # For static libraries (if added in the future)
    RUNTIME DESTINATION bin     # For executables (if added in the future)
)

install(DIRECTORY include/ DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)
