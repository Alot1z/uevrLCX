#include "RE_Engine_Adapter.hpp"
#include "RE_Engine_Hooks.hpp"
#include "engine_hooks.hpp"
#include <uevr/Plugin.hpp>
#include <uevr/API.hpp>
#include <spdlog/spdlog.h>
#include <MinHook.h>

// Main RE Engine Integration with uevr Cross-Engine Framework
class RE_Engine_Integration : public uevr::Plugin {
private:
    RE_Engine_Adapter* m_adapter;
    bool m_hooks_installed;
    bool m_vr_mode_active;
    
    // uevr Integration Components
    uevr::API* m_uevr_api;
    uevr::VRSystem* m_vr_system;
    uevr::RenderingSystem* m_rendering_system;
    uevr::InputSystem* m_input_system;

public:
    RE_Engine_Integration() : 
        m_adapter(nullptr), 
        m_hooks_installed(false), 
        m_vr_mode_active(false),
        m_uevr_api(nullptr),
        m_vr_system(nullptr),
        m_rendering_system(nullptr),
        m_input_system(nullptr) {
        
        spdlog::info("RE Engine Integration: Initializing with uevr Cross-Engine Framework");
    }

    virtual ~RE_Engine_Integration() {
        CleanupIntegration();
    }

    // uevr Plugin Interface Implementation
    virtual bool Initialize(uevr::API* api) override {
        m_uevr_api = api;
        if (!m_uevr_api) {
            spdlog::error("RE Engine Integration: Failed to get uevr API");
            return false;
        }

        spdlog::info("RE Engine Integration: uevr API acquired successfully");

        // Initialize uevr Systems
        m_vr_system = m_uevr_api->GetVRSystem();
        m_rendering_system = m_uevr_api->GetRenderingSystem();
        m_input_system = m_uevr_api->GetInputSystem();

        if (!m_vr_system || !m_rendering_system || !m_input_system) {
            spdlog::error("RE Engine Integration: Failed to get uevr systems");
            return false;
        }

        // Create RE Engine Adapter
        m_adapter = new RE_Engine_Adapter();
        if (!m_adapter) {
            spdlog::error("RE Engine Integration: Failed to create RE Engine adapter");
            return false;
        }

        // Initialize RE Engine Adapter with uevr
        if (!m_adapter->InitializeWithuevr(m_uevr_api)) {
            spdlog::error("RE Engine Integration: Failed to initialize adapter with uevr");
            return false;
        }

        spdlog::info("RE Engine Integration: Adapter initialized with uevr successfully");

        // Install Engine Hooks
        if (!InstallEngineHooks()) {
            spdlog::error("RE Engine Integration: Failed to install engine hooks");
            return false;
        }

        // Install D3D11 Hooks
        if (!InitializeHooks()) {
            spdlog::error("RE Engine Integration: Failed to install D3D11 hooks");
            return false;
        }

        m_hooks_installed = true;
        spdlog::info("RE Engine Integration: All hooks installed successfully");

        return true;
    }

    virtual void OnFrame() override {
        if (!m_adapter || !m_vr_mode_active) {
            return;
        }

        // Process VR Frame through uevr
        if (m_vr_system && m_vr_system->IsVRModeActive()) {
            m_adapter->ProcessVRFrame(m_vr_system, m_rendering_system, m_input_system);
        }
    }

    virtual void OnVRModeChanged(bool enabled) override {
        m_vr_mode_active = enabled;
        spdlog::info("RE Engine Integration: VR Mode changed to {}", enabled ? "enabled" : "disabled");

        if (enabled) {
            // Activate RE Engine VR Mode
            if (m_adapter) {
                m_adapter->ActivateVRMode();
            }
            
            // Enable uevr VR Systems
            if (m_vr_system) {
                m_vr_system->EnableVRMode();
            }
        } else {
            // Deactivate RE Engine VR Mode
            if (m_adapter) {
                m_adapter->DeactivateVRMode();
            }
            
            // Disable uevr VR Systems
            if (m_vr_system) {
                m_vr_system->DisableVRMode();
            }
        }
    }

    virtual void Shutdown() override {
        CleanupIntegration();
        spdlog::info("RE Engine Integration: Shutdown complete");
    }

private:
    bool InstallEngineHooks() {
        spdlog::info("RE Engine Integration: Installing engine hooks");
        
        // Install RE Engine specific hooks
        if (!::InstallEngineHooks()) {
            spdlog::error("RE Engine Integration: Failed to install engine hooks");
            return false;
        }

        return true;
    }

    void CleanupIntegration() {
        spdlog::info("RE Engine Integration: Cleaning up integration");

        // Cleanup hooks
        if (m_hooks_installed) {
            ::CleanupEngineHooks();
            CleanupHooks();
            m_hooks_installed = false;
        }

        // Cleanup adapter
        if (m_adapter) {
            m_adapter->Shutdown();
            delete m_adapter;
            m_adapter = nullptr;
        }

        // Reset uevr references
        m_uevr_api = nullptr;
        m_vr_system = nullptr;
        m_rendering_system = nullptr;
        m_input_system = nullptr;
    }
};

// uevr Plugin Export
extern "C" __declspec(dllexport) uevr::Plugin* CreatePlugin() {
    return new RE_Engine_Integration();
}
