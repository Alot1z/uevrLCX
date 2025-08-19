#pragma once

#include <uevr/MainFramework.hpp>
#include <uevr/CrossEngineManager.hpp>
#include <uevr/VRFramework.hpp>
#include <memory>

// Forward declarations
class RE_Engine_Main_Integration;
class RE_Engine_CrossEngine_Bridge;
class RE_Engine_Integration;
class RE_Engine_Adapter;
class uevr::MainFramework;
class uevr::CrossEngineManager;
class uevr::VRFramework;

// RE Engine Final Integration Class
// This class ensures complete integration between RE Engine and uevr's main cross-engine framework
class RE_Engine_Final_Integration {
private:
    // Main Integration Components
    RE_Engine_Main_Integration* m_main_integration;
    RE_Engine_CrossEngine_Bridge* m_cross_engine_bridge;
    RE_Engine_Integration* m_integration_layer;
    RE_Engine_Adapter* m_engine_adapter;
    
    // uevr Framework Components
    uevr::MainFramework* m_main_framework;
    uevr::CrossEngineManager* m_cross_engine_manager;
    uevr::VRFramework* m_vr_framework;
    
    // Integration State
    bool m_final_integration_active;
    bool m_all_components_connected;
    bool m_vr_systems_ready;
    
    // Integration Verification
    struct Integration_Verification {
        bool main_integration_verified;
        bool cross_engine_bridge_verified;
        bool integration_layer_verified;
        bool engine_adapter_verified;
        bool vr_framework_verified;
        bool performance_systems_verified;
    } m_verification;

public:
    RE_Engine_Final_Integration();
    ~RE_Engine_Final_Integration();

    // Initialize Final Integration
    bool InitializeFinalIntegration(uevr::MainFramework* main_framework);
    
    // Activate Final Integration
    bool ActivateFinalIntegration();
    
    // Process Final Integration Frame
    void ProcessFinalIntegrationFrame();
    
    // Get Final Integration Status
    bool IsFinalIntegrationActive() const;
    
    // Get Integration Verification Status
    const Integration_Verification& GetVerificationStatus() const;
    
    // Shutdown Final Integration
    void ShutdownFinalIntegration();

private:
    bool InitializeAllComponents();
    bool VerifyCompleteIntegration();
    void ProcessVRFrame();
    void UpdateIntegrationStatus();
    void DeactivateFinalIntegration();
};

// Final Integration Interface Functions
extern "C" {
    __declspec(dllexport) bool InitializeREFinalIntegration(uevr::MainFramework* framework);
    __declspec(dllexport) bool ActivateREFinalIntegration();
    __declspec(dllexport) void ProcessREFinalIntegrationFrame();
    __declspec(dllexport) bool IsREFinalIntegrationActive();
    __declspec(dllexport) void ShutdownREFinalIntegration();
    __declspec(dllexport) const void* GetREFinalIntegrationVerificationStatus();
}
