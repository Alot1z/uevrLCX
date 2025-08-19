#include "RE_Engine_Integration.hpp"
#include "RE_Engine_CrossEngine_Bridge.hpp"
#include "RE_Engine_Adapter.hpp"
#include <uevr/MainFramework.hpp>
#include <uevr/CrossEngineManager.hpp>
#include <spdlog/spdlog.h>

// RE Engine Main Integration with uevr Main Cross-Engine Framework
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
    RE_Engine_Main_Integration() : 
        m_main_integration(nullptr),
        m_cross_engine_bridge(nullptr),
        m_engine_adapter(nullptr),
        m_main_framework(nullptr),
        m_cross_engine_manager(nullptr),
        m_main_integration_active(false),
        m_cross_engine_connected(false),
        m_adapter_initialized(false) {
        
        memset(&m_features, 0, sizeof(m_features));
        spdlog::info("RE Engine Main Integration: Initializing with uevr Main Framework");
    }

    ~RE_Engine_Main_Integration() {
        ShutdownMainIntegration();
    }

    // Initialize Main Integration with uevr
    bool InitializeMainIntegration(uevr::MainFramework* main_framework) {
        if (!main_framework) {
            spdlog::error("RE Engine Main Integration: Invalid Main Framework");
            return false;
        }

        m_main_framework = main_framework;
        spdlog::info("RE Engine Main Integration: Main Framework acquired");

        // Get Cross-Engine Manager
        m_cross_engine_manager = m_main_framework->GetCrossEngineManager();
        if (!m_cross_engine_manager) {
            spdlog::error("RE Engine Main Integration: Failed to get Cross-Engine Manager");
            return false;
        }

        // Initialize Main Integration Features
        if (!InitializeMainIntegrationFeatures()) {
            spdlog::error("RE Engine Main Integration: Failed to initialize features");
            return false;
        }

        // Create Main Integration
        m_main_integration = new RE_Engine_Integration();
        if (!m_main_integration) {
            spdlog::error("RE Engine Main Integration: Failed to create main integration");
            return false;
        }

        // Create Cross-Engine Bridge
        m_cross_engine_bridge = new RE_Engine_CrossEngine_Bridge();
        if (!m_cross_engine_bridge) {
            spdlog::error("RE Engine Main Integration: Failed to create cross-engine bridge");
            return false;
        }

        // Create Engine Adapter
        m_engine_adapter = new RE_Engine_Adapter();
        if (!m_engine_adapter) {
            spdlog::error("RE Engine Main Integration: Failed to create engine adapter");
            return false;
        }

        // Initialize Cross-Engine Bridge
        uevr::CrossEngineAPI* cross_engine_api = m_cross_engine_manager->GetCrossEngineAPI();
        if (!cross_engine_api) {
            spdlog::error("RE Engine Main Integration: Failed to get Cross-Engine API");
            return false;
        }

        if (!m_cross_engine_bridge->InitializeBridge(cross_engine_api)) {
            spdlog::error("RE Engine Main Integration: Failed to initialize cross-engine bridge");
            return false;
        }

        m_cross_engine_connected = true;
        spdlog::info("RE Engine Main Integration: Cross-Engine Bridge initialized");

        // Initialize Engine Adapter
        if (!m_engine_adapter->Initialize()) {
            spdlog::error("RE Engine Main Integration: Failed to initialize engine adapter");
            return false;
        }

        m_adapter_initialized = true;
        spdlog::info("RE Engine Main Integration: Engine Adapter initialized");

        m_main_integration_active = true;
        spdlog::info("RE Engine Main Integration: Main integration initialized successfully");

        return true;
    }

    // Activate Main Integration
    bool ActivateMainIntegration() {
        if (!m_main_integration_active) {
            spdlog::error("RE Engine Main Integration: Main integration not initialized");
            return false;
        }

        spdlog::info("RE Engine Main Integration: Activating main integration");

        // Activate Cross-Engine Bridge
        if (!m_cross_engine_bridge->ActivateCrossEngine()) {
            spdlog::error("RE Engine Main Integration: Failed to activate cross-engine bridge");
            return false;
        }

        // Activate Engine Adapter
        if (!m_engine_adapter->ActivateVRMode()) {
            spdlog::error("RE Engine Main Integration: Failed to activate engine adapter");
            return false;
        }

        // Enable Main Framework Features
        if (m_main_framework) {
            m_main_framework->EnableCrossEngineSupport();
        }

        spdlog::info("RE Engine Main Integration: Main integration activated successfully");

        return true;
    }

    // Process Main Integration Frame
    void ProcessMainIntegrationFrame() {
        if (!m_main_integration_active) {
            return;
        }

        // Process Cross-Engine Frame
        if (m_cross_engine_bridge) {
            m_cross_engine_bridge->ProcessCrossEngineFrame();
        }

        // Process Engine Adapter Frame
        if (m_engine_adapter) {
            m_engine_adapter->OnFrame();
        }

        // Update Main Integration Features
        UpdateMainIntegrationFeatures();
    }

    // Get Main Integration Status
    bool IsMainIntegrationActive() const {
        return m_main_integration_active && m_cross_engine_connected && m_adapter_initialized;
    }

    // Shutdown Main Integration
    void ShutdownMainIntegration() {
        spdlog::info("RE Engine Main Integration: Shutting down main integration");

        if (m_main_integration_active) {
            DeactivateMainIntegration();
        }

        // Cleanup Components
        if (m_cross_engine_bridge) {
            m_cross_engine_bridge->ShutdownBridge();
            delete m_cross_engine_bridge;
            m_cross_engine_bridge = nullptr;
        }

        if (m_engine_adapter) {
            m_engine_adapter->Shutdown();
            delete m_engine_adapter;
            m_engine_adapter = nullptr;
        }

        if (m_main_integration) {
            m_main_integration->Shutdown();
            delete m_main_integration;
            m_main_integration = nullptr;
        }

        // Reset State
        m_main_integration_active = false;
        m_cross_engine_connected = false;
        m_adapter_initialized = false;
        m_main_framework = nullptr;
        m_cross_engine_manager = nullptr;
    }

