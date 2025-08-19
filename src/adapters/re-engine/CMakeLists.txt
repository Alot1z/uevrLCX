# RE Engine Cross-Engine VR Adapter CMakeLists.txt
# Builds the RE Engine adapter DLL for uevr

cmake_minimum_required(VERSION 3.16)
project(RE_Engine_Adapter)

# Set C++ standard (API.hpp uses consteval -> requires C++20)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Dependencies
# MinHook: prefer system package, fallback to FetchContent if not found
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

# spdlog: prefer system package, fallback to FetchContent if not found
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

# Source files (keep only active, non-duplicative sources)
set(RE_ENGINE_ADAPTER_SOURCES
    re7_adapter.cpp
    RE_Engine_Hooks.cpp
    engine_hooks.cpp
    RE_Engine_Integration.cpp
    RE_Engine_CrossEngine_Bridge.cpp
    RE_Engine_Main_Integration.cpp
    RE_Engine_Final_Integration.cpp
)

# Header files (for IDE visibility only)
set(RE_ENGINE_ADAPTER_HEADERS
    RE_Engine_Hooks.hpp
    engine_hooks.hpp
    RE_Engine_Integration.hpp
    RE_Engine_CrossEngine_Bridge.hpp
    RE_Engine_Main_Integration.hpp
    RE_Engine_Final_Integration.hpp
)

# Create shared library (DLL)
add_library(RE_Engine_Adapter SHARED
    ${RE_ENGINE_ADAPTER_SOURCES}
    ${RE_ENGINE_ADAPTER_HEADERS}
)

target_include_directories(RE_Engine_Adapter PRIVATE
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Set target properties
set_target_properties(RE_Engine_Adapter PROPERTIES
    OUTPUT_NAME "RE_Engine_Adapter"
    PREFIX ""
    SUFFIX ".dll"
    VERSION 1.0.0
    SOVERSION 1
)

# Link libraries
target_link_libraries(RE_Engine_Adapter
    d3d11
    dxgi
    d3dcompiler
    MinHook::MinHook
    spdlog::spdlog
    nlohmann_json::nlohmann_json
)

# Compiler definitions
target_compile_definitions(RE_Engine_Adapter PRIVATE
    UNICODE
    _UNICODE
    WIN32_LEAN_AND_MEAN
    NOMINMAX
    _WIN32_WINNT=0x0601
    RE_ENGINE_INTEGRATION_ENABLED
    CROSS_ENGINE_SUPPORT_ENABLED
    MAIN_FRAMEWORK_INTEGRATION_ENABLED
)

# Compiler flags
if(MSVC)
    target_compile_options(RE_Engine_Adapter PRIVATE
        /W4
        /WX
        /MP
        /EHsc
        /utf-8
    )
    
    # Fix spdlog compilation issues
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
endif()

# Installation
install(TARGETS RE_Engine_Adapter
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# Copy dependencies
install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/README.md
    DESTINATION share/RE_Engine_Adapter
)

# Post-build commands
add_custom_command(TARGET RE_Engine_Adapter POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:RE_Engine_Adapter>
        ${CMAKE_BINARY_DIR}/bin/RE_Engine_Adapter.dll
    COMMENT "Copying RE_Engine_Adapter.dll to output directory"
)

# Development targets
add_custom_target(RE_Engine_Adapter_clean
    COMMAND ${CMAKE_COMMAND} -E remove_directory
        ${CMAKE_BINARY_DIR}/bin
    COMMAND ${CMAKE_COMMAND} -E remove_directory
        ${CMAKE_BINARY_DIR}/lib
    COMMENT "Cleaning RE_Engine_Adapter build artifacts"
)

# Documentation target
add_custom_target(RE_Engine_Adapter_docs
    COMMAND ${CMAKE_COMMAND} -E echo "RE Engine Adapter Documentation:"
    COMMAND ${CMAKE_COMMAND} -E echo "  - README.md: ${CMAKE_CURRENT_SOURCE_DIR}/README.md"
    COMMAND ${CMAKE_COMMAND} -E echo "  - Source: ${CMAKE_CURRENT_SOURCE_DIR}"
    COMMAND ${CMAKE_COMMAND} -E echo "  - Output: ${CMAKE_BINARY_DIR}/bin/RE_Engine_Adapter.dll"
    COMMENT "Displaying RE_Engine_Adapter documentation information"
)

# Testing target (placeholder)
add_custom_target(RE_Engine_Adapter_test
    COMMAND ${CMAKE_COMMAND} -E echo "RE Engine Adapter testing not yet implemented"
    COMMENT "RE_Engine_Adapter testing placeholder"
)


