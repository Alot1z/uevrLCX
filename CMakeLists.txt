cmake_minimum_required(VERSION 3.20)

# Project information
project(uevrLCX VERSION 2.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Compiler-specific flags
if(MSVC)
    add_compile_options(/W4 /WX- /MP)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS NOMINMAX)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /Oi /GL")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od /Ob0 /RTC1")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g -DDEBUG")
endif()

# Platform-specific settings
if(WIN32)
    add_compile_definitions(UNICODE _UNICODE WIN32_LEAN_AND_MEAN)
    set(PLATFORM_LIBS d3d11 d3d12 dxgi d3dcompiler)
elseif(UNIX AND NOT APPLE)
    set(PLATFORM_LIBS dl pthread)
elseif(APPLE)
    set(PLATFORM_LIBS "-framework Cocoa" "-framework IOKit" "-framework CoreVideo")
endif()

# Find required packages
find_package(PkgConfig REQUIRED)

# Find OpenXR
find_package(OpenXR REQUIRED)

# Find GLM (OpenGL Mathematics)
find_package(glm REQUIRED)

# Find MinHook
find_package(MinHook REQUIRED)

# Find spdlog
find_package(spdlog REQUIRED)

# Find OpenCV
find_package(OpenCV REQUIRED)

# Find PyTorch (LibTorch)
find_package(Torch REQUIRED)

# Find TensorFlow
find_package(TensorFlow REQUIRED)

# Set include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/SOURCECODE/UEVR/include
    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/include
)

# Set library directories
link_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/lib64
)

# Source files
set(SOURCES
    src/Main.cpp
    src/Framework.cpp
    src/Plugin.cpp
    src/Mod.cpp
    src/Mods.cpp
    src/WindowFilter.cpp
    src/ExceptionHandler.cpp
)

# Core system source files
set(CORE_SOURCES
    src/core/Framework.cpp
    src/core/PluginManager.cpp
    src/core/HookManager.cpp
    src/core/engine_detection.cpp
    src/core/adapter_loader.cpp
)

# VR system source files
set(VR_SOURCES
    src/vr/VRSystem.cpp
    src/vr/OpenXRIntegration.cpp
    src/vr/UniversalVRSystem.cpp
    src/vr/EngineAgnosticRenderer.cpp
    src/vr/FullPhysicsIntegration.cpp
    src/vr/FullAestheticCollisionEngine.cpp
)

# Context7 AI system source files
set(CONTEXT7_SOURCES
    src/context7/Context7AISystem.cpp
    src/context7/NeuralNetworkEngine.cpp
    src/context7/DeepWikiAI.cpp
    src/context7/Context7Database.cpp
    src/context7/GameAnalyzer.cpp
    src/context7/PerformanceOptimizer.cpp
    src/context7/AITrainingManager.cpp
)

# Engine adapters source files
set(ADAPTER_SOURCES
    src/adapters/EngineAdapter.cpp
    src/adapters/re-engine/ree_engine_adapter.cpp
    src/adapters/unity/UnityAdapter.cpp
    src/adapters/cryengine/CryEngineAdapter.cpp
    src/adapters/godot/GodotAdapter.cpp
)

# Header files
set(HEADERS
    src/Framework.hpp
    src/Plugin.hpp
    src/Mod.hpp
    src/Mods.hpp
    src/WindowFilter.hpp
    src/ExceptionHandler.hpp
)

# Core system headers
set(CORE_HEADERS
    src/core/Framework.hpp
    src/core/PluginManager.hpp
    src/core/HookManager.hpp
    src/core/engine_detection.hpp
    src/core/adapter_loader.hpp
)

# VR system headers
set(VR_HEADERS
    src/vr/VRSystem.hpp
    src/vr/VRSystem.h
    src/vr/OpenXRIntegration.hpp
    src/vr/UniversalVRSystem.hpp
    src/vr/EngineAgnosticRenderer.hpp
    include/uevr/vr/FullPhysicsIntegration.hpp
    include/uevr/vr/FullAestheticCollisionEngine.hpp
)

# Context7 AI system headers
set(CONTEXT7_HEADERS
    src/context7/Context7AISystem.hpp
    src/context7/NeuralNetworkEngine.hpp
    src/context7/DeepWikiAI.hpp
    src/context7/Context7Database.hpp
    src/context7/GameAnalyzer.hpp
    src/context7/PerformanceOptimizer.hpp
    src/context7/AITrainingManager.hpp
)

