# uevr Cross-Engine VR Adapters CMakeLists.txt
# Builds all cross-engine VR adapters for uevr

cmake_minimum_required(VERSION 3.16)
project(uevr_CrossEngine_Adapters VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard (API.hpp uses C++20 features like consteval)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Force MSVC to use C++20
if(MSVC)
    add_compile_options(/std:c++20 /Zc:__cplusplus)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++20 /Zc:__cplusplus")
endif()

# Build options
option(BUILD_RE_ENGINE_ADAPTER "Build RE Engine adapter" ON)
option(BUILD_REDENGINE4_ADAPTER "Build REDengine 4 adapter" ON)
option(BUILD_MT_FRAMEWORK_ADAPTER "Build MT Framework adapter" ON)
option(BUILD_TESTS "Build tests" OFF)
option(BUILD_EXAMPLES "Build examples" OFF)

# Compiler warnings and error handling
if(MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /W4 /WX /EHsc")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /W4 /EHsc")
    add_compile_options(/wd4251 /wd4275 /wd4996 /wd4100 /wd4189 /wd4244)
else()
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Werror -Wall -Wextra")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra")
endif()

# Include directories are set per-target below using generator expressions

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

# Common adapter library
add_library(uevr_adapters_common STATIC
    common/CrossEngineAdapter.cpp
    common/UniversalEngineRegistry.cpp
    common/BaseCrossEngineAdapter.cpp
    common/UniversalEngineDetector.cpp
    common/UObjectHookSystem.cpp
    common/CrossEngineAdapterFactory.cpp
)

target_link_libraries(uevr_adapters_common
    PUBLIC
        uevr_core
        uevr_vr
        MinHook::MinHook
        nlohmann_json::nlohmann_json
)

