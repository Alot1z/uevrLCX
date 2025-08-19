#pragma once

#include "uevr/Plugin.hpp"
#include <windows.h>
#include <d3d11.h>
#include <d3d12.h>
#include <openxr/openxr.h>
#include <openxr/openxr_platform.h>

namespace RE_Engine {

// RE7 VR Adapter Plugin - Extends uevr's plugin architecture for RE Engine
class RE7VRAdapter : public uevr::Plugin {
private:
    // RE Engine specific state
    bool m_reEngineDetected;
    uintptr_t m_cameraSystemAddr;
    uintptr_t m_renderingPipelineAddr;
    
    // VR rendering state
    bool m_vrEnabled;
    XrSession m_xrSession;
    XrSpace m_viewSpace;
    XrSpace m_localSpace;
    
    // D3D11 hooks for RE Engine
    ID3D11Device* m_d3d11Device;
    ID3D11DeviceContext* m_d3d11Context;
    ID3D11RenderTargetView* m_renderTargetView;

public:
    RE7VRAdapter();
    virtual ~RE7VRAdapter();

    // Plugin initialization - called by uevr
    virtual void on_initialize() override;

    // D3D11 post-render hook - called by uevr after RE Engine renders
    virtual void on_post_render_vr_framework_dx11(
        ID3D11DeviceContext* context, 
        ID3D11Texture2D* texture, 
        ID3D11RenderTargetView* rtv) override;

    // Device reset hook
    virtual void on_device_reset() override;

private:
    // Detect RE Engine by scanning process memory
    bool detectREEngine();

    // Initialize RE Engine specific hooks
    bool initializeREEngineHooks();

    // Hook into RE Engine's camera system
    bool hookRECameraSystem();

    // Hook into RE Engine's rendering pipeline
    bool hookRERenderingPipeline();

    // Initialize VR system using OpenXR
    bool initializeVR();

    // Render VR frame
    void renderVRFrame();

    // Render VR views for stereo rendering
    void renderVRViews();

    // Render individual eye view
    void renderEye(uint32_t eyeIndex, const XrView& view);

    // Cleanup VR resources
    void cleanupVR();

    // General cleanup
    void cleanup();
};

} // namespace RE_Engine
