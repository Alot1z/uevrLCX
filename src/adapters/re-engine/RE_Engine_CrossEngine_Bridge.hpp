#pragma once

#include <uevr/CrossEngineAPI.hpp>
#include <uevr/VRFramework.hpp>
#include <memory>

// Forward declarations
class RE_Engine_Integration;
class uevr::CrossEngineAPI;
class uevr::VRFramework;

// RE Engine Cross-Engine Bridge Class
// This bridge ensures complete integration between RE Engine and uevr's main cross-engine framework
class RE_Engine_CrossEngine_Bridge {
private:
    RE_Engine_Integration* m_integration;
    uevr::CrossEngineAPI* m_cross_engine_api;
    uevr::VRFramework* m_vr_framework;
    
    // Cross-Engine State Management
    bool m_bridge_initialized;
    bool m_cross_engine_active;
    
    // RE Engine Specific Cross-Engine Features
    struct RE_CrossEngine_Features {
        bool stereo_rendering_enabled;
        bool motion_controllers_active;
        bool camera_tracking_enabled;
        bool performance_optimization_active;
    } m_features;

public:
    RE_Engine_CrossEngine_Bridge();
    ~RE_Engine_CrossEngine_Bridge();

    // Initialize Cross-Engine Bridge
    bool InitializeBridge(uevr::CrossEngineAPI* cross_engine_api);
    
    // Activate Cross-Engine Integration
    bool ActivateCrossEngine();
    
    // Process Cross-Engine Frame
    void ProcessCrossEngineFrame();
    
    // Get Cross-Engine Status
    bool IsCrossEngineActive() const;
    
    // Shutdown Bridge
    void ShutdownBridge();

private:
    bool InitializeCrossEngineFeatures();
    bool EnableCrossEngineFeatures();
    void ProcessVRFrame();
    void UpdateCrossEngineFeatures();
    void DeactivateCrossEngine();
};

// Cross-Engine Bridge Interface Functions
extern "C" {
    __declspec(dllexport) bool InitializeRECrossEngineBridge(uevr::CrossEngineAPI* api);
    __declspec(dllexport) bool ActivateRECrossEngine();
    __declspec(dllexport) void ProcessRECrossEngineFrame();
    __declspec(dllexport) bool IsRECrossEngineActive();
    __declspec(dllexport) void ShutdownRECrossEngineBridge();
}