target_include_directories(uevr_adapters_common PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_definitions(uevr_adapters_common PRIVATE
    _CRT_SECURE_NO_WARNINGS
    UNICODE
    _UNICODE
    WIN32_LEAN_AND_MEAN
    NOMINMAX
    _WIN32_WINNT=0x0A00
)

# RE Engine Adapter
if(BUILD_RE_ENGINE_ADAPTER)
    add_library(uevr_re_engine_adapter SHARED
        re-engine/REEngineAdapter.cpp
        re-engine/REEngineAdapter.hpp
    )
    
    target_link_libraries(uevr_re_engine_adapter
        uevr_adapters_common
        d3d11
        dxgi
        d3dcompiler
    )
    
    target_include_directories(uevr_re_engine_adapter PRIVATE
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    )
    
    target_compile_definitions(uevr_re_engine_adapter PRIVATE
        _CRT_SECURE_NO_WARNINGS
        UNICODE
        _UNICODE
        WIN32_LEAN_AND_MEAN
        NOMINMAX
        _WIN32_WINNT=0x0A00
        RE_ENGINE_ADAPTER_EXPORTS
    )
    
    set_target_properties(uevr_re_engine_adapter PROPERTIES
        OUTPUT_NAME "uevr_re_engine_adapter"
        PREFIX ""
        SUFFIX ".dll"
    )
endif()

# REDengine 4 Adapter
if(BUILD_REDENGINE4_ADAPTER)
    add_library(uevr_redengine4_adapter SHARED
        redengine4/redengine_adapter.cpp
        redengine4/redengine_adapter.hpp
        redengine4/uobject_hook.cpp
        redengine4/uobject_hook.hpp
    )
    
    target_link_libraries(uevr_redengine4_adapter
        uevr_adapters_common
        MinHook::MinHook
        nlohmann_json::nlohmann_json
        d3d12
        dxgi
        dxguid
    )
    
    target_include_directories(uevr_redengine4_adapter PRIVATE
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    )
    
    target_compile_definitions(uevr_redengine4_adapter PRIVATE
        _CRT_SECURE_NO_WARNINGS
        UNICODE
        _UNICODE
        WIN32_LEAN_AND_MEAN
        NOMINMAX
        _WIN32_WINNT=0x0A00
        REDENGINE4_ADAPTER_EXPORTS
    )
    
    set_target_properties(uevr_redengine4_adapter PROPERTIES
        OUTPUT_NAME "uevr_redengine4_adapter"
        PREFIX ""
        SUFFIX ".dll"
    )
endif()

# MT Framework Adapter
if(BUILD_MT_FRAMEWORK_ADAPTER)
    add_library(uevr_mt_framework_adapter SHARED
        mt-framework/MTFrameworkAdapter.cpp
        mt-framework/MTFrameworkAdapter.hpp
    )
    
    target_link_libraries(uevr_mt_framework_adapter
        uevr_adapters_common
        d3d11
        dxgi
    )
    
    target_include_directories(uevr_mt_framework_adapter PRIVATE
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    )
    
    target_compile_definitions(uevr_mt_framework_adapter PRIVATE
        _CRT_SECURE_NO_WARNINGS
        UNICODE
        _UNICODE
        WIN32_LEAN_AND_MEAN
        NOMINMAX
        _WIN32_WINNT=0x0A00
        MT_FRAMEWORK_ADAPTER_EXPORTS
    )
    
    set_target_properties(uevr_mt_framework_adapter PROPERTIES
        OUTPUT_NAME "uevr_mt_framework_adapter"
        PREFIX ""
        SUFFIX ".dll"
    )
endif()

# Specific Game Adapters (Context7-powered)
# Cyberpunk 2077 Adapter
add_library(uevr_cyberpunk2077_adapter SHARED
    cyberpunk2077/cyberpunk2077_adapter.cpp
)

target_link_libraries(uevr_cyberpunk2077_adapter
    PRIVATE
        uevr_adapters_common
        d3d12
        dxgi
        dxguid
)

target_include_directories(uevr_cyberpunk2077_adapter PRIVATE
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_definitions(uevr_cyberpunk2077_adapter PRIVATE
    _CRT_SECURE_NO_WARNINGS
    UNICODE
    _UNICODE
    WIN32_LEAN_AND_MEAN
    NOMINMAX
    _WIN32_WINNT=0x0A00
    CYBERPUNK2077_ADAPTER_EXPORTS
)

set_target_properties(uevr_cyberpunk2077_adapter PROPERTIES
    OUTPUT_NAME "uevr_cyberpunk2077_adapter"
    PREFIX ""
    SUFFIX ".dll"
)

# Resident Evil 7 Adapter
add_library(uevr_re7_adapter SHARED
    re7/re7_adapter.cpp
)

target_link_libraries(uevr_re7_adapter
    PRIVATE
        uevr_adapters_common
        d3d11
        dxgi
        d3dcompiler
)

target_include_directories(uevr_re7_adapter PRIVATE
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_definitions(uevr_re7_adapter PRIVATE
    _CRT_SECURE_NO_WARNINGS
    UNICODE
    _UNICODE
    WIN32_LEAN_AND_MEAN
    NOMINMAX
    _WIN32_WINNT=0x0A00
    RE7_ADAPTER_EXPORTS
)

set_target_properties(uevr_re7_adapter PROPERTIES
    OUTPUT_NAME "uevr_re7_adapter"
    PREFIX ""
    SUFFIX ".dll"
)

# Monster Hunter World Adapter
add_library(uevr_mhw_adapter SHARED
    mhw/mhw_adapter.cpp
)

target_link_libraries(uevr_mhw_adapter
    PRIVATE
        uevr_adapters_common
        d3d11
        dxgi
        d3dcompiler
)

target_include_directories(uevr_mhw_adapter PRIVATE
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_definitions(uevr_mhw_adapter PRIVATE
    _CRT_SECURE_NO_WARNINGS
    UNICODE
    _UNICODE
    WIN32_LEAN_AND_MEAN
    NOMINMAX
    _WIN32_WINNT=0x0A00
    MHW_ADAPTER_EXPORTS
)

set_target_properties(uevr_mhw_adapter PROPERTIES
    OUTPUT_NAME "uevr_mhw_adapter"
    PREFIX ""
    SUFFIX ".dll"
)

# Installation
install(TARGETS uevr_adapters_common
    ARCHIVE DESTINATION lib
)

# Install specific game adapters
install(TARGETS 
    uevr_cyberpunk2077_adapter
    uevr_re7_adapter
    uevr_mhw_adapter
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
)

if(BUILD_RE_ENGINE_ADAPTER)
    install(TARGETS uevr_re_engine_adapter
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
    )
endif()

if(BUILD_REDENGINE4_ADAPTER)
    install(TARGETS uevr_redengine4_adapter
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
    )
endif()

if(BUILD_MT_FRAMEWORK_ADAPTER)
    install(TARGETS uevr_mt_framework_adapter
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
    )
endif()

# Print build configuration
message(STATUS "uevr Cross-Engine VR Adapters Build Configuration:")
message(STATUS "  RE Engine Adapter: ${BUILD_RE_ENGINE_ADAPTER}")
message(STATUS "  REDengine 4 Adapter: ${BUILD_REDENGINE4_ADAPTER}")
message(STATUS "  MT Framework Adapter: ${BUILD_MT_FRAMEWORK_ADAPTER}")
message(STATUS "  Tests: ${BUILD_TESTS}")
message(STATUS "  Examples: ${BUILD_EXAMPLES}")

