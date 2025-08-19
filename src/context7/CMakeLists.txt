# Context7 Database System CMake Configuration
# 
# This file configures the build system for the Context7 database system
# which provides intelligent game analysis and VR compatibility assessment.

cmake_minimum_required(VERSION 3.16)

# Set Context7 target name
set(CONTEXT7_TARGET "uevr_context7")

# Create Context7 library target
add_library(${CONTEXT7_TARGET} STATIC
    Context7Database.cpp
    Context7Database.hpp
)

# Set target properties
set_target_properties(${CONTEXT7_TARGET} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    POSITION_INDEPENDENT_CODE ON
)

# Set include directories
target_include_directories(${CONTEXT7_TARGET} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/dependencies/spdlog/include
    ${CMAKE_SOURCE_DIR}/dependencies/nlohmann_json/include
    ${CMAKE_SOURCE_DIR}/dependencies/curl/include
)

# Link dependencies
target_link_libraries(${CONTEXT7_TARGET} PUBLIC
    spdlog::spdlog
    nlohmann_json::nlohmann_json
    CURL::libcurl
)

# Compiler-specific settings
if(MSVC)
    target_compile_options(${CONTEXT7_TARGET} PRIVATE
        /W4
        /wd4251  # dll-interface warning
        /wd4275  # dll-interface warning
    )
else()
    target_compile_options(${CONTEXT7_TARGET} PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Werror=return-type
    )
endif()

# Install configuration
install(TARGETS ${CONTEXT7_TARGET}
    EXPORT ${CONTEXT7_TARGET}Targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(FILES Context7Database.hpp
    DESTINATION include/uevr/context7
)

# Export targets
install(EXPORT ${CONTEXT7_TARGET}Targets
    FILE ${CONTEXT7_TARGET}Targets.cmake
    NAMESPACE uevr::
    DESTINATION lib/cmake/${CONTEXT7_TARGET}
)

# Create and install config file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/${CONTEXT7_TARGET}ConfigVersion.cmake"
    VERSION 1.0.0
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/${CONTEXT7_TARGET}Config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${CONTEXT7_TARGET}Config.cmake"
    INSTALL_DESTINATION lib/cmake/${CONTEXT7_TARGET}
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/${CONTEXT7_TARGET}Config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/${CONTEXT7_TARGET}ConfigVersion.cmake"
    DESTINATION lib/cmake/${CONTEXT7_TARGET}
)

# Add Context7 to parent project
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
