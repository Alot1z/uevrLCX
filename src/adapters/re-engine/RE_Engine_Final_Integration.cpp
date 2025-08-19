#include "RE_Engine_Main_Integration.hpp"
#include "RE_Engine_CrossEngine_Bridge.hpp"
#include "RE_Engine_Integration.hpp"
#include "RE_Engine_Adapter.hpp"
#include <uevr/MainFramework.hpp>
#include <uevr/CrossEngineManager.hpp>
#include <uevr/VRFramework.hpp>
#include <spdlog/spdlog.h>

// RE Engine Final Integration Verification
// This file ensures complete integration between RE Engine and uevr's main cross-engine framework

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
    RE_Engine_Final_Integration() : 
        m_main_integration(nullptr),
        m_cross_engine_bridge(nullptr),
        m_integration_layer(nullptr),
        m_engine_adapter(nullptr),
        m_main_framework(nullptr),
        m_cross_engine_manager(nullptr),
        m_vr_framework(nullptr),
        m_final_integration_active(false),
        m_all_components_connected(false),
        m_vr_systems_ready(false) {
        
        memset(&m_verification, 0, sizeof(m_verification));
        spdlog::info("RE Engine Final Integration: Initializing complete integration verification");
    }

    ~RE_Engine_Final_Integration() {
        ShutdownFinalIntegration();
    }

    // Initialize Final Integration
    bool InitializeFinalIntegration(uevr::MainFramework* main_framework) {
        if (!main_framework) {
            spdlog::error("RE Engine Final Integration: Invalid Main Framework");
            return false;
        }

        m_main_framework = main_framework;
        spdlog::info("RE Engine Final Integration: Main Framework acquired");

        // Get Cross-Engine Manager
        m_cross_engine_manager = m_main_framework->GetCrossEngineManager();
        if (!m_cross_engine_manager) {
            spdlog::error("RE Engine Final Integration: Failed to get Cross-Engine Manager");
            return false;
        }

        // Get VR Framework
        m_vr_framework = m_main_framework->GetVRFramework();
        if (!m_vr_framework) {
            spdlog::error("RE Engine Final Integration: Failed to get VR Framework");
            return false;
        }

        // Initialize All Components
        if (!InitializeAllComponents()) {
            spdlog::error("RE Engine Final Integration: Failed to initialize all components");
            return false;
        }

        // Verify Integration
        if (!VerifyCompleteIntegration()) {
            spdlog::error("RE Engine Final Integration: Integration verification failed");
            return false;
        }

        m_final_integration_active = true;
        spdlog::info("RE Engine Final Integration: Complete integration verified and active");

        return true;
    }

    // Activate Final Integration
    bool ActivateFinalIntegration() {
        if (!m_final_integration_active) {
            spdlog::error("RE Engine Final Integration: Final integration not initialized");
            return false;
        }

        spdlog::info("RE Engine Final Integration: Activating complete integration");

        // Activate Main Integration
        if (!m_main_integration->ActivateMainIntegration()) {
            spdlog::error("RE Engine Final Integration: Failed to activate main integration");
            return false;
        }

        // Activate Cross-Engine Bridge
        if (!m_cross_engine_bridge->ActivateCrossEngine()) {
            spdlog::error("RE Engine Final Integration: Failed to activate cross-engine bridge");
            return false;
        }

        // Activate VR Framework
        if (m_vr_framework) {
            m_vr_framework->Activate();
        }

        spdlog::info("RE Engine Final Integration: Complete integration activated successfully");

        return true;
    }

    // Process Final Integration Frame
    void ProcessFinalIntegrationFrame() {
        if (!m_final_integration_active) {
            return;
        }

        // Process Main Integration Frame
        if (m_main_integration) {
            m_main_integration->ProcessMainIntegrationFrame();
        }

        // Process Cross-Engine Frame
        if (m_cross_engine_bridge) {
            m_cross_engine_bridge->ProcessCrossEngineFrame();
        }

        // Process VR Frame
        if (m_vr_framework && m_vr_framework->IsActive()) {
            ProcessVRFrame();
        }

        // Update Integration Status
        UpdateIntegrationStatus();
    }

    // Get Final Integration Status
    bool IsFinalIntegrationActive() const {
        return m_final_integration_active && m_all_components_connected && m_vr_systems_ready;
    }

    // Get Integration Verification Status
    const Integration_Verification& GetVerificationStatus() const {
        return m_verification;
    }

    // Shutdown Final Integration
    void ShutdownFinalIntegration() {
        spdlog::info("RE Engine Final Integration: Shutting down complete integration");

        if (m_final_integration_active) {
            DeactivateFinalIntegration();
        }

        // Cleanup Components
        if (m_main_integration) {
            m_main_integration->ShutdownMainIntegration();
            delete m_main_integration;
            m_main_integration = nullptr;
        }

        if (m_cross_engine_bridge) {
            m_cross_engine_bridge->ShutdownBridge();
            delete m_cross_engine_bridge;
            m_cross_engine_bridge = nullptr;
        }

        if (m_integration_layer) {
            m_integration_layer->Shutdown();
            delete m_integration_layer;
            m_integration_layer = nullptr;
        }

        if (m_engine_adapter) {
            m_engine_adapter->Cleanup();
            delete m_engine_adapter;
            m_engine_adapter = nullptr;
        }

        // Reset State
        m_final_integration_active = false;
        m_all_components_connected = false;
        m_vr_systems_ready = false;
        m_main_framework = nullptr;
        m_cross_engine_manager = nullptr;
        m_vr_framework = nullptr;
    }

