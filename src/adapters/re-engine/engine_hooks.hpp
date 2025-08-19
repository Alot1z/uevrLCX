#pragma once

#include <windows.h>
#include <cstdint>
#include <DirectXMath.h>

using namespace DirectX;

// RE Engine input types
enum RE_Input_Type
{
    RE_INPUT_WEAPON_FIRE,
    RE_INPUT_WEAPON_RELOAD,
    RE_INPUT_WEAPON_GRIP,
    RE_INPUT_INVENTORY_OPEN,
    RE_INPUT_MAP_OPEN,
    RE_INPUT_MOVEMENT_GRIP,
    RE_INPUT_MOVEMENT
};

// RE Engine engine state
struct RE_Engine_State
{
    bool rendering_active;
    bool camera_updated;
    bool input_processed;
    float frame_time;
    uint64_t frame_count;
};

// Forward declarations
bool InstallEngineHooks();
void CleanupEngineHooks();
bool IsREEngineActive();
bool IsRenderingActive();
bool IsCameraUpdated();
bool IsInputProcessed();
uint64_t GetFrameCount();
float GetFrameTime();
void UpdatePerformanceMetrics();

// VR processing functions
void ProcessVRPostRender(void* renderContext);
void ProcessVRPostCameraUpdate(void* cameraData);
void ProcessVRPostInput(void* inputData);

// VR-specific processing
void ModifyRenderContextForVR(void* renderContext);
void ApplyVRRenderingOptimizations(void* renderContext);
void ExtractCameraMatrices(void* cameraData);
void ApplyVRHeadTracking(void* cameraData);
void UpdateVRComfortSettings(void* cameraData);
void ProcessMotionControllerInput(void* inputData);
void ApplyVRInputMapping(void* inputData);
void UpdateHapticFeedback(void* inputData);

// Hook installation helpers
bool HookRenderingFunction();
bool HookCameraFunction();
bool HookInputFunction();

// External variables (defined in engine_hooks.cpp)
extern bool g_engineHooksInstalled;
extern RE_Engine_State g_engineState;
extern bool g_vrModeEnabled;
