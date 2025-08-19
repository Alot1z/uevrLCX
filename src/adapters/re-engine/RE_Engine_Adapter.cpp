/*
RE Engine Cross-Engine VR Adapter for uevr
Copyright (c) 2024 uevr Cross-Engine Development Team

This adapter extends uevr's plugin architecture to support Capcom's RE Engine,
enabling VR support for Resident Evil 7 and other RE Engine titles.

Based on uevr Plugin API: SOURCECODE/uevr/include/uevr/Plugin.hpp
*/

#include "RE_Engine_Adapter.hpp"
#include "RE_Engine_Hooks.hpp"
#include "engine_hooks.hpp"
#include <d3d11.h>
#include <dxgi.h>
#include <MinHook.h>
#include <spdlog/spdlog.h>
#include <uevr/Plugin.hpp>
#include <uevr/API.hpp>
#include <DirectXMath.h>

using namespace DirectX;

// Global adapter instance
static REEngineAdapter* g_adapter = nullptr;

// RE Engine specific data structures
RE_Engine_Camera_Data g_cameraData = {};
RE_Engine_Render_State g_renderState = {};
RE_Engine_Matrix_Data g_matrixData = {};
RE_Engine_VR_Config g_vrConfig = {};

// uevr integration
uevr::API* g_uevrAPI = nullptr;
uevr::Plugin* g_uevrPlugin = nullptr;

// Hook management
bool g_hooksInstalled = false;
bool g_vrModeEnabled = false;

// RE Engine detection
bool g_reEngineDetected = false;
HMODULE g_reEngineModule = nullptr;

// Forward declarations
bool InitializeREEngineHooks();
bool InitializeuevrIntegration();
void CleanupREEngineHooks();
void UpdateVRMatrices();
void ProcessVRInput();

// RE Engine adapter implementation
class REEngineAdapter : public uevr::Plugin
{
public:
    REEngineAdapter() = default;
    ~REEngineAdapter() = default;

    // uevr Plugin interface implementation
    bool Initialize(uevr::API* api) override
    {
        spdlog::info("RE Engine Adapter: Initializing...");
        
        g_uevrAPI = api;
        g_uevrPlugin = this;
        
        // Initialize RE Engine detection
        if (!DetectREEngine())
        {
            spdlog::error("RE Engine Adapter: Failed to detect RE Engine");
            return false;
        }
        
        // Initialize engine hooks
        if (!InitializeREEngineHooks())
        {
            spdlog::error("RE Engine Adapter: Failed to initialize engine hooks");
            return false;
        }
        
        // Initialize uevr integration
        if (!InitializeuevrIntegration())
        {
            spdlog::error("RE Engine Adapter: Failed to initialize uevr integration");
            return false;
        }
        
        spdlog::info("RE Engine Adapter: Initialization complete");
        return true;
    }

    void Cleanup() override
    {
        spdlog::info("RE Engine Adapter: Cleaning up...");
        
        CleanupREEngineHooks();
        g_uevrAPI = nullptr;
        g_uevrPlugin = nullptr;
        
        spdlog::info("RE Engine Adapter: Cleanup complete");
    }

    void OnFrame() override
    {
        if (!g_vrModeEnabled) return;
        
        // Update VR matrices and process input
        UpdateVRMatrices();
        ProcessVRInput();
    }

