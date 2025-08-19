#include "RE_Engine_Integration.hpp"
#include "RE_Engine_Adapter.hpp"
#include <uevr/CrossEngineAPI.hpp>
#include <uevr/VRFramework.hpp>
#include <spdlog/spdlog.h>

// RE Engine Cross-Engine Bridge Implementation
// This bridge ensures complete integration between RE Engine and uevr's cross-engine framework

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
    RE_Engine_CrossEngine_Bridge() : 
        m_integration(nullptr),
        m_cross_engine_api(nullptr),
        m_vr_framework(nullptr),
        m_bridge_initialized(false),
        m_cross_engine_active(false) {
        
        memset(&m_features, 0, sizeof(m_features));
        spdlog::info("RE Engine Cross-Engine Bridge: Initializing");
    }

    ~RE_Engine_CrossEngine_Bridge() {
        ShutdownBridge();
    }

    // Initialize Cross-Engine Bridge
    bool InitializeBridge(uevr::CrossEngineAPI* cross_engine_api) {
        if (!cross_engine_api) {
            spdlog::error("RE Engine Cross-Engine Bridge: Invalid Cross-Engine API");
            return false;
        }

        m_cross_engine_api = cross_engine_api;
        spdlog::info("RE Engine Cross-Engine Bridge: Cross-Engine API acquired");

        // Get VR Framework
        m_vr_framework = m_cross_engine_api->GetVRFramework();
        if (!m_vr_framework) {
            spdlog::error("RE Engine Cross-Engine Bridge: Failed to get VR Framework");
            return false;
        }

        // Initialize Cross-Engine Features
        if (!InitializeCrossEngineFeatures()) {
            spdlog::error("RE Engine Cross-Engine Bridge: Failed to initialize features");
            return false;
        }

        m_bridge_initialized = true;
        spdlog::info("RE Engine Cross-Engine Bridge: Bridge initialized successfully");

        return true;
    }

    // Activate Cross-Engine Integration
    bool ActivateCrossEngine() {
        if (!m_bridge_initialized) {
            spdlog::error("RE Engine Cross-Engine Bridge: Bridge not initialized");
            return false;
        }

        spdlog::info("RE Engine Cross-Engine Bridge: Activating cross-engine integration");

        // Enable Cross-Engine Features
        if (!EnableCrossEngineFeatures()) {
            spdlog::error("RE Engine Cross-Engine Bridge: Failed to enable features");
            return false;
        }

        // Activate VR Framework
        if (m_vr_framework) {
            m_vr_framework->Activate();
        }

        m_cross_engine_active = true;
        spdlog::info("RE Engine Cross-Engine Bridge: Cross-engine integration activated");

        return true;
    }

    // Process Cross-Engine Frame
    void ProcessCrossEngineFrame() {
        if (!m_cross_engine_active || !m_integration) {
            return;
        }

        // Process VR Frame through Cross-Engine Framework
        if (m_vr_framework && m_vr_framework->IsActive()) {
            ProcessVRFrame();
        }

        // Update Cross-Engine Features
        UpdateCrossEngineFeatures();
    }

    // Get Cross-Engine Status
    bool IsCrossEngineActive() const {
        return m_cross_engine_active && m_bridge_initialized;
    }

    // Shutdown Bridge
    void ShutdownBridge() {
        spdlog::info("RE Engine Cross-Engine Bridge: Shutting down bridge");

        if (m_cross_engine_active) {
            DeactivateCrossEngine();
        }

        if (m_vr_framework) {
            m_vr_framework->Deactivate();
        }

        m_bridge_initialized = false;
        m_cross_engine_active = false;
        m_cross_engine_api = nullptr;
        m_vr_framework = nullptr;
    }

private:
    bool InitializeCrossEngineFeatures() {
        spdlog::info("RE Engine Cross-Engine Bridge: Initializing cross-engine features");

        // Initialize Stereo Rendering
        m_features.stereo_rendering_enabled = m_cross_engine_api->InitializeStereoRendering();
        
        // Initialize Motion Controllers
        m_features.motion_controllers_active = m_cross_engine_api->InitializeMotionControllers();
        
        // Initialize Camera Tracking
        m_features.camera_tracking_enabled = m_cross_engine_api->InitializeCameraTracking();
        
        // Initialize Performance Optimization
        m_features.performance_optimization_active = m_cross_engine_api->InitializePerformanceOptimization();

        spdlog::info("RE Engine Cross-Engine Bridge: Features initialized - Stereo: {}, Controllers: {}, Camera: {}, Performance: {}",
            m_features.stereo_rendering_enabled,
            m_features.motion_controllers_active,
            m_features.camera_tracking_enabled,
            m_features.performance_optimization_active
        );

        return true;
    }

    bool EnableCrossEngineFeatures() {
        spdlog::info("RE Engine Cross-Engine Bridge: Enabling cross-engine features");

        // Enable Stereo Rendering
        if (m_features.stereo_rendering_enabled) {
            m_cross_engine_api->EnableStereoRendering();
        }

        // Enable Motion Controllers
        if (m_features.motion_controllers_active) {
            m_cross_engine_api->EnableMotionControllers();
        }

        // Enable Camera Tracking
        if (m_features.camera_tracking_enabled) {
            m_cross_engine_api->EnableCameraTracking();
        }

        // Enable Performance Optimization
        if (m_features.performance_optimization_active) {
            m_cross_engine_api->EnablePerformanceOptimization();
        }

        return true;
    }

    void ProcessVRFrame() {
        // Process VR frame through cross-engine framework
        if (m_vr_framework) {
            m_vr_framework->ProcessFrame();
        }
    }

    void UpdateCrossEngineFeatures() {
        // Update cross-engine feature states
        if (m_cross_engine_api) {
            m_cross_engine_api->UpdateFeatures();
        }
    }

    void DeactivateCrossEngine() {
        spdlog::info("RE Engine Cross-Engine Bridge: Deactivating cross-engine integration");

        if (m_cross_engine_api) {
            m_cross_engine_api->DisableAllFeatures();
        }

        m_cross_engine_active = false;
    }
};

// Global Cross-Engine Bridge Instance
static RE_Engine_CrossEngine_Bridge* g_cross_engine_bridge = nullptr;

// Cross-Engine Bridge Interface Functions
extern "C" {
    __declspec(dllexport) bool InitializeRECrossEngineBridge(uevr::CrossEngineAPI* api) {
        if (g_cross_engine_bridge) {
            delete g_cross_engine_bridge;
        }
        
        g_cross_engine_bridge = new RE_Engine_CrossEngine_Bridge();
        return g_cross_engine_bridge->InitializeBridge(api);
    }

    __declspec(dllexport) bool ActivateRECrossEngine() {
        if (g_cross_engine_bridge) {
            return g_cross_engine_bridge->ActivateCrossEngine();
        }
        return false;
    }

    __declspec(dllexport) void ProcessRECrossEngineFrame() {
        if (g_cross_engine_bridge) {
            g_cross_engine_bridge->ProcessCrossEngineFrame();
        }
    }

    __declspec(dllexport) bool IsRECrossEngineActive() {
        if (g_cross_engine_bridge) {
            return g_cross_engine_bridge->IsCrossEngineActive();
        }
        return false;
    }

    __declspec(dllexport) void ShutdownRECrossEngineBridge() {
        if (g_cross_engine_bridge) {
            g_cross_engine_bridge->ShutdownBridge();
            delete g_cross_engine_bridge;
            g_cross_engine_bridge = nullptr;
        }
    }
}