# Engine adapters headers
set(ADAPTER_HEADERS
    src/adapters/EngineAdapter.hpp
    src/adapters/re-engine/ree_engine_adapter.hpp
    src/adapters/unity/UnityAdapter.hpp
    src/adapters/cryengine/CryEngineAdapter.hpp
    src/adapters/godot/GodotAdapter.hpp
)

# Create core library
add_library(uevr_core STATIC ${CORE_SOURCES} ${CORE_HEADERS})
target_link_libraries(uevr_core 
    ${PLATFORM_LIBS}
    spdlog::spdlog
    MinHook::MinHook
)

# Create VR system library
add_library(uevr_vr STATIC ${VR_SOURCES} ${VR_HEADERS})
target_link_libraries(uevr_vr 
    uevr_core
    OpenXR::openxr_loader
    glm::glm
    ${PLATFORM_LIBS}
)

# Create Context7 AI system library
add_library(uevr_context7 STATIC ${CONTEXT7_SOURCES} ${CONTEXT7_HEADERS})
target_link_libraries(uevr_context7 
    uevr_core
    ${OpenCV_LIBS}
    ${TORCH_LIBRARIES}
    ${TensorFlow_LIBRARIES}
)

# Create engine adapters library
add_library(uevr_adapters STATIC ${ADAPTER_SOURCES} ${ADAPTER_HEADERS})
target_link_libraries(uevr_adapters 
    uevr_core
    uevr_vr
    ${PLATFORM_LIBS}
)

# Create main executable
add_executable(uevrLCX ${SOURCES} ${HEADERS})
target_link_libraries(uevrLCX 
    uevr_core
    uevr_vr
    uevr_context7
    uevr_adapters
    ${PLATFORM_LIBS}
)

# Set target properties
set_target_properties(uevrLCX PROPERTIES
    OUTPUT_NAME "uevrLCX"
    VERSION ${PROJECT_VERSION}
    DEBUG_POSTFIX "_d"
)

# Set target properties for libraries
set_target_properties(uevr_core PROPERTIES
    OUTPUT_NAME "uevr_core"
    VERSION ${PROJECT_VERSION}
    DEBUG_POSTFIX "_d"
)

set_target_properties(uevr_vr PROPERTIES
    OUTPUT_NAME "uevr_vr"
    VERSION ${PROJECT_VERSION}
    DEBUG_POSTFIX "_d"
)

set_target_properties(uevr_context7 PROPERTIES
    OUTPUT_NAME "uevr_context7"
    VERSION ${PROJECT_VERSION}
    DEBUG_POSTFIX "_d"
)

set_target_properties(uevr_adapters PROPERTIES
    OUTPUT_NAME "uevr_adapters"
    VERSION ${PROJECT_VERSION}
    DEBUG_POSTFIX "_d"
)

