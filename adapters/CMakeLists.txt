# UEVR Cross-Engine VR Adapters CMake Configuration
# 
# Builds cross-engine VR adapter plugins for:
# - RE Engine (Resident Evil 7)
# - REDengine 4 (Cyberpunk 2077)  
# - MT Framework (Monster Hunter: World)
#
# Copyright (c) 2024 UEVR Cross-Engine Adapter Project

cmake_minimum_required(VERSION 3.16)
project(UEVR_CrossEngine_Adapters VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build options
option(BUILD_RE_ENGINE_ADAPTER "Build RE Engine adapter" ON)
option(BUILD_REDENGINE4_ADAPTER "Build REDengine 4 adapter" ON)
option(BUILD_MT_FRAMEWORK_ADAPTER "Build MT Framework adapter" ON)

# Include directories are set per-target below using target_include_directories

# Dependency setup (with fallbacks via FetchContent)
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

# spdlog (try system, else fetch)
find_package(spdlog QUIET)
if(NOT spdlog_FOUND)
    FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.12.0
    )
    FetchContent_MakeAvailable(spdlog)
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

# Note: common sources are header-only for now; no shared .cpp compiled

# RE Engine Adapter
if(BUILD_RE_ENGINE_ADAPTER)
    add_library(re_engine_adapter SHARED
        RE-Engine/re7_adapter.cpp
        RE-Engine/RE_Engine_Hooks.cpp
        RE-Engine/engine_hooks.cpp
        ${COMMON_SOURCES}
    )
    target_include_directories(re_engine_adapter PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/RE-Engine
    )
    
    target_link_libraries(re_engine_adapter
        MinHook::MinHook
        spdlog::spdlog
        nlohmann_json::nlohmann_json
        d3d11
        dxgi
        d3dcompiler
    )
    
    target_compile_definitions(re_engine_adapter PRIVATE
        RE_ENGINE_ADAPTER_EXPORTS
        _CRT_SECURE_NO_WARNINGS
        UNICODE
        _UNICODE
        WIN32_LEAN_AND_MEAN
        NOMINMAX
        _WIN32_WINNT=0x0601
    )
    
    set_target_properties(re_engine_adapter PROPERTIES
        OUTPUT_NAME "re_engine_adapter"
        PREFIX ""
    )
endif()

# REDengine 4 Adapter
if(BUILD_REDENGINE4_ADAPTER)
    add_library(redengine4_adapter SHARED
        REDengine4/redengine_adapter.cpp
        ${COMMON_SOURCES}
    )
    target_include_directories(redengine4_adapter PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/REDengine4
    )
    
    target_link_libraries(redengine4_adapter
        MinHook::MinHook
        spdlog::spdlog
        d3d12
        dxgi
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
    
    set_target_properties(redengine4_adapter PROPERTIES
        OUTPUT_NAME "redengine4_adapter"
        PREFIX ""
    )
endif()

# MT Framework Adapter
if(BUILD_MT_FRAMEWORK_ADAPTER)
    add_library(mt_framework_adapter SHARED
        MT-Framework/mt_framework_bridge.cpp
        ${COMMON_SOURCES}
    )
    target_include_directories(mt_framework_adapter PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/MT-Framework
    )
    
    target_link_libraries(mt_framework_adapter
        MinHook::MinHook
        spdlog::spdlog
        nlohmann_json::nlohmann_json
        d3d11
        dxgi
    )
    
    target_compile_definitions(mt_framework_adapter PRIVATE
        MT_FRAMEWORK_ADAPTER_EXPORTS
        _CRT_SECURE_NO_WARNINGS
        UNICODE
        _UNICODE
        WIN32_LEAN_AND_MEAN
        NOMINMAX
        _WIN32_WINNT=0x0601
    )
    
    set_target_properties(mt_framework_adapter PROPERTIES
        OUTPUT_NAME "mt_framework_adapter"
        PREFIX ""
    )
endif()

# Installation
if(TARGET re_engine_adapter)
    install(TARGETS re_engine_adapter DESTINATION lib/uevr/adapters)
endif()
if(TARGET redengine4_adapter)
    install(TARGETS redengine4_adapter DESTINATION lib/uevr/adapters)
endif()
if(TARGET mt_framework_adapter)
    install(TARGETS mt_framework_adapter DESTINATION lib/uevr/adapters)
endif()

# Copy adapter files to profiles
install(FILES
    RE-Engine/engine_hooks.cpp
    REDengine4/redengine_adapter.cpp
    MT-Framework/mt_framework_bridge.cpp
    DESTINATION profiles/adapters
)
