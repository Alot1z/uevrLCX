# uevr Tests CMake Configuration
# 
# Builds the test suite for uevr including:
# - Unit tests
# - Integration tests
# - Performance tests
#
# Copyright (c) 2024 uevr Project

# Enable testing
enable_testing()

# Find Google Test
find_package(GTest QUIET)
if(NOT GTest_FOUND)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
    )
    FetchContent_MakeAvailable(googletest)
endif()

# Test sources
set(TEST_SOURCES
    unit/core_tests.cpp
    unit/adapter_tests.cpp
    unit/hook_tests.cpp
    integration/mt_framework_tests.cpp
    integration/re_engine_tests.cpp
    integration/redengine4_tests.cpp
)

# Create test executable
add_executable(uevr_tests ${TEST_SOURCES})

# Include directories
target_include_directories(uevr_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/uevr
    ${CMAKE_SOURCE_DIR}/src
)

# Link dependencies
target_link_libraries(uevr_tests
    uevr_core
    uevr_adapters_common
    GTest::gtest
    GTest::gtest_main
    spdlog::spdlog
    nlohmann_json::nlohmann_json
)

# Add tests
add_test(NAME uevr_Unit_Tests COMMAND uevr_tests --gtest_filter="Unit*")
add_test(NAME uevr_Integration_Tests COMMAND uevr_tests --gtest_filter="Integration*")

# Set output properties
set_target_properties(uevr_tests PROPERTIES
    OUTPUT_NAME "uevr_tests"
)
