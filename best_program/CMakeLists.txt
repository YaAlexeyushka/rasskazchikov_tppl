cmake_minimum_required(VERSION 3.10)
project(data_collector)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 --coverage")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0 --coverage")

find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

include_directories(${GTEST_INCLUDE_DIRS})

add_library(data_collector_lib STATIC data_collector.cpp)

add_executable(data_collector main.cpp)
target_link_libraries(data_collector data_collector_lib pthread gcov)

add_executable(test_data_collector test_data_collector.cpp)
target_link_libraries(test_data_collector 
    data_collector_lib 
    ${GTEST_LIBRARIES} 
    ${GTEST_MAIN_LIBRARIES} 
    pthread 
    gcov
)

enable_testing()
add_test(NAME test_data_collector COMMAND test_data_collector)
