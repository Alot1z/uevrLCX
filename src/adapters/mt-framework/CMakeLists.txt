cmake_minimum_required(VERSION 3.16)
project(MTFrameworkCrossEngineAdapter)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Dependencies (match RE-Engine pattern)
find_package(MinHook QUIET)
if(NOT MinHook_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        minhook
        GIT_REPOSITORY https://github.com/TsudaKageyu/minhook.git
        GIT_TAG master
    )
    FetchContent_MakeAvailable(minhook)
    if(NOT TARGET MinHook::MinHook)
        add_library(MinHook::MinHook ALIAS minhook)
    endif()
endif()

find_package(spdlog QUIET)
if(NOT spdlog_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.12.0
    )
    FetchContent_MakeAvailable(spdlog)
    if(NOT TARGET spdlog::spdlog)
        add_library(spdlog::spdlog ALIAS spdlog)
    endif()
endif()

# JSON dependency for adapter profile parsing
include(FetchContent)
FetchContent_Declare(nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json
    GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

# Include directories are set per-target below
# Source files (use active bridge implementation)
set(SOURCES
    mt_framework_bridge.cpp
    mt_framework_factory.cpp
    mt_framework_main.cpp
    test_integration.cpp
)

# Header files
set(HEADERS
    mt_framework_adapter.hpp
    mt_framework_factory.hpp
)

# Create shared library
add_library(MTFrameworkCrossEngineAdapter SHARED ${SOURCES} ${HEADERS})

target_include_directories(MTFrameworkCrossEngineAdapter PRIVATE
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Set output name
set_target_properties(MTFrameworkCrossEngineAdapter PROPERTIES
    OUTPUT_NAME "mt_framework_cross_engine_adapter"
    PREFIX ""
    SUFFIX ".dll"
)

# Link libraries
target_link_libraries(MTFrameworkCrossEngineAdapter
    d3d11
    dxgi
    MinHook::MinHook
    spdlog::spdlog
    nlohmann_json::nlohmann_json
)

# Compiler flags - Warnings as errors for Release builds
if(MSVC)
    target_compile_options(MTFrameworkCrossEngineAdapter PRIVATE
        /W4
        /MP
        /EHsc
    )
    
    # Warnings as errors for Release builds (support multi-config generators)
    target_compile_options(MTFrameworkCrossEngineAdapter PRIVATE
        $<$<CONFIG:Release>:/WX>
    )
    
    # Set DLL export/import
    target_compile_definitions(MTFrameworkCrossEngineAdapter PRIVATE
        _CRT_SECURE_NO_WARNINGS
        WIN32_LEAN_AND_MEAN
        NOMINMAX
    )
else()
    target_compile_options(MTFrameworkCrossEngineAdapter PRIVATE
        -Wall
        -Wextra
        -fPIC
    )
    
    # Warnings as errors for Release builds
    target_compile_options(MTFrameworkCrossEngineAdapter PRIVATE
        $<$<CONFIG:Release>:-Werror>
    )
endif()

# Installation
install(TARGETS MTFrameworkCrossEngineAdapter
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(FILES ${HEADERS}
    DESTINATION include/uevr/mt_framework
)

# Copy DLL to game directory (optional)
if(WIN32)
    add_custom_command(TARGET MTFrameworkCrossEngineAdapter POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:MTFrameworkCrossEngineAdapter>
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../profiles/MonsterHunterWorld/
        COMMENT "Copying MT Framework adapter to Monster Hunter World profile directory"
    )
endif()

# Print configuration info
message(STATUS "MT Framework Cross-Engine Adapter Configuration:")
message(STATUS "  Target: ${CMAKE_TARGET_NAME}")
message(STATUS "  Output: $<TARGET_FILE:MTFrameworkCrossEngineAdapter>")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
