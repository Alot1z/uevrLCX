# VR System Components
# 
# Universal VR System and Engine-Agnostic Renderer

add_library(uevr_vr STATIC
    VRSystem.cpp
    UniversalVRSystem.cpp
    FullAestheticCollisionEngine.cpp
    FullPhysicsIntegration.cpp
    EngineAgnosticRenderer.cpp
    OpenXRIntegration.cpp
)

target_include_directories(uevr_vr PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../dependencies/submodules/glm
)

target_link_libraries(uevr_vr
    PUBLIC
        spdlog::spdlog
        nlohmann_json::nlohmann_json
        glm::glm
    PRIVATE
        uevr_core
)

# VR runtime specific libraries
if(uevr_ENABLE_OPENVR AND TARGET openvr_api)
    target_link_libraries(uevr_vr PRIVATE openvr_api)
endif()

if(uevr_ENABLE_OPENXR AND TARGET OpenXR::OpenXR)
    target_link_libraries(uevr_vr PRIVATE OpenXR::OpenXR)
endif()

if(uevr_ENABLE_VULKAN AND TARGET Vulkan::Vulkan)
    target_link_libraries(uevr_vr PRIVATE Vulkan::Vulkan)
endif()

if(uevr_ENABLE_OPENGL AND TARGET OpenGL::GL)
    target_link_libraries(uevr_vr PRIVATE OpenGL::GL)
endif()

# Platform-specific graphics libraries
if(WIN32)
    target_link_libraries(uevr_vr PRIVATE d3d11 d3d12 dxgi)
endif()

target_compile_definitions(uevr_vr
    PRIVATE
        $<$<BOOL:${uevr_ENABLE_OPENVR}>:UEVR_ENABLE_OPENVR>
        $<$<BOOL:${uevr_ENABLE_OPENXR}>:UEVR_ENABLE_OPENXR>
        $<$<BOOL:${uevr_ENABLE_VULKAN}>:UEVR_ENABLE_VULKAN>
        $<$<BOOL:${uevr_ENABLE_OPENGL}>:UEVR_ENABLE_OPENGL>
)

message(STATUS "Configured VR system components with full aesthetic collision and physics integration")