private:
    bool InitializeMainIntegrationFeatures() {
        spdlog::info("RE Engine Main Integration: Initializing main integration features");

        // Initialize Main Framework Connection
        m_features.main_framework_connected = m_main_framework->IsAvailable();
        
        // Initialize Cross-Engine Bridge Support
        m_features.cross_engine_bridge_active = m_cross_engine_manager->IsAvailable();
        
        // Initialize Adapter Plugin Support
        m_features.adapter_plugin_loaded = m_main_framework->SupportsAdapterPlugins();
        
        // Initialize VR Systems
        m_features.vr_systems_initialized = m_main_framework->AreVRSystemsAvailable();
        
        // Initialize Performance Monitoring
        m_features.performance_monitoring_active = m_main_framework->SupportsPerformanceMonitoring();

        spdlog::info("RE Engine Main Integration: Features initialized - Main: {}, Cross-Engine: {}, Adapter: {}, VR: {}, Performance: {}",
            m_features.main_framework_connected,
            m_features.cross_engine_bridge_active,
            m_features.adapter_plugin_loaded,
            m_features.vr_systems_initialized,
            m_features.performance_monitoring_active
        );

        return true;
    }

    void UpdateMainIntegrationFeatures() {
        // Update main integration feature states
        if (m_main_framework) {
            m_main_framework->UpdateIntegrationFeatures();
        }
    }

    void DeactivateMainIntegration() {
        spdlog::info("RE Engine Main Integration: Deactivating main integration");

        // Disable Main Framework Features
        if (m_main_framework) {
            m_main_framework->DisableCrossEngineSupport();
        }

        // Deactivate Engine Adapter
        if (m_engine_adapter) {
            m_engine_adapter->DeactivateVRMode();
        }

        m_main_integration_active = false;
    }
};

// Global Main Integration Instance
static RE_Engine_Main_Integration* g_main_integration = nullptr;

// Main Integration Interface Functions
extern "C" {
    __declspec(dllexport) bool InitializeREMainIntegration(uevr::MainFramework* framework) {
        if (g_main_integration) {
            delete g_main_integration;
        }
        
        g_main_integration = new RE_Engine_Main_Integration();
        return g_main_integration->InitializeMainIntegration(framework);
    }

    __declspec(dllexport) bool ActivateREMainIntegration() {
        if (g_main_integration) {
            return g_main_integration->ActivateMainIntegration();
        }
        return false;
    }

    __declspec(dllexport) void ProcessREMainIntegrationFrame() {
        if (g_main_integration) {
            g_main_integration->ProcessMainIntegrationFrame();
        }
    }

    __declspec(dllexport) bool IsREMainIntegrationActive() {
        if (g_main_integration) {
            return g_main_integration->IsMainIntegrationActive();
        }
        return false;
    }

    __declspec(dllexport) void ShutdownREMainIntegration() {
        if (g_main_integration) {
            g_main_integration->ShutdownMainIntegration();
            delete g_main_integration;
            g_main_integration = nullptr;
        }
    }
}