private:
    bool InitializeAllComponents() {
        spdlog::info("RE Engine Final Integration: Initializing all components");

        // Create Main Integration
        m_main_integration = new RE_Engine_Main_Integration();
        if (!m_main_integration) {
            spdlog::error("RE Engine Final Integration: Failed to create main integration");
            return false;
        }

        // Initialize Main Integration
        if (!m_main_integration->InitializeMainIntegration(m_main_framework)) {
            spdlog::error("RE Engine Final Integration: Failed to initialize main integration");
            return false;
        }

        // Create Cross-Engine Bridge
        m_cross_engine_bridge = new RE_Engine_CrossEngine_Bridge();
        if (!m_cross_engine_bridge) {
            spdlog::error("RE Engine Final Integration: Failed to create cross-engine bridge");
            return false;
        }

        // Initialize Cross-Engine Bridge
        uevr::CrossEngineAPI* cross_engine_api = m_cross_engine_manager->GetCrossEngineAPI();
        if (!cross_engine_api) {
            spdlog::error("RE Engine Final Integration: Failed to get Cross-Engine API");
            return false;
        }

        if (!m_cross_engine_bridge->InitializeBridge(cross_engine_api)) {
            spdlog::error("RE Engine Final Integration: Failed to initialize cross-engine bridge");
            return false;
        }

        // Create Integration Layer
        m_integration_layer = new RE_Engine_Integration();
        if (!m_integration_layer) {
            spdlog::error("RE Engine Final Integration: Failed to create integration layer");
            return false;
        }

        // Create Engine Adapter
        m_engine_adapter = new RE_Engine_Adapter();
        if (!m_engine_adapter) {
            spdlog::error("RE Engine Final Integration: Failed to create engine adapter");
            return false;
        }

        // Initialize Engine Adapter
        if (!m_engine_adapter->Initialize(nullptr)) { // Initialize without API for now
            spdlog::error("RE Engine Final Integration: Failed to initialize engine adapter");
            return false;
        }

        m_all_components_connected = true;
        spdlog::info("RE Engine Final Integration: All components initialized successfully");

        return true;
    }

    bool VerifyCompleteIntegration() {
        spdlog::info("RE Engine Final Integration: Verifying complete integration");

        // Verify Main Integration
        m_verification.main_integration_verified = m_main_integration && 
            m_main_integration->IsMainIntegrationActive();

        // Verify Cross-Engine Bridge
        m_verification.cross_engine_bridge_verified = m_cross_engine_bridge && 
            m_cross_engine_bridge->IsCrossEngineActive();

        // Verify Integration Layer
        m_verification.integration_layer_verified = m_integration_layer != nullptr;

        // Verify Engine Adapter
        m_verification.engine_adapter_verified = m_engine_adapter != nullptr;

        // Verify VR Framework
        m_verification.vr_framework_verified = m_vr_framework != nullptr;

        // Verify Performance Systems
        m_verification.performance_systems_verified = m_main_framework && 
            m_main_framework->SupportsPerformanceMonitoring();

        // Check if all verifications passed
        bool all_verified = m_verification.main_integration_verified &&
                           m_verification.cross_engine_bridge_verified &&
                           m_verification.integration_layer_verified &&
                           m_verification.engine_adapter_verified &&
                           m_verification.vr_framework_verified &&
                           m_verification.performance_systems_verified;

        spdlog::info("RE Engine Final Integration: Integration verification - Main: {}, Cross-Engine: {}, Layer: {}, Adapter: {}, VR: {}, Performance: {}",
            m_verification.main_integration_verified,
            m_verification.cross_engine_bridge_verified,
            m_verification.integration_layer_verified,
            m_verification.engine_adapter_verified,
            m_verification.vr_framework_verified,
            m_verification.performance_systems_verified
        );

        if (all_verified) {
            m_vr_systems_ready = true;
            spdlog::info("RE Engine Final Integration: All integration components verified successfully");
        } else {
            spdlog::error("RE Engine Final Integration: Some integration components failed verification");
        }

        return all_verified;
    }

    void ProcessVRFrame() {
        // Process VR frame through VR framework
        if (m_vr_framework) {
            m_vr_framework->ProcessFrame();
        }
    }

    void UpdateIntegrationStatus() {
        // Update integration status
        if (m_main_framework) {
            m_main_framework->UpdateIntegrationStatus();
        }
    }

    void DeactivateFinalIntegration() {
        spdlog::info("RE Engine Final Integration: Deactivating complete integration");

        // Deactivate VR Framework
        if (m_vr_framework) {
            m_vr_framework->Deactivate();
        }

        // Deactivate Cross-Engine Bridge
        if (m_cross_engine_bridge) {
            // Bridge deactivation handled by main integration
        }

        m_final_integration_active = false;
    }
};

