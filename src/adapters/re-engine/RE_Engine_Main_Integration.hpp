#pragma once

#include <uevr/MainFramework.hpp>
#include <uevr/CrossEngineManager.hpp>
#include <memory>

// Forward declarations
class RE_Engine_Integration;
class RE_Engine_CrossEngine_Bridge;
class RE_Engine_Adapter;
class uevr::MainFramework;
class uevr::CrossEngineManager;

// RE Engine Main Integration Class
// This is the primary integration point that connects RE Engine with uevr's main system
class RE_Engine_Main_Integration {
private:
    // Core Components
    RE_Engine_Integration* m_main_integration;
    RE_Engine_CrossEngine_Bridge* m_cross_engine_bridge;
    RE_Engine_Adapter* m_engine_adapter;
    
    // uevr Main Framework Integration
    uevr::MainFramework* m_main_framework;
    uevr::CrossEngineManager* m_cross_engine_manager;
    
    // Integration State
    bool m_main_integration_active;
    bool m_cross_engine_connected;
    bool m_adapter_initialized;
    
    // RE Engine Specific Integration Features
    struct RE_Main_Integration_Features {
        bool main_framework_connected;
        bool cross_engine_bridge_active;
        bool adapter_plugin_loaded;
        bool vr_systems_initialized;
        bool performance_monitoring_active;
    } m_features;

public:
    RE_Engine_Main_Integration();
    ~RE_Engine_Main_Integration();

    // Initialize Main Integration with uevr
    bool InitializeMainIntegration(uevr::MainFramework* main_framework);
    
    // Activate Main Integration
    bool ActivateMainIntegration();
    
    // Process Main Integration Frame
    void ProcessMainIntegrationFrame();
    
    // Get Main Integration Status
    bool IsMainIntegrationActive() const;
    
    // Shutdown Main Integration
    void ShutdownMainIntegration();

private:
    bool InitializeMainIntegrationFeatures();
    void UpdateMainIntegrationFeatures();
    void DeactivateMainIntegration();
};

// Main Integration Interface Functions
extern "C" {
    __declspec(dllexport) bool InitializeREMainIntegration(uevr::MainFramework* framework);
    __declspec(dllexport) bool ActivateREMainIntegration();
    __declspec(dllexport) void ProcessREMainIntegrationFrame();
    __declspec(dllexport) bool IsREMainIntegrationActive();
    __declspec(dllexport) void ShutdownREMainIntegration();
}
