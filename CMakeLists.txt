cmake_minimum_required(VERSION 3.16)
project(uevrLCX VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# DeepWiki AI Integration
option(DEEPWIKI_ENABLED "Enable DeepWiki AI integration" ON)
option(DEEPWIKI_QUANTUM_ENCRYPTION "Enable quantum encryption for DeepWiki" ON)
option(DEEPWIKI_BITS_STORAGE "Enable bits-based storage" ON)

# Platform detection
if(WIN32)
    set(PLATFORM_WINDOWS TRUE)
elseif(APPLE)
    set(PLATFORM_MACOS TRUE)
else()
    set(PLATFORM_LINUX TRUE)
endif()

# Find required packages
find_package(OpenGL REQUIRED)
find_package(Vulkan QUIET)
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)

# DeepWiki AI dependencies
if(DEEPWIKI_ENABLED)
    find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
    find_package(CURL REQUIRED)
    find_package(OpenSSL REQUIRED)
    
    # DeepWiki specific packages
    find_package(Boost COMPONENTS system filesystem REQUIRED)
    find_package(Threads REQUIRED)
endif()

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/third_party
)

# DeepWiki AI include directories
if(DEEPWIKI_ENABLED)
    include_directories(
        ${CMAKE_SOURCE_DIR}/src/DeepWikiAI
        ${CMAKE_SOURCE_DIR}/plugins
        ${CURL_INCLUDE_DIRS}
        ${OPENSSL_INCLUDE_DIR}
        ${Boost_INCLUDE_DIRS}
    )
endif()

# Source files
set(SOURCES
    src/Main.cpp
    src/UEVRLCX_Core.cpp
    src/VR/VRConversionSystem.cpp
    src/VR/VRGINInjector.cpp
    src/VR/Depth3DExtractor.cpp
    src/VR/ReshadeFramework.cpp
    src/VR/PhysicsEngine.cpp
    src/VR/CollisionDetector.cpp
    src/VR/PerformanceOptimizer.cpp
    src/VR/FoveatedRenderer.cpp
    src/VR/EngineAdapter.cpp
    src/Security/SecurityFramework.cpp
)

# DeepWiki AI source files
if(DEEPWIKI_ENABLED)
    set(DEEPWIKI_SOURCES
        src/DeepWikiAI/GameAnalyzer.cpp
        src/DeepWikiAI/VRCompatibilityChecker.cpp
        src/DeepWikiAI/PerformancePredictor.cpp
        src/DeepWikiAI/ModCompatibilityAnalyzer.cpp
        src/DeepWikiAI/CommunityIntelligence.cpp
        plugins/vr_transformer.py
        plugins/game_converter.py
        plugins/complete_analyzer.py
    )
    list(APPEND SOURCES ${DEEPWIKI_SOURCES})
endif()

# Create executable
add_executable(uevrLCX ${SOURCES})

# Link libraries
target_link_libraries(uevrLCX
    OpenGL::GL
    ${CMAKE_THREAD_LIBS_INIT}
)

# Platform-specific libraries
if(PLATFORM_WINDOWS)
    target_link_libraries(uevrLCX
        ws2_32
        winmm
        d3d11
        dxgi
    )
elseif(PLATFORM_LINUX)
    target_link_libraries(uevrLCX
        X11
        Xrandr
        Xxf86vm
        dl
    )
elseif(PLATFORM_MACOS)
    target_link_libraries(uevrLCX
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
    )
endif()

# DeepWiki AI libraries
if(DEEPWIKI_ENABLED)
    target_link_libraries(uevrLCX
        ${CURL_LIBRARIES}
        ${OPENSSL_LIBRARIES}
        ${Boost_LIBRARIES}
        Python3::Python
        Python3::Module
    )
    
    # DeepWiki configuration
    target_compile_definitions(uevrLCX PRIVATE
        DEEPWIKI_ENABLED=1
        DEEPWIKI_CONFIG_PATH="${CMAKE_SOURCE_DIR}/.deepwiki-docs/config.yml"
    )
    
    if(DEEPWIKI_QUANTUM_ENCRYPTION)
        target_compile_definitions(uevrLCX PRIVATE
            DEEPWIKI_QUANTUM_ENCRYPTION=1
        )
    endif()
    
    if(DEEPWIKI_BITS_STORAGE)
        target_compile_definitions(uevrLCX PRIVATE
            DEEPWIKI_BITS_STORAGE=1
        )
    endif()
