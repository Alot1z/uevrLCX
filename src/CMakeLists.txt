# uevrLCX Source Libraries Configuration

# Core library sources
set(CORE_SOURCES
    core/Framework.cpp
    core/GameDetector.cpp
    core/EngineAdapter.cpp
    core/VRSystem.cpp
    core/PerformanceMonitor.cpp
    core/ConfigurationManager.cpp
    core/LoggingSystem.cpp
    core/PluginManager.cpp
    core/ErrorHandler.cpp
    core/ThreadManager.cpp
    core/MemoryManager.cpp
    core/FileSystem.cpp
    core/NetworkManager.cpp
    core/SecurityManager.cpp
    core/UpdateManager.cpp
    core/BackupManager.cpp
    core/StatisticsManager.cpp
    core/NotificationManager.cpp
    core/ResourceManager.cpp
    core/CacheManager.cpp
    core/CompressionManager.cpp
    core/EncryptionManager.cpp
    core/ValidationManager.cpp
    core/CompatibilityManager.cpp
    core/ProfilingManager.cpp
    core/DebugManager.cpp
    core/TestingManager.cpp
    core/DeploymentManager.cpp
    ExceptionHandler.cpp
    Framework.cpp
)

# VR library sources
set(VR_SOURCES
    vr/UniversalVRConverter.cpp
    vr/AdvancedPhysicsIntegration.cpp
    vr/VRGINIntegration.cpp
    vr/Depth3DIntegration.cpp
    vr/ReshadeIntegration.cpp
    vr/PhysicsEngineIntegration.cpp
    vr/PerformanceOptimization.cpp
)

# AI library sources
set(AI_SOURCES
    ai/AIPoweredVROptimizer.cpp
)

# Neural Rendering library sources
set(NEURAL_SOURCES
    neural/NeuralRenderingSystem.cpp
)

# Advanced Engine Adapters library sources
set(ADVANCED_ADAPTER_SOURCES
    adapters/AdvancedEngineAdapters.cpp
)

# Community Learning library sources
set(COMMUNITY_SOURCES
    community/CommunityLearningSystem.cpp
    community/RedditCommunityIntegration.cpp
)

# Context7 library sources
set(CONTEXT7_SOURCES
    context7/Context7System.cpp
    context7/Context7Analyzer.cpp
    context7/Context7Processor.cpp
    context7/Context7Manager.cpp
)

# Utility library sources
set(UTILITY_SOURCES
    utility/UtilityFunctions.cpp
    utility/MathUtils.cpp
    utility/StringUtils.cpp
    utility/FileUtils.cpp
    utility/NetworkUtils.cpp
    utility/TimeUtils.cpp
    utility/ThreadUtils.cpp
    utility/MemoryUtils.cpp
)

# Main executable sources
set(MAIN_SOURCES
    uevrLCX_Main.cpp
)

# Create core library
add_library(uevr_core STATIC ${CORE_SOURCES})
target_include_directories(uevr_core PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
)
target_compile_definitions(uevr_core PRIVATE UEVR_CORE_EXPORTS)

# Create VR library
add_library(uevr_vr STATIC ${VR_SOURCES})
target_include_directories(uevr_vr PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
)
target_compile_definitions(uevr_vr PRIVATE UEVR_VR_EXPORTS)
target_link_libraries(uevr_vr uevr_core)

# Create AI library
add_library(uevr_ai STATIC ${AI_SOURCES})
target_include_directories(uevr_ai PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
)
target_compile_definitions(uevr_ai PRIVATE UEVR_AI_EXPORTS)
target_link_libraries(uevr_ai uevr_core)

# Create Neural Rendering library
add_library(uevr_neural STATIC ${NEURAL_SOURCES})
target_include_directories(uevr_neural PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
)
target_compile_definitions(uevr_neural PRIVATE UEVR_NEURAL_EXPORTS)
target_link_libraries(uevr_neural uevr_core uevr_ai)

# Create Advanced Engine Adapters library
add_library(uevr_advanced_adapters STATIC ${ADVANCED_ADAPTER_SOURCES})
target_include_directories(uevr_advanced_adapters PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
)
target_compile_definitions(uevr_advanced_adapters PRIVATE UEVR_ADVANCED_ADAPTERS_EXPORTS)
target_link_libraries(uevr_advanced_adapters uevr_core)

# Create Community Learning library
add_library(uevr_community STATIC ${COMMUNITY_SOURCES})
target_include_directories(uevr_community PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
)
target_compile_definitions(uevr_community PRIVATE UEVR_COMMUNITY_EXPORTS)
target_link_libraries(uevr_community uevr_core)

# Create Context7 library
add_library(uevr_context7 STATIC ${CONTEXT7_SOURCES})
target_include_directories(uevr_context7 PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
)
target_compile_definitions(uevr_context7 PRIVATE UEVR_CONTEXT7_EXPORTS)
target_link_libraries(uevr_context7 uevr_core)

# Create Utility library
add_library(uevr_utility STATIC ${UTILITY_SOURCES})
target_include_directories(uevr_utility PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
)
target_compile_definitions(uevr_utility PRIVATE UEVR_UTILITY_EXPORTS)

# Create main executable
add_executable(uevrLCX ${MAIN_SOURCES})
target_include_directories(uevrLCX PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
)

# Link all libraries to main executable
target_link_libraries(uevrLCX 
    uevr_core
    uevr_vr
    uevr_ai
    uevr_neural
    uevr_advanced_adapters
    uevr_community
    uevr_context7
    uevr_utility
)

# Set properties for main executable
set_target_properties(uevrLCX PROPERTIES
    OUTPUT_NAME "uevrLCX"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Platform-specific settings for main executable
if(WIN32)
    set_target_properties(uevrLCX PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()
