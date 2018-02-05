cmake_minimum_required(VERSION 3.5)

project (myLib)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ".")

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "-g")

include_directories(bloom-filter)
include_directories(hash)
include_directories(thread-pool)
include_directories(util)
include_directories(vqs)

find_package(Threads)

set(SOURCE_FILES
    bloom-filter/bit_vector.cpp
    bloom-filter/bloom_filter.cpp
    thread-pool/thread.cpp
    thread-pool/thread_pool.cpp
    thread-pool/work_queue.cpp
    thread-pool/worker.cpp
    )

add_executable(myLib ${SOURCE_FILES})
target_link_libraries(myLib ${CMAKE_THREAD_LIBS_INIT})
