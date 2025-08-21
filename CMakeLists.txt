cmake_minimum_required(VERSION 3.16)
project(uevrLCX VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Platform-specific settings
if(WIN32)
    set(PLATFORM_WINDOWS TRUE)
    add_definitions(-DWIN32_LEAN_AND_MEAN -D_CRT_SECURE_NO_WARNINGS)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
elseif(UNIX AND NOT APPLE)
    set(PLATFORM_LINUX TRUE)
    add_definitions(-DLINUX)
elseif(APPLE)
    set(PLATFORM_MACOS TRUE)
    add_definitions(-DMACOS)
endif()

# Compiler-specific settings
if(MSVC)
    add_compile_options(/W4 /MP)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Find required packages
find_package(PkgConfig QUIET)

# Graphics API dependencies
find_package(OpenGL REQUIRED)
find_package(Vulkan QUIET)

# Optional dependencies
find_package(Bullet QUIET)
find_package(PhysX QUIET)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/third_party
)

# Add subdirectories for component libraries
add_subdirectory(src)

# Platform-specific linking will be handled in src/CMakeLists.txt

# Optional physics engine linking will be handled in src/CMakeLists.txt

# Installation will be handled in src/CMakeLists.txt

# Install configuration files
install(FILES 
    config/default.ini
    config/performance.ini
    config/quality.ini
    DESTINATION share/uevrLCX/config
)

# Install documentation
install(FILES 
    README.md
    LICENSE
    DESTINATION share/uevrLCX
)

# Create package
include(CPack)
set(CPACK_PACKAGE_NAME "uevrLCX")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Universal VR Conversion System")
set(CPACK_PACKAGE_VENDOR "uevrLCX Team")
set(CPACK_PACKAGE_CONTACT "support@uevrlcx.com")

if(WIN32)
    set(CPACK_GENERATOR "NSIS;ZIP")
    set(CPACK_NSIS_DISPLAY_NAME "uevrLCX VR Conversion System")
    set(CPACK_NSIS_PACKAGE_NAME "uevrLCX")
    set(CPACK_NSIS_CONTACT "support@uevrlcx.com")
    set(CPACK_NSIS_URL "https://uevrlcx.com")
elseif(UNIX AND NOT APPLE)
    set(CPACK_GENERATOR "DEB;RPM;TGZ")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "uevrLCX Team")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libopengl0, libvulkan1")
    set(CPACK_RPM_PACKAGE_REQUIRES "opengl, vulkan")
elseif(APPLE)
    set(CPACK_GENERATOR "DragNDrop;TGZ")
    set(CPACK_DMG_VOLUME_NAME "uevrLCX")
    set(CPACK_DMG_FORMAT "UDZO")
endif()

# Testing
enable_testing()
if(EXISTS ${CMAKE_SOURCE_DIR}/tests)
    add_subdirectory(tests)
endif()

# Documentation
find_package(Doxygen QUIET)
if(DOXYGEN_FOUND)
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
    
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    
    add_custom_target(docs ALL
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM
    )
endif()

# Custom targets
add_custom_target(format
    COMMAND clang-format -i ${CMAKE_SOURCE_DIR}/src/*.cpp ${CMAKE_SOURCE_DIR}/src/*/*.cpp
    COMMAND clang-format -i ${CMAKE_SOURCE_DIR}/src/*.h ${CMAKE_SOURCE_DIR}/src/*/*.h
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Formatting source code"
)

add_custom_target(clean-all
    COMMAND ${CMAKE_BUILD_TOOL} clean
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Cleaning all build artifacts"
)

# Print configuration summary
message(STATUS "=== uevrLCX Configuration Summary ===")
message(STATUS "Platform: ${CMAKE_SYSTEM_NAME}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Install Prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "OpenGL: ${OPENGL_FOUND}")
message(STATUS "Vulkan: ${Vulkan_FOUND}")
message(STATUS "Bullet Physics: ${BULLET_FOUND}")
message(STATUS "PhysX: ${PHYSX_FOUND}")
message(STATUS "Doxygen: ${DOXYGEN_FOUND}")
message(STATUS "=====================================")