    void EnableVRMode(bool enable) override
    {
        g_vrModeEnabled = enable;
        spdlog::info("RE Engine Adapter: VR mode {}", enable ? "enabled" : "disabled");
        
        if (enable)
        {
            // Initialize VR-specific systems
            InitializeVRSystems();
        }
        else
        {
            // Cleanup VR-specific systems
            CleanupVRSystems();
        }
    }

private:
    bool DetectREEngine()
    {
        // Try to detect RE Engine by looking for characteristic modules
        HMODULE modules[1024];
        DWORD cbNeeded;
        
        if (EnumProcessModules(GetCurrentProcess(), modules, sizeof(modules), &cbNeeded))
        {
            for (unsigned int i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
            {
                TCHAR szModName[MAX_PATH];
                if (GetModuleBaseName(GetCurrentProcess(), modules[i], szModName, sizeof(szModName) / sizeof(TCHAR)))
                {
                    std::wstring moduleName(szModName);
                    if (moduleName.find(L"RE") != std::wstring::npos || 
                        moduleName.find(L"Capcom") != std::wstring::npos)
                    {
                        g_reEngineModule = modules[i];
                        g_reEngineDetected = true;
                        spdlog::info("RE Engine Adapter: RE Engine detected in module: {}", std::string(moduleName.begin(), moduleName.end()));
                        return true;
                    }
                }
            }
        }
        
        // Check for RE8.exe (Resident Evil 8 Village)
        if (GetModuleHandle(L"re8.exe"))
        {
            g_reEngineDetected = true;
            spdlog::info("RE Engine Adapter: RE8 Village detected via executable name");
            return true;
        }
        
        // Check for re7.exe (Resident Evil 7)
        if (GetModuleHandle(L"re7.exe"))
        {
            g_reEngineDetected = true;
            spdlog::info("RE Engine Adapter: RE7 detected via executable name");
            return true;
        }
        
        return false;
    }

    bool InitializeVRSystems()
    {
        // Initialize VR camera system
        g_cameraData.world_scale = 1.0f;
        g_cameraData.decoupled_pitch = true;
        g_cameraData.camera_forward_offset = 0.0f;
        g_cameraData.camera_right_offset = 0.0f;
        g_cameraData.camera_up_offset = 0.0f;
        
        // Initialize VR rendering state
        g_renderState.stereo_rendering = true;
        g_renderState.vr_overlay_mode = true;
        g_renderState.performance_optimization = true;
        
        // Initialize VR matrices
        g_matrixData.view_matrix = XMMatrixIdentity();
        g_matrixData.projection_matrix = XMMatrixIdentity();
        g_matrixData.world_matrix = XMMatrixIdentity();
        
        // Initialize VR configuration
        g_vrConfig.enable_vr_mode = true;
        g_vrConfig.enable_stereo_rendering = true;
        g_vrConfig.enable_vr_controllers = true;
        g_vrConfig.vr_target_framerate = 90;
        g_vrConfig.vr_resolution_scale = 1.0f;
        g_vrConfig.weapon_to_right_controller = true;
        g_vrConfig.utility_to_left_controller = true;
        
        spdlog::info("RE Engine Adapter: VR systems initialized");
        return true;
    }

    void CleanupVRSystems()
    {
        // Reset VR state
        g_vrModeEnabled = false;
        g_renderState.stereo_rendering = false;
        g_vrConfig.enable_vr_mode = false;
        
        spdlog::info("RE Engine Adapter: VR systems cleaned up");
    }
};

// Global adapter instance management
extern "C" __declspec(dllexport) uevr::Plugin* CreatePlugin()
{
    if (!g_adapter)
    {
        g_adapter = new REEngineAdapter();
    }
    return g_adapter;
}

extern "C" __declspec(dllexport) void DestroyPlugin(uevr::Plugin* plugin)
{
    if (g_adapter && g_adapter == plugin)
    {
        delete g_adapter;
        g_adapter = nullptr;
    }
}

// RE Engine hook initialization
bool InitializeREEngineHooks()
{
    if (g_hooksInstalled) return true;
    
    spdlog::info("RE Engine Adapter: Installing RE Engine hooks...");
    
    // Initialize MinHook
    if (MH_Initialize() != MH_OK)
    {
        spdlog::error("RE Engine Adapter: Failed to initialize MinHook");
        return false;
    }
    
    // Install D3D11 hooks
    if (!InstallD3D11Hooks())
    {
        spdlog::error("RE Engine Adapter: Failed to install D3D11 hooks");
        return false;
    }
    
    // Install engine-specific hooks
    if (!InstallEngineHooks())
    {
        spdlog::error("RE Engine Adapter: Failed to install engine hooks");
        return false;
    }
    
    // Enable all hooks
    if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK)
    {
        spdlog::error("RE Engine Adapter: Failed to enable hooks");
        return false;
    }
    
    g_hooksInstalled = true;
    spdlog::info("RE Engine Adapter: RE Engine hooks installed successfully");
    return true;
}

// uevr integration initialization
bool InitializeuevrIntegration()
{
    if (!g_uevrAPI) return false;
    
    spdlog::info("RE Engine Adapter: Initializing uevr integration...");
    
    // Register with uevr plugin system
    if (!g_uevrAPI->RegisterPlugin(this))
    {
        spdlog::error("RE Engine Adapter: Failed to register with uevr");
        return false;
    }
    
    // Initialize VR mode
    EnableVRMode(true);
    
    spdlog::info("RE Engine Adapter: uevr integration initialized successfully");
    return true;
}

// Hook cleanup
void CleanupREEngineHooks()
{
    if (!g_hooksInstalled) return;
    
    spdlog::info("RE Engine Adapter: Cleaning up RE Engine hooks...");
    
    // Disable all hooks
    MH_DisableHook(MH_ALL_HOOKS);
    
    // Uninstall hooks
    MH_Uninitialize();
    
    g_hooksInstalled = false;
    spdlog::info("RE Engine Adapter: RE Engine hooks cleaned up");
}

