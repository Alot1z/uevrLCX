# uevr Core Source CMake Configuration
# 
# Builds the core uevr functionality including:
# - Framework initialization
# - Exception handling
# - Core VR system
#
# Copyright (c) 2024 uevr Project

cmake_minimum_required(VERSION 3.16)
project(uevr_Core VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Core source files
set(CORE_SOURCES
    Framework.cpp
    ExceptionHandler.cpp
    utility/Logger.cpp
    utility/ConfigManager.cpp
)

# Create core library
add_library(uevr_core STATIC ${CORE_SOURCES})

# Include directories
target_include_directories(uevr_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Link dependencies
target_link_libraries(uevr_core
    spdlog::spdlog
    nlohmann_json::nlohmann_json
)

# Compile definitions
target_compile_definitions(uevr_core PRIVATE
    uevr_CORE_EXPORTS
    _CRT_SECURE_NO_WARNINGS
    UNICODE
    _UNICODE
    WIN32_LEAN_AND_MEAN
    NOMINMAX
    _WIN32_WINNT=0x0601
)

# Set output properties
set_target_properties(uevr_core PROPERTIES
    OUTPUT_NAME "uevr_core"
    PREFIX ""
)
