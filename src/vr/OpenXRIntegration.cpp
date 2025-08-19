#include <uevr/vr/OpenXRIntegration.hpp>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <cstring>
#include <Windows.h>

namespace uevr {

OpenXRIntegration::OpenXRIntegration()
    : m_initialized(false)
    , m_xrInstance(XR_NULL_HANDLE)
    , m_xrSession(XR_NULL_HANDLE)
    , m_viewSpace(XR_NULL_HANDLE)
    , m_localSpace(XR_NULL_HANDLE)
    , m_actionSet(XR_NULL_HANDLE)
    , m_thumbstickAction(XR_NULL_HANDLE)
    , m_triggerAction(XR_NULL_HANDLE)
    , m_gripAction(XR_NULL_HANDLE)
    , m_hapticAction(XR_NULL_HANDLE)
    , m_sessionState(XR_SESSION_STATE_UNKNOWN)
    , m_frameCount(0)
    , m_lastFrameTime(std::chrono::high_resolution_clock::now())
    , m_lastError("")
{
    // Initialize controller paths
    m_controllerPaths[0] = XR_NULL_PATH; // Left controller
    m_controllerPaths[1] = XR_NULL_PATH; // Right controller
    
    // Initialize swapchains
    m_swapchains.resize(2); // Left and right eye
    m_swapchainImages.resize(2);
    
    // Initialize view configuration
    m_viewConfigViews.resize(2);
    
    // Initialize performance metrics
    m_performanceMetrics = {0, 0, 0.0, 0.0, 0.0, 0.0};
    
    std::cout << "[OpenXR] OpenXR Integration initialized" << std::endl;
}

OpenXRIntegration::~OpenXRIntegration() {
    shutdown();
}

bool OpenXRIntegration::initialize() {
    if (m_initialized) {
        return true;
    }
    
    std::cout << "[OpenXR] Initializing OpenXR..." << std::endl;
    
    // Create OpenXR instance
    if (!createInstance()) {
        logError("Failed to create OpenXR instance");
        return false;
    }
    
    // Initialize input system
    if (!initializeInput()) {
        logError("Failed to initialize input system");
        return false;
    }
    
    // Initialize haptics
    if (!initializeHaptics()) {
        logError("Failed to initialize haptics");
        return false;
    }
    
    // Initialize rendering
    if (!initializeRendering()) {
        logError("Failed to initialize rendering");
        return false;
    }
    
    m_initialized = true;
    std::cout << "[OpenXR] OpenXR initialization successful" << std::endl;
    return true;
}

void OpenXRIntegration::shutdown() {
    if (!m_initialized) {
        return;
    }
    
    std::cout << "[OpenXR] Shutting down OpenXR..." << std::endl;
    
    // End session if active
    if (m_xrSession != XR_NULL_HANDLE && m_sessionState == XR_SESSION_STATE_FOCUSED) {
        endSession();
    }
    
    // Destroy session
    if (m_xrSession != XR_NULL_HANDLE) {
        destroySession();
    }
    
    // Destroy instance
    if (m_xrInstance != XR_NULL_HANDLE) {
        xrDestroyInstance(m_xrInstance);
        m_xrInstance = XR_NULL_HANDLE;
    }
    
    m_initialized = false;
    std::cout << "[OpenXR] OpenXR shutdown complete" << std::endl;
}

bool OpenXRIntegration::createSession() {
    if (m_xrInstance == XR_NULL_HANDLE) {
        logError("OpenXR instance not initialized");
        return false;
    }
    
    // Get system properties
    XrSystemGetInfo systemInfo = {XR_TYPE_SYSTEM_GET_INFO};
    systemInfo.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
    
    XrSystemId systemId;
    XrResult result = xrGetSystem(m_xrInstance, &systemInfo, &systemId);
    if (!checkXRResult(result, "xrGetSystem")) {
        return false;
    }
    
    // Get system properties
    XrSystemProperties systemProperties = {XR_TYPE_SYSTEM_PROPERTIES};
    result = xrGetSystemProperties(m_xrInstance, systemId, &systemProperties);
    if (!checkXRResult(result, "xrGetSystemProperties")) {
        return false;
    }
    
    std::cout << "[OpenXR] System: " << systemProperties.systemName << std::endl;
    std::cout << "[OpenXR] Vendor: " << systemProperties.vendorId << std::endl;
    
    // Create session
    XrSessionCreateInfo sessionCreateInfo = {XR_TYPE_SESSION_CREATE_INFO};
    sessionCreateInfo.systemId = systemId;
    
    result = xrCreateSession(m_xrInstance, &sessionCreateInfo, &m_xrSession);
    if (!checkXRResult(result, "xrCreateSession")) {
        return false;
    }
    
    // Create reference spaces
    if (!createReferenceSpaces()) {
        return false;
    }
    
    std::cout << "[OpenXR] Session created successfully" << std::endl;
    return true;
}

void OpenXRIntegration::destroySession() {
    if (m_xrSession != XR_NULL_HANDLE) {
        xrDestroySession(m_xrSession);
        m_xrSession = XR_NULL_HANDLE;
        std::cout << "[OpenXR] Session destroyed" << std::endl;
    }
}

bool OpenXRIntegration::beginSession() {
    if (m_xrSession == XR_NULL_HANDLE) {
        logError("Session not created");
        return false;
    }
    
    XrSessionBeginInfo sessionBeginInfo = {XR_TYPE_SESSION_BEGIN_INFO};
    sessionBeginInfo.primaryViewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
    
    XrResult result = xrBeginSession(m_xrSession, &sessionBeginInfo);
    if (!checkXRResult(result, "xrBeginSession")) {
        return false;
    }
    
    m_sessionState = XR_SESSION_STATE_READY;
    std::cout << "[OpenXR] Session begun" << std::endl;
    return true;
}

void OpenXRIntegration::endSession() {
    if (m_xrSession != XR_NULL_HANDLE && m_sessionState != XR_SESSION_STATE_STOPPING) {
        xrEndSession(m_xrSession);
        m_sessionState = XR_SESSION_STATE_STOPPING;
        std::cout << "[OpenXR] Session ended" << std::endl;
    }
}

bool OpenXRIntegration::initializeInput() {
    if (m_xrInstance == XR_NULL_HANDLE) {
        logError("OpenXR instance not initialized");
        return false;
    }
    
    // Create action set
    XrActionSetCreateInfo actionSetInfo = {XR_TYPE_ACTION_SET_CREATE_INFO};
    strcpy_s(actionSetInfo.actionSetName, "uevr_actions");
    strcpy_s(actionSetInfo.localizedActionSetName, "uevr Actions");
    
    XrResult result = xrCreateActionSet(m_xrInstance, &actionSetInfo, &m_actionSet);
    if (!checkXRResult(result, "xrCreateActionSet")) {
        return false;
    }
    
    // Create actions
    if (!createActions()) {
        return false;
    }
    
    // Create action spaces
    if (!createActionSpaces()) {
        return false;
    }
    
    // Suggest bindings
    if (!suggestBindings()) {
        return false;
    }
    
    std::cout << "[OpenXR] Input system initialized" << std::endl;
    return true;
}

bool OpenXRIntegration::initializeHaptics() {
    if (m_xrSession == XR_NULL_HANDLE) {
        logError("Session not created");
        return false;
    }
    
    // Create haptic action
    XrActionCreateInfo hapticActionInfo = {XR_TYPE_ACTION_CREATE_INFO};
    hapticActionInfo.actionType = XR_ACTION_TYPE_VIBRATION_OUTPUT;
    strcpy_s(hapticActionInfo.actionName, "haptic_feedback");
    strcpy_s(hapticActionInfo.localizedActionName, "Haptic Feedback");
    
    XrResult result = xrCreateAction(m_actionSet, &hapticActionInfo, &m_hapticAction);
    if (!checkXRResult(result, "xrCreateAction")) {
        return false;
    }
    
    std::cout << "[OpenXR] Haptics system initialized" << std::endl;
    return true;
}

bool OpenXRIntegration::initializeRendering() {
    if (m_xrSession == XR_NULL_HANDLE) {
        logError("Session not created");
        return false;
    }
    
    // Get view configuration properties
    uint32_t viewConfigCount;
    XrResult result = xrEnumerateViewConfigurations(m_xrInstance, m_xrSession, 0, &viewConfigCount, nullptr);
    if (!checkXRResult(result, "xrEnumerateViewConfigurations")) {
        return false;
    }
    
    std::vector<XrViewConfigurationType> viewConfigTypes(viewConfigCount);
    result = xrEnumerateViewConfigurations(m_xrInstance, m_xrSession, viewConfigCount, &viewConfigCount, viewConfigTypes.data());
    if (!checkXRResult(result, "xrEnumerateViewConfigurations")) {
        return false;
    }
    
    // Find stereo configuration
    bool foundStereo = false;
    for (const auto& configType : viewConfigTypes) {
        if (configType == XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO) {
            foundStereo = true;
            break;
        }
    }
    
    if (!foundStereo) {
        logError("Stereo view configuration not supported");
        return false;
    }
    
    // Get view configuration views
    uint32_t viewCount;
    result = xrEnumerateViewConfigurationViews(m_xrInstance, m_xrSession, XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO, 0, &viewCount, nullptr);
    if (!checkXRResult(result, "xrEnumerateViewConfigurationViews")) {
        return false;
    }
    
    m_viewConfigViews.resize(viewCount, {XR_TYPE_VIEW_CONFIGURATION_VIEW});
    result = xrEnumerateViewConfigurationViews(m_xrInstance, m_xrSession, XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO, viewCount, &viewCount, m_viewConfigViews.data());
    if (!checkXRResult(result, "xrEnumerateViewConfigurationViews")) {
        return false;
    }
    
    // Create swapchains
    if (!createSwapchains()) {
        return false;
    }
    
    std::cout << "[OpenXR] Rendering system initialized" << std::endl;
    return true;
}

bool OpenXRIntegration::beginFrame() {
    if (m_xrSession == XR_NULL_HANDLE) {
        return false;
    }
    
    // Wait for frame
    XrFrameWaitInfo frameWaitInfo = {XR_TYPE_FRAME_WAIT_INFO};
    XrFrameState frameState = {XR_TYPE_FRAME_STATE};
    
    XrResult result = xrWaitFrame(m_xrSession, &frameWaitInfo, &frameState);
    if (!checkXRResult(result, "xrWaitFrame")) {
        return false;
    }
    
    // Begin frame
    XrFrameBeginInfo frameBeginInfo = {XR_TYPE_FRAME_BEGIN_INFO};
    result = xrBeginFrame(m_xrSession, &frameBeginInfo);
    if (!checkXRResult(result, "xrBeginFrame")) {
        return false;
    }
    
    // Update performance metrics
    updatePerformanceMetrics();
    
    return true;
}

void OpenXRIntegration::endFrame() {
    if (m_xrSession == XR_NULL_HANDLE) {
        return;
    }
    
    // End frame
    XrFrameEndInfo frameEndInfo = {XR_TYPE_FRAME_END_INFO};
    frameEndInfo.displayTime = 0; // Will be set by submitFrame
    frameEndInfo.environmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_OPAQUE;
    frameEndInfo.layerCount = 0;
    frameEndInfo.layers = nullptr;
    
    XrResult result = xrEndFrame(m_xrSession, &frameEndInfo);
    if (!checkXRResult(result, "xrEndFrame")) {
        // Log error but don't fail
        logError("Failed to end frame");
    }
}

bool OpenXRIntegration::submitFrame(const std::vector<XrCompositionLayerBaseHeader*>& layers) {
    if (m_xrSession == XR_NULL_HANDLE) {
        return false;
    }
    
    // End frame with layers
    XrFrameEndInfo frameEndInfo = {XR_TYPE_FRAME_END_INFO};
    frameEndInfo.displayTime = 0; // Will be set by runtime
    frameEndInfo.environmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_OPAQUE;
    frameEndInfo.layerCount = static_cast<uint32_t>(layers.size());
    frameEndInfo.layers = layers.data();
    
    XrResult result = xrEndFrame(m_xrSession, &frameEndInfo);
    if (!checkXRResult(result, "xrSubmitFrame")) {
        return false;
    }
    
    m_frameCount++;
    return true;
}

void OpenXRIntegration::updateInput() {
    if (m_xrSession == XR_NULL_HANDLE || m_actionSet == XR_NULL_HANDLE) {
        return;
    }
    
    // Sync actions
    XrActiveActionSet activeActionSet = {m_actionSet, XR_NULL_PATH};
    XrActionsSyncInfo syncInfo = {XR_TYPE_ACTIONS_SYNC_INFO};
    syncInfo.countActiveActionSets = 1;
    syncInfo.activeActionSets = &activeActionSet;
    
    XrResult result = xrSyncActions(m_xrSession, &syncInfo);
    if (!checkXRResult(result, "xrSyncActions")) {
        return;
    }
    
    // Update controller states
    updateControllerStates();
}

bool OpenXRIntegration::triggerHapticFeedback(uint32_t controllerId, float intensity, float duration) {
    if (m_xrSession == XR_NULL_HANDLE || m_hapticAction == XR_NULL_HANDLE) {
        return false;
    }
    
    if (controllerId >= 2) {
        logError("Invalid controller ID: " + std::to_string(controllerId));
        return false;
    }
    
    // Apply haptic feedback
    XrHapticVibration hapticVibration = {XR_TYPE_HAPTIC_VIBRATION};
    hapticVibration.amplitude = intensity;
    hapticVibration.duration = duration;
    hapticVibration.frequency = XR_FREQUENCY_UNSPECIFIED;
    
    XrHapticActionInfo hapticActionInfo = {XR_TYPE_HAPTIC_ACTION_INFO};
    hapticActionInfo.action = m_hapticAction;
    hapticActionInfo.subactionPath = m_controllerPaths[controllerId];
    
    XrResult result = xrApplyHapticFeedback(m_xrSession, &hapticActionInfo, (const XrHapticBaseHeader*)&hapticVibration);
    if (!checkXRResult(result, "xrApplyHapticFeedback")) {
        return false;
    }
    
    return true;
}

VRDeviceInfo OpenXRIntegration::getDeviceInfo() const {
    VRDeviceInfo info;
    info.deviceName = "OpenXR Device";
    info.vendorId = 0;
    info.productId = 0;
    info.firmwareVersion = "1.0.0";
    info.supportedFeatures = "VR, Motion Controllers, Haptic Feedback";
    info.isConnected = m_initialized && m_xrSession != XR_NULL_HANDLE;
    return info;
}

std::vector<ControllerState> OpenXRIntegration::getControllerStates() const {
    return m_controllerStates;
}

VRRenderConfig OpenXRIntegration::getRenderConfig() const {
    VRRenderConfig config;
    config.stereoEnabled = true;
    config.ipd = 0.064f; // Default IPD
    config.nearPlane = 0.1f;
    config.farPlane = 1000.0f;
    config.frameRate = 90.0f;
    config.resolution = {1920, 1080}; // Default resolution
    return config;
}

PerformanceMetrics OpenXRIntegration::getPerformanceMetrics() const {
    return m_performanceMetrics;
}

std::string OpenXRIntegration::getLastError() const {
    return m_lastError;
}

void OpenXRIntegration::clearErrors() {
    m_lastError = "";
}

// Private helper methods

bool OpenXRIntegration::createInstance() {
    XrApplicationInfo appInfo = {XR_TYPE_APPLICATION_INFO};
    strcpy_s(appInfo.applicationName, "uevr");
    strcpy_s(appInfo.engineName, "uevr Engine");
    appInfo.applicationVersion = XR_MAKE_VERSION(1, 0, 0);
    appInfo.engineVersion = XR_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = XR_CURRENT_API_VERSION;
    
    XrInstanceCreateInfo createInfo = {XR_TYPE_INSTANCE_CREATE_INFO};
    createInfo.applicationInfo = appInfo;
    createInfo.enabledApiLayerCount = 0;
    createInfo.enabledExtensionCount = 0;
    
    XrResult result = xrCreateInstance(&createInfo, &m_xrInstance);
    if (!checkXRResult(result, "xrCreateInstance")) {
        return false;
    }
    
    std::cout << "[OpenXR] Instance created successfully" << std::endl;
    return true;
}

bool OpenXRIntegration::createReferenceSpaces() {
    if (m_xrSession == XR_NULL_HANDLE) {
        return false;
    }
    
    // Create view space
    XrReferenceSpaceCreateInfo viewSpaceInfo = {XR_TYPE_REFERENCE_SPACE_CREATE_INFO};
    viewSpaceInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_VIEW;
    viewSpaceInfo.poseInReferenceSpace = {{0, 0, 0, 1}, {0, 0, 0}};
    
    XrResult result = xrCreateReferenceSpace(m_xrSession, &viewSpaceInfo, &m_viewSpace);
    if (!checkXRResult(result, "xrCreateReferenceSpace view")) {
        return false;
    }
    
    // Create local space
    XrReferenceSpaceCreateInfo localSpaceInfo = {XR_TYPE_REFERENCE_SPACE_CREATE_INFO};
    localSpaceInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_LOCAL;
    localSpaceInfo.poseInReferenceSpace = {{0, 0, 0, 1}, {0, 0, 0}};
    
    result = xrCreateReferenceSpace(m_xrSession, &localSpaceInfo, &m_localSpace);
    if (!checkXRResult(result, "xrCreateReferenceSpace local")) {
        return false;
    }
    
    return true;
}

bool OpenXRIntegration::createActions() {
    if (m_actionSet == XR_NULL_HANDLE) {
        return false;
    }
    
    // Create thumbstick action
    XrActionCreateInfo thumbstickInfo = {XR_TYPE_ACTION_CREATE_INFO};
    thumbstickInfo.actionType = XR_ACTION_TYPE_VECTOR2F_INPUT;
    strcpy_s(thumbstickInfo.actionName, "thumbstick");
    strcpy_s(thumbstickInfo.localizedActionName, "Thumbstick");
    
    XrResult result = xrCreateAction(m_actionSet, &thumbstickInfo, &m_thumbstickAction);
    if (!checkXRResult(result, "xrCreateAction thumbstick")) {
        return false;
    }
    
    // Create trigger action
    XrActionCreateInfo triggerInfo = {XR_TYPE_ACTION_CREATE_INFO};
    triggerInfo.actionType = XR_ACTION_TYPE_FLOAT_INPUT;
    strcpy_s(triggerInfo.actionName, "trigger");
    strcpy_s(triggerInfo.localizedActionName, "Trigger");
    
    result = xrCreateAction(m_actionSet, &triggerInfo, &m_triggerAction);
    if (!checkXRResult(result, "xrCreateAction trigger")) {
        return false;
    }
    
    // Create grip action
    XrActionCreateInfo gripInfo = {XR_TYPE_ACTION_CREATE_INFO};
    gripInfo.actionType = XR_ACTION_TYPE_FLOAT_INPUT;
    strcpy_s(gripInfo.actionName, "grip");
    strcpy_s(gripInfo.localizedActionName, "Grip");
    
    result = xrCreateAction(m_actionSet, &gripInfo, &m_gripAction);
    if (!checkXRResult(result, "xrCreateAction grip")) {
        return false;
    }
    
    return true;
}

bool OpenXRIntegration::createActionSpaces() {
    if (m_xrSession == XR_NULL_HANDLE || m_actionSet == XR_NULL_HANDLE) {
        return false;
    }
    
    // Get controller paths
    uint32_t pathCount;
    XrResult result = xrStringToPath(m_xrInstance, "/user/hand/left", &m_controllerPaths[0]);
    if (!checkXRResult(result, "xrStringToPath left")) {
        return false;
    }
    
    result = xrStringToPath(m_xrInstance, "/user/hand/right", &m_controllerPaths[1]);
    if (!checkXRResult(result, "xrStringToPath right")) {
        return false;
    }
    
    return true;
}

bool OpenXRIntegration::suggestBindings() {
    if (m_xrInstance == XR_NULL_HANDLE || m_actionSet == XR_NULL_HANDLE) {
        return false;
    }
    
    // Create suggested bindings
    std::vector<XrActionSuggestedBinding> bindings;
    
    // Left controller bindings
    XrPath leftThumbstickPath, leftTriggerPath, leftGripPath;
    xrStringToPath(m_xrInstance, "/user/hand/left/input/thumbstick", &leftThumbstickPath);
    xrStringToPath(m_xrInstance, "/user/hand/left/input/trigger", &leftTriggerPath);
    xrStringToPath(m_xrInstance, "/user/hand/left/input/squeeze", &leftGripPath);
    
    bindings.push_back({m_thumbstickAction, leftThumbstickPath});
    bindings.push_back({m_triggerAction, leftTriggerPath});
    bindings.push_back({m_gripAction, leftGripPath});
    
    // Right controller bindings
    XrPath rightThumbstickPath, rightTriggerPath, rightGripPath;
    xrStringToPath(m_xrInstance, "/user/hand/right/input/thumbstick", &rightThumbstickPath);
    xrStringToPath(m_xrInstance, "/user/hand/right/input/trigger", &rightTriggerPath);
    xrStringToPath(m_xrInstance, "/user/hand/right/input/squeeze", &rightGripPath);
    
    bindings.push_back({m_thumbstickAction, rightThumbstickPath});
    bindings.push_back({m_triggerAction, rightTriggerPath});
    bindings.push_back({m_gripAction, rightGripPath});
    
    // Suggest bindings
    XrInteractionProfileSuggestedBinding suggestedBindings = {XR_TYPE_INTERACTION_PROFILE_SUGGESTED_BINDING};
    suggestedBindings.interactionProfile = XR_NULL_PATH; // Use default profile
    suggestedBindings.countSuggestedBindings = static_cast<uint32_t>(bindings.size());
    suggestedBindings.suggestedBindings = bindings.data();
    
    XrResult result = xrSuggestInteractionProfileBindings(m_xrInstance, &suggestedBindings);
    if (!checkXRResult(result, "xrSuggestInteractionProfileBindings")) {
        return false;
    }
    
    return true;
}

bool OpenXRIntegration::createSwapchains() {
    if (m_xrSession == XR_NULL_HANDLE) {
        return false;
    }
    
    // Create swapchains for each view
    for (size_t i = 0; i < m_viewConfigViews.size(); ++i) {
        XrSwapchainCreateInfo swapchainInfo = {XR_TYPE_SWAPCHAIN_CREATE_INFO};
        swapchainInfo.usageFlags = XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT | XR_SWAPCHAIN_USAGE_SAMPLED_BIT;
        swapchainInfo.format = 0; // Will be set by runtime
        swapchainInfo.sampleCount = 1;
        swapchainInfo.width = m_viewConfigViews[i].recommendedImageRectWidth;
        swapchainInfo.height = m_viewConfigViews[i].recommendedImageRectHeight;
        swapchainInfo.faceCount = 1;
        swapchainInfo.arraySize = 1;
        swapchainInfo.mipCount = 1;
        
        XrSwapchain swapchain;
        XrResult result = xrCreateSwapchain(m_xrSession, &swapchainInfo, &swapchain);
        if (!checkXRResult(result, "xrCreateSwapchain")) {
            return false;
        }
        
        m_swapchains[i] = swapchain;
        
        // Get swapchain images
        uint32_t imageCount;
        result = xrEnumerateSwapchainImages(swapchain, 0, &imageCount, nullptr);
        if (!checkXRResult(result, "xrEnumerateSwapchainImages")) {
            return false;
        }
        
        m_swapchainImages[i].resize(imageCount);
        result = xrEnumerateSwapchainImages(swapchain, imageCount, &imageCount, m_swapchainImages[i].data());
        if (!checkXRResult(result, "xrEnumerateSwapchainImages")) {
            return false;
        }
    }
    
    return true;
}

void OpenXRIntegration::updateControllerStates() {
    if (m_controllerPaths[0] == XR_NULL_PATH || m_controllerPaths[1] == XR_NULL_PATH) {
        return;
    }
    
    m_controllerStates.clear();
    m_controllerStates.resize(2);
    
    // Update left controller
    updateControllerState(0, m_controllerPaths[0]);
    
    // Update right controller
    updateControllerState(1, m_controllerPaths[1]);
}

void OpenXRIntegration::updateControllerState(uint32_t controllerId, XrPath controllerPath) {
    if (controllerId >= m_controllerStates.size()) {
        return;
    }
    
    ControllerState& state = m_controllerStates[controllerId];
    state.controllerId = controllerId;
    state.isConnected = true;
    
    // Get thumbstick state
    XrActionStateVector2f thumbstickState = {XR_TYPE_ACTION_STATE_VECTOR2F};
    XrActionStateGetInfo thumbstickInfo = {XR_TYPE_ACTION_STATE_GET_INFO};
    thumbstickInfo.action = m_thumbstickAction;
    thumbstickInfo.subactionPath = controllerPath;
    
    if (xrGetActionStateVector2f(m_xrSession, &thumbstickInfo, &thumbstickState) == XR_SUCCESS) {
        state.thumbstick = {thumbstickState.currentState.x, thumbstickState.currentState.y};
        state.thumbstickActive = thumbstickState.isActive;
    }
    
    // Get trigger state
    XrActionStateFloat triggerState = {XR_TYPE_ACTION_STATE_FLOAT};
    XrActionStateGetInfo triggerInfo = {XR_TYPE_ACTION_STATE_GET_INFO};
    triggerInfo.action = m_triggerAction;
    triggerInfo.subactionPath = controllerPath;
    
    if (xrGetActionStateFloat(m_xrSession, &triggerInfo, &triggerState) == XR_SUCCESS) {
        state.trigger = triggerState.currentState;
        state.triggerActive = triggerState.isActive;
    }
    
    // Get grip state
    XrActionStateFloat gripState = {XR_TYPE_ACTION_STATE_FLOAT};
    XrActionStateGetInfo gripInfo = {XR_TYPE_ACTION_STATE_GET_INFO};
    gripInfo.action = m_gripAction;
    gripInfo.subactionPath = controllerPath;
    
    if (xrGetActionStateFloat(m_xrSession, &gripInfo, &gripState) == XR_SUCCESS) {
        state.grip = gripState.currentState;
        state.gripActive = gripState.isActive;
    }
}

void OpenXRIntegration::updatePerformanceMetrics() {
    auto now = std::chrono::high_resolution_clock::now();
    auto frameTime = std::chrono::duration<double, std::milli>(now - m_lastFrameTime).count();
    
    m_performanceMetrics.frameCount = m_frameCount;
    m_performanceMetrics.averageFrameTime = frameTime;
    m_performanceMetrics.lastFrameTime = frameTime;
    
    // Update rolling average
    if (m_frameCount > 0) {
        m_performanceMetrics.averageFrameTime = 
            (m_performanceMetrics.averageFrameTime * (m_frameCount - 1) + frameTime) / m_frameCount;
    }
    
    m_lastFrameTime = now;
}

bool OpenXRIntegration::checkXRResult(XrResult result, const std::string& operation) {
    if (XR_SUCCEEDED(result)) {
        return true;
    }
    
    std::string errorMsg = "OpenXR error in " + operation + ": " + std::to_string(result);
    logError(errorMsg);
    return false;
}

void OpenXRIntegration::logError(const std::string& error) {
    m_lastError = error;
    std::cerr << "[OpenXR ERROR] " << error << std::endl;
}

} // namespace uevr
