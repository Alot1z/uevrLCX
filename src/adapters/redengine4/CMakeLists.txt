# REDengine 4 Cross-Engine VR Adapter CMake Configuration
# 
# Builds REDengine 4 VR adapter plugin for Cyberpunk 2077
# Extends uevr's plugin architecture for REDengine 4 compatibility
#
# Copyright (c) 2024 uevr Cross-Engine Adapter Project

cmake_minimum_required(VERSION 3.16)
project(REDengine4_Adapter VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler warnings and error handling
if(MSVC)
    # MSVC specific flags
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /WX /W4")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /W4")
    # Disable specific warnings that are not critical
    add_compile_options(/wd4251 /wd4275 /wd4996)
else()
    # GCC/Clang specific flags
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Werror -Wall -Wextra")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra")
endif()

# Include directories are set per-target below

# Dependency setup
include(FetchContent)

# MinHook (try system, else fetch)
find_package(MinHook QUIET)
if(NOT MinHook_FOUND)
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

# nlohmann_json (try system, else fetch)
find_package(nlohmann_json QUIET)
if(NOT nlohmann_json_FOUND)
    FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json
        GIT_TAG v3.11.3
    )
    FetchContent_MakeAvailable(nlohmann_json)
endif()

# Build REDengine 4 Adapter
add_library(redengine4_adapter SHARED
    redengine_adapter.cpp
    redengine_adapter.hpp
    uobject_hook.cpp
    uobject_hook.hpp
)

target_include_directories(redengine4_adapter PRIVATE
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(redengine4_adapter
    MinHook::MinHook
    nlohmann_json::nlohmann_json
    d3d12
    dxgi
    dxguid
)

target_compile_definitions(redengine4_adapter PRIVATE
    REDENGINE4_ADAPTER_EXPORTS
    _CRT_SECURE_NO_WARNINGS
    UNICODE
    _UNICODE
    WIN32_LEAN_AND_MEAN
    NOMINMAX
    _WIN32_WINNT=0x0A00
)

# Additional compiler-specific optimizations
if(MSVC)
    target_compile_options(redengine4_adapter PRIVATE
        $<$<CONFIG:Release>:/O2 /GL /Gy>
        $<$<CONFIG:Debug>:/Od /RTC1>
    )
else()
    target_compile_options(redengine4_adapter PRIVATE
        $<$<CONFIG:Release>:-O3 -DNDEBUG>
        $<$<CONFIG:Debug>:-O0 -g>
    )
endif()

set_target_properties(redengine4_adapter PROPERTIES
    OUTPUT_NAME "redengine4_adapter"
    PREFIX ""
)

# Installation
install(TARGETS redengine4_adapter DESTINATION lib/uevr/adapters)

# Copy adapter files to profiles
install(FILES
    redengine_adapter.cpp
    redengine_adapter.hpp
    uobject_hook.cpp
    uobject_hook.hpp
    DESTINATION profiles/adapters/redengine4
)
