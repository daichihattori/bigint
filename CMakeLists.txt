cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)

option(BIGINT_ENABLE_TESTING "Enable tests for bigint" ON)
option(BIGINT_ENABLE_BENCHMARKS "Enable benchmarks for bigint" ON)
set(BIGINT_VERSION "0.0.0" CACHE STRING "Version string for bigint")

project(bigint VERSION ${BIGINT_VERSION} LANGUAGES CXX)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
find_package(GMP REQUIRED)

add_library(bigint INTERFACE)
target_include_directories(bigint INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(bigint INTERFACE ${GMP_LIBRARIES})

if(BIGINT_ENABLE_TESTING)
    include(cmake/AddGtest.cmake)
    add_executable(bigint_tests test/test_bigint.cpp)
    target_include_directories(bigint_tests PRIVATE include ${GMP_INCLUDE_DIRS})
    target_link_libraries(bigint_tests PRIVATE ${GMP_LIBRARIES} GTest::gtest_main)
    gtest_discover_tests(bigint_tests)
endif()

if(BIGINT_ENABLE_BENCHMARKS)
    include(cmake/AddGBench.cmake)
    add_executable(bigint_bench bench/bench_all.cpp)
    target_include_directories(bigint_bench PRIVATE include ${GMP_INCLUDE_DIRS})
    target_link_libraries(bigint_bench PRIVATE ${GMP_LIBRARIES} benchmark::benchmark)
endif()


include(CMakePackageConfigHelpers)
install(DIRECTORY include/ DESTINATION include)
install(TARGETS bigint EXPORT bigintTargets)
install(EXPORT bigintTargets
  FILE bigintTargets.cmake
  NAMESPACE bigint::
  DESTINATION lib/cmake/bigint
)

file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/bigintConfig.cmake"
"include(\"\${CMAKE_CURRENT_LIST_DIR}/bigintTargets.cmake\")\n"
)

install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/bigintConfig.cmake"
  DESTINATION lib/cmake/bigint
)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/bigintConfigVersion.cmake"
  VERSION 0.1
  COMPATIBILITY SameMajorVersion
)

install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/bigintConfigVersion.cmake"
  DESTINATION lib/cmake/bigint
)

export(PACKAGE bigint)