# Include directories for targets
target_include_directories(uevr_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_include_directories(uevr_vr PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_include_directories(uevr_context7 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_include_directories(uevr_adapters PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Compile definitions
target_compile_definitions(uevr_core PUBLIC
    UEVR_CORE_EXPORTS
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
)

target_compile_definitions(uevr_vr PUBLIC
    UEVR_VR_EXPORTS
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
)

target_compile_definitions(uevr_context7 PUBLIC
    UEVR_CONTEXT7_EXPORTS
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
)

target_compile_definitions(uevr_adapters PUBLIC
    UEVR_ADAPTERS_EXPORTS
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
)

# Install targets
install(TARGETS uevrLCX uevr_core uevr_vr uevr_context7 uevr_adapters
    EXPORT uevrLCXTargets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# Install headers
install(DIRECTORY include/ DESTINATION include)
install(DIRECTORY src/ DESTINATION include FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h")

# Install configuration files
install(FILES 
    docs/roadmap/vr-implementation-roadmap.md
    docs/roadmap/context7-roadmap.md
    docs/roadmap/hooks-implementation-roadmap.md
    docs/roadmap/COMPLETE_MVP_ROADMAP.md
    DESTINATION share/uevrLCX/docs
)

# Install license and readme
install(FILES 
    LICENSE
    README.md
    DESTINATION share/uevrLCX
)

# Export targets
install(EXPORT uevrLCXTargets
    FILE uevrLCXTargets.cmake
    NAMESPACE uevrLCX::
    DESTINATION lib/cmake/uevrLCX
)

# Create config file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "uevrLCXConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

# Install config files
install(FILES
    "uevrLCXConfigVersion.cmake"
    DESTINATION lib/cmake/uevrLCX
)

# Create and install config file
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/uevrLCXConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/uevrLCXConfig.cmake"
    @ONLY
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/uevrLCXConfig.cmake"
    DESTINATION lib/cmake/uevrLCX
)

# Enable testing
enable_testing()

# Add test targets
add_executable(uevr_tests
    test/test_framework.cpp
    test/test_vr_system.cpp
    test/test_ai_system.cpp
    test/test_adapters.cpp
)

target_link_libraries(uevr_tests
    uevr_core
    uevr_vr
    uevr_context7
    uevr_adapters
    gtest
    gtest_main
)

# Add tests
add_test(NAME FrameworkTests COMMAND uevr_tests --gtest_filter="Framework*")
add_test(NAME VRSystemTests COMMAND uevr_tests --gtest_filter="VRSystem*")
add_test(NAME AISystemTests COMMAND uevr_tests --gtest_filter="AISystem*")
add_test(NAME AdapterTests COMMAND uevr_tests --gtest_filter="Adapter*")

# Set test properties
set_tests_properties(FrameworkTests PROPERTIES
    TIMEOUT 300
    ENVIRONMENT "UEVR_TEST_MODE=1"
)

set_tests_properties(VRSystemTests PROPERTIES
    TIMEOUT 300
    ENVIRONMENT "UEVR_TEST_MODE=1"
)

set_tests_properties(AISystemTests PROPERTIES
    TIMEOUT 300
    ENVIRONMENT "UEVR_TEST_MODE=1"
)

set_tests_properties(AdapterTests PROPERTIES
    TIMEOUT 300
    ENVIRONMENT "UEVR_TEST_MODE=1"
)

# Custom targets
add_custom_target(docs
    COMMAND ${CMAKE_COMMAND} -E echo "Generating documentation..."
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/docs
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/docs ${CMAKE_BINARY_DIR}/docs
    COMMENT "Generating documentation"
)

add_custom_target(package
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target package
    COMMENT "Creating package"
)

# Dependencies
add_custom_target(download_deps
    COMMAND ${CMAKE_COMMAND} -E echo "Downloading dependencies..."
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_SOURCE_DIR}/cmake/DownloadDependencies.cmake
    COMMENT "Downloading dependencies"
)

# Set dependency order
add_dependencies(uevr_core download_deps)
add_dependencies(uevr_vr uevr_core)
add_dependencies(uevr_context7 uevr_core)
add_dependencies(uevr_adapters uevr_core uevr_vr)
add_dependencies(uevrLCX uevr_core uevr_vr uevr_context7 uevr_adapters)

# Print configuration summary
message(STATUS "uevrLCX Configuration Summary:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Platform: ${CMAKE_SYSTEM_NAME}")
message(STATUS "  Architecture: ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "  Install Prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "  Build Directory: ${CMAKE_BINARY_DIR}")
message(STATUS "  Source Directory: ${CMAKE_SOURCE_DIR}")
message(STATUS "  Output Directory: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

# Print found packages
message(STATUS "Found Packages:")
message(STATUS "  OpenXR: ${OpenXR_FOUND}")
message(STATUS "  GLM: ${glm_FOUND}")
message(STATUS "  MinHook: ${MinHook_FOUND}")
message(STATUS "  spdlog: ${spdlog_FOUND}")
message(STATUS "  OpenCV: ${OpenCV_FOUND}")
message(STATUS "  PyTorch: ${Torch_FOUND}")
message(STATUS "  TensorFlow: ${TensorFlow_FOUND}")

# Print target information
message(STATUS "Targets:")
message(STATUS "  uevr_core: Core framework library")
message(STATUS "  uevr_vr: VR system library")
message(STATUS "  uevr_context7: Context7 AI system library")
message(STATUS "  uevr_adapters: Engine adapters library")
message(STATUS "  uevrLCX: Main executable")
message(STATUS "  uevr_tests: Test suite")
message(STATUS "  docs: Documentation generation")
message(STATUS "  package: Package creation")
message(STATUS "  download_deps: Dependency download")