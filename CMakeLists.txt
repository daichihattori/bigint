cmake_minimum_required(VERSION 3.10)
project(gmp_example)

set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
find_package(GMP REQUIRED)

add_executable(bigint bigint.cpp)

target_include_directories(bigint PRIVATE ${GMP_INCLUDE_DIRS})
target_link_libraries(bigint PRIVATE ${GMP_LIBRARIES})