# uevr Mods CMake Configuration
# 
# Builds the VR modification and enhancement system
#
# Copyright (c) 2024 uevr Project

# Mod sources
set(MOD_SOURCES
    VRModManager.cpp
    CameraMod.cpp
    ControllerMod.cpp
    DisplayMod.cpp
    AudioMod.cpp
    PerformanceMod.cpp
)

# Create mods library
add_library(uevr_mods STATIC ${MOD_SOURCES})

# Include directories
target_include_directories(uevr_mods PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    ${CMAKE_CURRENT_SOURCE_DIR}/../core
    ${CMAKE_CURRENT_SOURCE_DIR}/../hooks
)

# Link dependencies
target_link_libraries(uevr_mods
    uevr_core
    uevr_hooks
    spdlog::spdlog
    nlohmann_json::nlohmann_json
)

# Compile definitions
target_compile_definitions(uevr_mods PRIVATE
    UEVR_MODS_EXPORTS
    _CRT_SECURE_NO_WARNINGS
    UNICODE
    _UNICODE
    WIN32_LEAN_AND_MEAN
    NOMINMAX
    _WIN32_WINNT=0x0601
)
