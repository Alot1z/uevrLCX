# Tests for uevrLCX

# Enable testing
enable_testing()

# Find Google Test
find_package(GTest QUIET)
if(NOT GTEST_FOUND)
    message(WARNING "Google Test not found - tests will be disabled")
    return()
endif()

# Unit tests
add_executable(unit_tests
    unit/test_core.cpp
    unit/test_vr.cpp
    unit/test_hooks.cpp
    unit/test_mods.cpp
    unit/test_adapters.cpp
    unit/test_context7.cpp
    unit/test_deepwiki.cpp
)

target_link_libraries(unit_tests
    uevr_core
    uevr_vr
    uevr_hooks
    uevr_mods
    uevr_adapters
    uevr_context7
    uevr_deepwiki
    GTest::gtest
    GTest::gtest_main
)

# Integration tests
add_executable(integration_tests
    integration/test_integration.cpp
    integration/test_vr_integration.cpp
    integration/test_engine_integration.cpp
    integration/test_performance_integration.cpp
)

target_link_libraries(integration_tests
    uevr_core
    uevr_vr
    uevr_hooks
    uevr_mods
    uevr_adapters
    uevr_context7
    uevr_deepwiki
    GTest::gtest
    GTest::gtest_main
)

# Performance tests
add_executable(performance_tests
    performance/test_performance.cpp
    performance/test_vr_performance.cpp
    performance/test_rendering_performance.cpp
    performance/test_physics_performance.cpp
)

target_link_libraries(performance_tests
    uevr_core
    uevr_vr
    uevr_hooks
    uevr_mods
    uevr_adapters
    uevr_context7
    uevr_deepwiki
    GTest::gtest
    GTest::gtest_main
)

# Add tests to CTest
add_test(NAME UnitTests COMMAND unit_tests)
add_test(NAME IntegrationTests COMMAND integration_tests)
add_test(NAME PerformanceTests COMMAND performance_tests)

# Set test properties
set_tests_properties(UnitTests PROPERTIES
    TIMEOUT 300
    LABELS "unit"
)

set_tests_properties(IntegrationTests PROPERTIES
    TIMEOUT 600
    LABELS "integration"
)

set_tests_properties(PerformanceTests PROPERTIES
    TIMEOUT 900
    LABELS "performance"
)