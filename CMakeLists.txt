# uevr - Unreal Engine VR Modding Framework
# 
# Main CMake configuration for the entire uevr project
# 
# Copyright (c) 2024 uevr Project

cmake_minimum_required(VERSION 3.16)
project(uevr VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard and policies
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Modern CMake policies
cmake_policy(SET CMP0071 NEW)  # Use absolute paths in target_sources
cmake_policy(SET CMP0072 NEW)  # Use absolute paths in target_include_directories
cmake_policy(SET CMP0077 NEW)  # Use absolute paths in target_sources

# Project options
option(BUILD_TESTS "Build test suite" ON)
option(BUILD_EXAMPLES "Build example implementations" ON)
option(BUILD_DOCS "Build documentation" OFF)
option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(uevr_ENABLE_DEBUG "Enable debug features" OFF)
option(uevr_ENABLE_PROFILING "Enable performance profiling" OFF)

# Set output directories with proper organization
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Platform-specific settings
if(WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /utf-8")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zi /Od")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /DNDEBUG")
elseif(UNIX AND NOT APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")
elseif(APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")
endif()

# Dependency setup with proper error handling
include(FetchContent)

# MinHook - API hooking library
find_package(MinHook QUIET)
if(NOT MinHook_FOUND)
    message(STATUS "MinHook not found, fetching from source...")
    FetchContent_Declare(
        minhook
        GIT_REPOSITORY https://github.com/TsudaKageyu/minhook.git
        GIT_TAG master
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(minhook)
    if(NOT TARGET MinHook::MinHook)
        add_library(MinHook::MinHook ALIAS minhook)
    endif()
    message(STATUS "MinHook fetched successfully")
else()
    message(STATUS "MinHook found: ${MinHook_DIR}")
endif()

# spdlog - Fast logging library
find_package(spdlog QUIET)
if(NOT spdlog_FOUND)
    message(STATUS "spdlog not found, fetching from source...")
    FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.12.0
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(spdlog)
    message(STATUS "spdlog fetched successfully")
else()
    message(STATUS "spdlog found: ${spdlog_DIR}")
endif()

# nlohmann_json - JSON library
find_package(nlohmann_json QUIET)
if(NOT nlohmann_json_FOUND)
    message(STATUS "nlohmann_json not found, fetching from source...")
    FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json
        GIT_TAG v3.11.3
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(nlohmann_json)
    message(STATUS "nlohmann_json fetched successfully")
else()
    message(STATUS "nlohmann_json found: ${nlohmann_json_DIR}")
endif()

# Google Test for testing (if enabled)
if(BUILD_TESTS)
    find_package(GTest QUIET)
    if(NOT GTest_FOUND)
        message(STATUS "Google Test not found, fetching from source...")
        FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG release-1.12.1
            GIT_SHALLOW TRUE
        )
        FetchContent_MakeAvailable(googletest)
        message(STATUS "Google Test fetched successfully")
    else()
        message(STATUS "Google Test found: ${GTest_DIR}")
    endif()
endif()

# Add subdirectories in dependency order
message(STATUS "Configuring uevr components...")

# Core framework (no dependencies)
add_subdirectory(src/core)

# Adapters (depends on core)
add_subdirectory(src/adapters)

# Hooks (depends on core and adapters)
add_subdirectory(src/hooks)

# Mods (depends on core, adapters, and hooks)
add_subdirectory(src/mods)

# Tests (depends on all components)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
    message(STATUS "Testing enabled")
endif()

# Examples (depends on all components)
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
    message(STATUS "Examples enabled")
endif()

# Documentation
if(BUILD_DOCS)
    add_subdirectory(docs)
    message(STATUS "Documentation enabled")
endif()

# Main uevr library
message(STATUS "Configuring main uevr library...")
add_library(uevr SHARED
    src/main.cpp
    src/Plugin.cpp
)

# Set library properties
set_target_properties(uevr PROPERTIES
    OUTPUT_NAME "uevr"
    PREFIX ""
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    DEBUG_POSTFIX "_d"
    RELEASE_POSTFIX ""
    RELWITHDEBINFO_POSTFIX "_rd"
    MINSIZEREL_POSTFIX "_ms"
)

# Link dependencies
target_link_libraries(uevr
    PRIVATE
        uevr_core
        uevr_adapters_common
        spdlog::spdlog
        nlohmann_json::nlohmann_json
        MinHook::MinHook
)

# Include directories
target_include_directories(uevr
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Compile definitions
target_compile_definitions(uevr
    PRIVATE
        $<$<BOOL:${uevr_ENABLE_DEBUG}>:uevr_DEBUG>
        $<$<BOOL:${uevr_ENABLE_PROFILING}>:uevr_PROFILING>
        uevr_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
        uevr_VERSION_MINOR=${PROJECT_VERSION_MINOR}
        uevr_VERSION_PATCH=${PROJECT_VERSION_PATCH}
)

# Installation configuration
include(GNUInstallDirs)

install(TARGETS uevr
    EXPORT uevrTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)

# Export targets
install(EXPORT uevrTargets
    FILE uevrTargets.cmake
    NAMESPACE uevr::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/uevr
)

# Generate and install package configuration
include(CMakePackageConfigHelpers)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/uevrConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/uevrConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/uevr
    PATH_VARS
        CMAKE_INSTALL_INCLUDEDIR
        CMAKE_INSTALL_LIBDIR
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/uevrConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/uevrConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/uevrConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/uevr
)

# Print configuration summary
message(STATUS "")
message(STATUS "=== uevr Configuration Summary ===")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Tests: ${BUILD_TESTS}")
message(STATUS "Examples: ${BUILD_EXAMPLES}")
message(STATUS "Documentation: ${BUILD_DOCS}")
message(STATUS "Debug features: ${uevr_ENABLE_DEBUG}")
message(STATUS "Profiling: ${uevr_ENABLE_PROFILING}")
message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "==================================")
message(STATUS "")