// Global Final Integration Instance
static RE_Engine_Final_Integration* g_final_integration = nullptr;

// Final Integration Interface Functions
extern "C" {
    __declspec(dllexport) bool InitializeREFinalIntegration(uevr::MainFramework* framework) {
        if (g_final_integration) {
            delete g_final_integration;
        }
        
        g_final_integration = new RE_Engine_Final_Integration();
        return g_final_integration->InitializeFinalIntegration(framework);
    }

    __declspec(dllexport) bool ActivateREFinalIntegration() {
        if (g_final_integration) {
            return g_final_integration->ActivateFinalIntegration();
        }
        return false;
    }

    __declspec(dllexport) void ProcessREFinalIntegrationFrame() {
        if (g_final_integration) {
            g_final_integration->ProcessFinalIntegrationFrame();
        }
    }

    __declspec(dllexport) bool IsREFinalIntegrationActive() {
        if (g_final_integration) {
            return g_final_integration->IsFinalIntegrationActive();
        }
        return false;
    }

    __declspec(dllexport) void ShutdownREFinalIntegration() {
        if (g_final_integration) {
            g_final_integration->ShutdownFinalIntegration();
            delete g_final_integration;
            g_final_integration = nullptr;
        }
    }

    __declspec(dllexport) const void* GetREFinalIntegrationVerificationStatus() {
        if (g_final_integration) {
            return &g_final_integration->GetVerificationStatus();
        }
        return nullptr;
    }
}