endif()

# Compiler-specific flags
if(MSVC)
    target_compile_options(uevrLCX PRIVATE /W4)
else()
    target_compile_options(uevrLCX PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Debug flags
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(uevrLCX PRIVATE DEBUG=1)
    if(MSVC)
        target_compile_options(uevrLCX PRIVATE /MTd)
    endif()
else()
    if(MSVC)
        target_compile_options(uevrLCX PRIVATE /MT /O2)
    else()
        target_compile_options(uevrLCX PRIVATE -O3)
    endif()
endif()

# Install rules
install(TARGETS uevrLCX
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# Install configuration files
install(FILES
    config/default.ini
    config/performance.ini
    config/quality.ini
    DESTINATION share/uevrLCX/config
)

# Install DeepWiki configuration
if(DEEPWIKI_ENABLED)
    install(FILES
        .deepwiki-docs/config.yml
        config/deepwiki.ini
        DESTINATION share/uevrLCX/deepwiki
    )
    
    # Install Python plugins
    install(FILES
        plugins/vr_transformer.py
        plugins/game_converter.py
        plugins/complete_analyzer.py
        DESTINATION share/uevrLCX/plugins
    )
endif()

# Install documentation
install(FILES
    README.md
    LICENSE
    DESTINATION share/uevrLCX/docs
)

# Install DeepWiki documentation
if(DEEPWIKI_ENABLED)
    install(DIRECTORY .deepwiki-docs/
        DESTINATION share/uevrLCX/docs/deepwiki
        FILES_MATCHING PATTERN "*.md"
    )
endif()

# Testing
enable_testing()

# DeepWiki integration tests
if(DEEPWIKI_ENABLED)
    add_test(NAME DeepWikiConfigTest
        COMMAND ${CMAKE_COMMAND} -E echo "Testing DeepWiki configuration..."
    )
    
    add_test(NAME DeepWikiIntegrationTest
        COMMAND ${CMAKE_COMMAND} -E echo "Testing DeepWiki integration..."
    )
endif()

# Package configuration
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/uevrLCXConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

# CPack configuration
set(CPACK_PACKAGE_NAME "uevrLCX")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Universal VR Conversion System with DeepWiki AI")
set(CPACK_PACKAGE_VENDOR "uevrLCX Team")
set(CPACK_PACKAGE_CONTACT "https://github.com/uevrLCX/uevrLCX")

if(WIN32)
    set(CPACK_GENERATOR "NSIS;ZIP")
    set(CPACK_NSIS_DISPLAY_NAME "uevrLCX VR Conversion System")
    set(CPACK_NSIS_PACKAGE_NAME "uevrLCX")
elseif(APPLE)
    set(CPACK_GENERATOR "DragNDrop;TGZ")
else()
    set(CPACK_GENERATOR "DEB;RPM;TGZ")
endif()

include(CPack)

# Print configuration summary
message(STATUS "=== uevrLCX Configuration Summary ===")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Platform: ${CMAKE_SYSTEM_NAME}")
message(STATUS "DeepWiki AI: ${DEEPWIKI_ENABLED}")
if(DEEPWIKI_ENABLED)
    message(STATUS "  Quantum Encryption: ${DEEPWIKI_QUANTUM_ENCRYPTION}")
    message(STATUS "  Bits Storage: ${DEEPWIKI_BITS_STORAGE}")
endif()
message(STATUS "OpenGL: ${OpenGL_FOUND}")
message(STATUS "Vulkan: ${Vulkan_FOUND}")
message(STATUS "Python3: ${Python3_FOUND}")
if(DEEPWIKI_ENABLED)
    message(STATUS "CURL: ${CURL_FOUND}")
    message(STATUS "OpenSSL: ${OPENSSL_FOUND}")
    message(STATUS "Boost: ${Boost_FOUND}")
endif()
message(STATUS "================================")