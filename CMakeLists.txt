cmake_minimum_required(VERSION 3.10)
project(MemoryPool)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 头文件路径
include_directories(include)

# 公共源码（不包含 main）
set(MEMPOOL_SRC
    src/CentralCache.cpp
    src/PageCache.cpp
    src/ThreadCache.cpp
)

# 功能测试可执行文件：unit_test
add_executable(unit_test
    ${MEMPOOL_SRC}
    tests/UnitTest.cpp
)
target_link_libraries(unit_test pthread)

# 性能测试可执行文件：perf_test
add_executable(perf_test
    ${MEMPOOL_SRC}
    tests/PerformanceTest.cpp
)
target_link_libraries(perf_test pthread)
