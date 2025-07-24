# bench/CMakeLists.txt

add_executable(bench_all bench_all.cpp)
target_include_directories(bench_all PRIVATE ${GMP_INCLUDE_DIRS} ../include)
target_link_libraries(bench_all PRIVATE ${GMP_LIBRARIES} benchmark::benchmark)