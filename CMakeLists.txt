cmake_minimum_required(VERSION 3.10)
project(gmp_example)

set(CMAKE_CXX_STANDARD 17)

enable_testing()
include(cmake/googletest.cmake)
include(cmake/googlebench.cmake)
include(GoogleTest)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
find_package(GMP REQUIRED)

add_executable(bigint_tests test/test_bigint.cpp)
target_include_directories(bigint_tests PRIVATE include ${GMP_INCLUDE_DIRS})
target_link_libraries(bigint_tests PRIVATE ${GMP_LIBRARIES} GTest::gtest_main)

add_executable(bigint_bench bench/bench_all.cpp)
target_include_directories(bigint_bench PRIVATE include ${GMP_INCLUDE_DIRS})
target_link_libraries(bigint_bench PRIVATE ${GMP_LIBRARIES} benchmark::benchmark)

include(GoogleTest)
gtest_discover_tests(bigint_tests)