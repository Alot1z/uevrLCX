#pragma once

#include <uevr/Plugin.hpp>
#include <uevr/API.hpp>
#include <memory>

// Forward declarations
class RE_Engine_Adapter;
class uevr::VRSystem;
class uevr::RenderingSystem;
class uevr::InputSystem;

// Main RE Engine Integration Class
// This class serves as the bridge between RE Engine and uevr's main cross-engine framework
class RE_Engine_Integration : public uevr::Plugin {
private:
    RE_Engine_Adapter* m_adapter;
    bool m_hooks_installed;
    bool m_vr_mode_active;
    
    // uevr Main Framework Integration Components
    uevr::API* m_uevr_api;
    uevr::VRSystem* m_vr_system;
    uevr::RenderingSystem* m_rendering_system;
    uevr::InputSystem* m_input_system;

public:
    RE_Engine_Integration();
    virtual ~RE_Engine_Integration();

    // uevr Plugin Interface Implementation
    virtual bool Initialize(uevr::API* api) override;
    virtual void OnFrame() override;
    virtual void OnVRModeChanged(bool enabled) override;
    virtual void Shutdown() override;

private:
    bool InstallEngineHooks();
    void CleanupIntegration();
};

// uevr Plugin Export Function
extern "C" __declspec(dllexport) uevr::Plugin* CreatePlugin();
