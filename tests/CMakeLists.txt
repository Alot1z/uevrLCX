# Context7 VR Analysis System Tests
cmake_minimum_required(VERSION 3.16)

# Enable testing
enable_testing()

# Find required packages
find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
    ${GTEST_INCLUDE_DIRS}
)

# Create test executable
add_executable(context7_tests
    test_context7_system.cpp
)

# Link against required libraries
target_link_libraries(context7_tests
    PRIVATE
        uevr_core
        uevr_vr
        uevr_adapters_common
        ${GTEST_LIBRARIES}
        ${GTEST_MAIN_LIBRARIES}
        Threads::Threads
)

# Set up compile definitions
target_compile_definitions(context7_tests
    PRIVATE
        $<$<BOOL:${uevr_ENABLE_CONTEXT7}>:UEVR_ENABLE_CONTEXT7>
        $<$<BOOL:${uevr_ENABLE_OPENVR}>:UEVR_ENABLE_OPENVR>
        $<$<BOOL:${uevr_ENABLE_OPENXR}>:UEVR_ENABLE_OPENXR>
        $<$<BOOL:${uevr_ENABLE_VULKAN}>:UEVR_ENABLE_VULKAN>
        $<$<BOOL:${uevr_ENABLE_OPENGL}>:UEVR_ENABLE_OPENGL>
        GTEST_HAS_PTHREAD=1
)

# Set C++ standard
set_target_properties(context7_tests PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

# Add test to CTest
add_test(
    NAME Context7SystemTests
    COMMAND context7_tests
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

# Set test properties
set_tests_properties(Context7SystemTests PROPERTIES
    TIMEOUT 300  # 5 minutes timeout
    ENVIRONMENT "CONTEXT7_TEST_MODE=1"
)

# Create test data directory and copy test files
set(TEST_DATA_DIR ${CMAKE_CURRENT_BINARY_DIR}/data)
file(MAKE_DIRECTORY ${TEST_DATA_DIR})

# Copy test configuration files
configure_file(
    ${CMAKE_SOURCE_DIR}/data/context7_games_database_complete.json
    ${TEST_DATA_DIR}/context7_games_database_complete.json
    COPYONLY
)

# Create minimal test config
file(WRITE ${TEST_DATA_DIR}/context7_test_config.json
"{
  \"database_path\": \"${TEST_DATA_DIR}/context7_games_database_complete.json\",
  \"ai_models_path\": \"${TEST_DATA_DIR}/models\",
  \"enable_learning\": true,
  \"cache_size_mb\": 100
}")

# Create test models directory
file(MAKE_DIRECTORY ${TEST_DATA_DIR}/models/test)

# Create minimal test pattern file
file(WRITE ${TEST_DATA_DIR}/test_patterns.json
"{
  \"engine_patterns\": {
    \"RE Engine\": {
      \"signatures\": [\"re_engine_signature\"],
      \"executables\": [\"re*.exe\"],
      \"modules\": [\"re_engine.dll\"]
    },
    \"REDengine 4\": {
      \"signatures\": [\"redengine4_signature\"],
      \"executables\": [\"Cyberpunk2077.exe\"],
      \"modules\": [\"redengine4.dll\"]
    }
  }
}")

# Performance test target
add_executable(context7_performance_tests
    test_context7_system.cpp
)

target_link_libraries(context7_performance_tests
    PRIVATE
        uevr_core
        uevr_vr
        uevr_adapters_common
        ${GTEST_LIBRARIES}
        Threads::Threads
)

target_compile_definitions(context7_performance_tests
    PRIVATE
        CONTEXT7_PERFORMANCE_TEST=1
        $<$<BOOL:${uevr_ENABLE_CONTEXT7}>:UEVR_ENABLE_CONTEXT7>
)

# Add performance test
add_test(
    NAME Context7PerformanceTests
    COMMAND context7_performance_tests --gtest_filter="*Performance*"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

set_tests_properties(Context7PerformanceTests PROPERTIES
    TIMEOUT 600  # 10 minutes for performance tests
    LABELS "performance"
)

# Memory leak detection test (if available)
find_program(VALGRIND_PROGRAM valgrind)
if(VALGRIND_PROGRAM AND NOT WIN32)
    add_test(
        NAME Context7MemoryTests
        COMMAND ${VALGRIND_PROGRAM} 
            --tool=memcheck 
            --leak-check=full 
            --error-exitcode=1
            $<TARGET_FILE:context7_tests>
    )
    
    set_tests_properties(Context7MemoryTests PROPERTIES
        TIMEOUT 1200  # 20 minutes for memory tests
        LABELS "memory"
    )
endif()

# Coverage target (if available)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    find_program(GCOV_PROGRAM gcov)
    find_program(LCOV_PROGRAM lcov)
    
    if(GCOV_PROGRAM AND LCOV_PROGRAM)
        add_custom_target(coverage
            COMMAND ${LCOV_PROGRAM} --directory . --capture --output-file coverage.info
            COMMAND ${LCOV_PROGRAM} --remove coverage.info '/usr/*' --output-file coverage.info
            COMMAND ${LCOV_PROGRAM} --list coverage.info
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Generating code coverage report"
        )
        
        add_dependencies(coverage context7_tests)
    endif()
endif()

# Benchmark target for performance testing
add_executable(context7_benchmarks
    test_context7_system.cpp
)

target_link_libraries(context7_benchmarks
    PRIVATE
        uevr_core
        uevr_vr
        uevr_adapters_common
        ${GTEST_LIBRARIES}
        Threads::Threads
)

target_compile_definitions(context7_benchmarks
    PRIVATE
        CONTEXT7_BENCHMARK_MODE=1
        $<$<BOOL:${uevr_ENABLE_CONTEXT7}>:UEVR_ENABLE_CONTEXT7>
)

# Create benchmark test
add_test(
    NAME Context7Benchmarks
    COMMAND context7_benchmarks --gtest_filter="*Stress*"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

set_tests_properties(Context7Benchmarks PROPERTIES
    TIMEOUT 1800  # 30 minutes for benchmarks
    LABELS "benchmark"
)

# Print test configuration
message(STATUS "Context7 VR Analysis System Tests configured:")
message(STATUS "  Test executable: context7_tests")
message(STATUS "  Performance tests: context7_performance_tests")
message(STATUS "  Benchmarks: context7_benchmarks")
message(STATUS "  Test data directory: ${TEST_DATA_DIR}")
if(VALGRIND_PROGRAM)
    message(STATUS "  Memory leak detection: Available")
endif()
if(GCOV_PROGRAM AND LCOV_PROGRAM)
    message(STATUS "  Code coverage: Available")
endif()