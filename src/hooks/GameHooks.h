#pragma once

#include <string>
#include <vector>

namespace UEVR {
namespace Hooks {

// Game hook management
bool initializeGameHooks();
void cleanupGameHooks();
bool installEngineHooks(const std::string& engine_name);
bool uninstallEngineHooks();

// Matrix hook management
bool hookViewMatrix();
bool hookProjectionMatrix();
bool hookViewProjectionMatrix();
bool injectVRMatrices();

// Rendering hooks
bool hookSwapchainPresent();
bool hookDrawCall();
bool hookVertexShader();
bool hookPixelShader();

// Input hooks
bool hookGameInput();
bool injectVRInput();
bool overrideGameInput();

// UObjectHook system
bool initializeUObjectHooks();
bool scanForGameObjects();
bool attachObjectToController(const std::string& object_path, int controller);
bool detachObjectFromController(const std::string& object_path);

// Hook validation
bool validateHooks();
bool testHookFunctionality();
std::vector<std::string> getActiveHooks();

// Performance monitoring
bool enableHookProfiling();
bool getHookPerformanceStats();

} // namespace Hooks
} // namespace UEVR