// VR matrix updates
void UpdateVRMatrices()
{
    if (!g_vrModeEnabled || !g_uevrAPI) return;
    
    // Get VR headset pose from uevr
    uevr::HeadsetPose pose;
    if (g_uevrAPI->GetHeadsetPose(pose))
    {
        // Update view matrix with VR head tracking
        XMMATRIX headMatrix = XMMatrixRotationRollPitchYaw(
            pose.pitch, pose.yaw, pose.roll
        );
        
        // Apply head position offset
        XMMATRIX positionMatrix = XMMatrixTranslation(
            pose.x, pose.y, pose.z
        );
        
        // Combine matrices
        g_matrixData.view_matrix = headMatrix * positionMatrix;
        
        // Update projection matrix for stereo rendering
        float aspectRatio = 16.0f / 9.0f; // RE7 default aspect ratio
        float fov = XMConvertToRadians(90.0f); // VR-optimized FOV
        
        g_matrixData.projection_matrix = XMMatrixPerspectiveFovLH(
            fov, aspectRatio, 0.01f, 1000.0f
        );
    }
}

// VR input processing
void ProcessVRInput()
{
    if (!g_vrModeEnabled || !g_uevrAPI) return;
    
    // Get motion controller states from uevr
    uevr::MotionControllerState leftController, rightController;
    
    if (g_uevrAPI->GetMotionControllerState(0, leftController)) // Left controller
    {
        // Process utility/left controller input
        ProcessUtilityInput(leftController);
    }
    
    if (g_uevrAPI->GetMotionControllerState(1, rightController)) // Right controller
    {
        // Process weapon/right controller input
        ProcessWeaponInput(rightController);
    }
}

// Utility input processing (left controller)
void ProcessUtilityInput(const uevr::MotionControllerState& controller)
{
    // Map utility actions to left controller
    if (controller.primaryButtonPressed)
    {
        // Inventory open
        SendInputToREEngine(RE_INPUT_INVENTORY_OPEN);
    }
    
    if (controller.secondaryButtonPressed)
    {
        // Map open
        SendInputToREEngine(RE_INPUT_MAP_OPEN);
    }
    
    if (controller.gripPressed)
    {
        // Movement grip
        SendInputToREEngine(RE_INPUT_MOVEMENT_GRIP);
    }
    
    // Process joystick movement
    if (abs(controller.joystickX) > 0.2f || abs(controller.joystickY) > 0.2f)
    {
        SendInputToREEngine(RE_INPUT_MOVEMENT, controller.joystickX, controller.joystickY);
    }
}

// Weapon input processing (right controller)
void ProcessWeaponInput(const uevr::MotionControllerState& controller)
{
    // Map weapon actions to right controller
    if (controller.triggerPressed)
    {
        // Weapon fire
        SendInputToREEngine(RE_INPUT_WEAPON_FIRE);
    }
    
    if (controller.primaryButtonPressed)
    {
        // Weapon reload
        SendInputToREEngine(RE_INPUT_WEAPON_RELOAD);
    }
    
    if (controller.gripPressed)
    {
        // Weapon grip
        SendInputToREEngine(RE_INPUT_WEAPON_GRIP);
    }
    
    // Process aiming (right controller)
    if (controller.aimPoseValid)
    {
        // Update weapon aim based on controller pose
        UpdateWeaponAim(controller.aimPose);
    }
}

// Send input to RE Engine
void SendInputToREEngine(RE_Input_Type inputType, float x, float y)
{
    // This function would interface with RE Engine's input system
    // For now, we log the input for debugging
    switch (inputType)
    {
        case RE_INPUT_WEAPON_FIRE:
            spdlog::debug("RE Engine Adapter: Weapon fire input");
            break;
        case RE_INPUT_WEAPON_RELOAD:
            spdlog::debug("RE Engine Adapter: Weapon reload input");
            break;
        case RE_INPUT_WEAPON_GRIP:
            spdlog::debug("RE Engine Adapter: Weapon grip input");
            break;
        case RE_INPUT_INVENTORY_OPEN:
            spdlog::debug("RE Engine Adapter: Inventory open input");
            break;
        case RE_INPUT_MAP_OPEN:
            spdlog::debug("RE Engine Adapter: Map open input");
            break;
        case RE_INPUT_MOVEMENT_GRIP:
            spdlog::debug("RE Engine Adapter: Movement grip input");
            break;
        case RE_INPUT_MOVEMENT:
            spdlog::debug("RE Engine Adapter: Movement input X:{} Y:{}", x, y);
            break;
        default:
            spdlog::debug("RE Engine Adapter: Unknown input type: {}", inputType);
            break;
    }
}

// Update weapon aim based on controller pose
void UpdateWeaponAim(const uevr::Pose& aimPose)
{
    // Convert controller pose to weapon aim direction
    // This would update the weapon's orientation in RE Engine
    
    // For now, we log the aim update
    spdlog::debug("RE Engine Adapter: Weapon aim updated - Pitch:{} Yaw:{} Roll:{}", 
                  aimPose.pitch, aimPose.yaw, aimPose.roll);
}

// DLL entry point
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
            spdlog::info("RE Engine Adapter: DLL loaded");
            break;
            
        case DLL_PROCESS_DETACH:
            spdlog::info("RE Engine Adapter: DLL unloaded");
            if (g_adapter)
            {
                g_adapter->Cleanup();
            }
            break;
    }
    return TRUE;
}
