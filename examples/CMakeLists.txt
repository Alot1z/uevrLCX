# uevr Examples CMake Configuration
# 
# Builds example implementations showing how to use uevr
#
# Copyright (c) 2024 uevr Project

cmake_minimum_required(VERSION 3.16)
project(uevr_Examples)

# Example sources
set(EXAMPLE_SOURCES
    basic_plugin/basic_plugin.cpp
    custom_adapter/custom_adapter.cpp
    lua_scripting/lua_example.cpp
)

# Create example executables
foreach(EXAMPLE_SOURCE ${EXAMPLE_SOURCES})
    get_filename_component(EXAMPLE_NAME ${EXAMPLE_SOURCE} NAME_WE)
    
    add_executable(${EXAMPLE_NAME} ${EXAMPLE_SOURCE})
    
    target_include_directories(${EXAMPLE_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include/uevr
        ${CMAKE_SOURCE_DIR}/src
    )
    
    # Simplified linking without external dependencies
    target_link_libraries(${EXAMPLE_NAME}
        uevr_core
        uevr_adapters_common
        spdlog::spdlog
        nlohmann_json::nlohmann_json
    )
    
    set_target_properties(${EXAMPLE_NAME} PROPERTIES
        OUTPUT_NAME "${EXAMPLE_NAME}"
    )
endforeach()
