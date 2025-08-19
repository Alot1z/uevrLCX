#pragma once

#include <string>

namespace UEVR {
namespace VR {

// VR system management
bool initializeVRSystem();
void cleanupVRSystem();
bool activateVRMode();
bool deactivateVRMode();
bool isVRModeActive();

// VR runtime management
bool initializeOpenXR();
bool initializeOpenVR();
bool initializeSteamVR();
std::string getCurrentVRRuntime();

// VR device management
bool detectVRHeadset();
std::string getHeadsetName();
bool detectVRControllers();
int getControllerCount();

// VR rendering
bool initializeVRStereoRendering();
bool setVRViewport(int width, int height);
bool submitVRFrame();
bool presentVRFrame();

// VR input
bool initializeVRInput();
bool pollVRInput();
bool getControllerPose(int controller, float* position, float* rotation);
bool getControllerButtonState(int controller, int button);

// VR performance
bool enableAsyncReprojection();
bool enableMotionSmoothing();
bool setVRPerformanceMode(const std::string& mode);

} // namespace VR
} // namespace UEVR


