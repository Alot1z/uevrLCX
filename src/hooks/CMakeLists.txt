# uevr Hooks CMake Configuration
# 
# Builds the engine hooking and interception system
#
# Copyright (c) 2024 uevr Project

# Hook sources
set(HOOK_SOURCES
    DirectXHooks.cpp
    OpenGLHooks.cpp
    VulkanHooks.cpp
    EngineHooks.cpp
    HookManager.cpp
)

# Create hooks library
add_library(uevr_hooks STATIC ${HOOK_SOURCES})

# Include directories
target_include_directories(uevr_hooks PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    ${CMAKE_CURRENT_SOURCE_DIR}/../core
)

# Link dependencies
target_link_libraries(uevr_hooks
    uevr_core
    MinHook::MinHook
    spdlog::spdlog
)

# Compile definitions
target_compile_definitions(uevr_hooks PRIVATE
    uevr_HOOKS_EXPORTS
    _CRT_SECURE_NO_WARNINGS
    UNICODE
    _UNICODE
    WIN32_LEAN_AND_MEAN
    NOMINMAX
    _WIN32_WINNT=0x0601
)
