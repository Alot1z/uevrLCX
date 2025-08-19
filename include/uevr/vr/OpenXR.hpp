/*
 * UEVR OpenXR Integration
 * 
 * OpenXR support for UEVR
 * 
 * Copyright (c) 2024 UEVR Project
 * Licensed under MIT License
 */

#pragma once

#include <openxr/openxr.h>
#include <openxr/openxr_platform.h>
#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace uevr {
namespace VR {

// OpenXR instance management
class OpenXRInstance {
public:
    OpenXRInstance();
    ~OpenXRInstance();
    
    bool initialize();
    void cleanup();
    bool isInitialized() const;
    
    XrInstance getInstance() const;
    XrSystemId getSystemId() const;
    
private:
    XrInstance m_instance;
    XrSystemId m_system_id;
    bool m_initialized;
};

// OpenXR session management
class OpenXRSession {
public:
    OpenXRSession();
    ~OpenXRSession();
    
    bool createSession(const OpenXRInstance& instance);
    void destroySession();
    bool isSessionCreated() const;
    
    XrSession getSession() const;
    
private:
    XrSession m_session;
    bool m_created;
};

// OpenXR swapchain management
class OpenXRSwapchain {
public:
    OpenXRSwapchain();
    ~OpenXRSwapchain();
    
    bool createSwapchain(const OpenXRSession& session, uint32_t width, uint32_t height);
    void destroySwapchain();
    
    XrSwapchain getSwapchain() const;
    uint32_t getWidth() const;
    uint32_t getHeight() const;
    
private:
    XrSwapchain m_swapchain;
    uint32_t m_width;
    uint32_t m_height;
};

// OpenXR action set management
class OpenXRActionSet {
public:
    OpenXRActionSet();
    ~OpenXRActionSet();
    
    bool createActionSet(const std::string& name, const std::string& localized_name);
    void destroyActionSet();
    
    XrActionSet getActionSet() const;
    
private:
    XrActionSet m_action_set;
    bool m_created;
};

// OpenXR action management
class OpenXRAction {
public:
    OpenXRAction();
    ~OpenXRAction();
    
    bool createAction(const OpenXRActionSet& action_set, 
                     const std::string& name,
                     const std::string& localized_name,
                     XrActionType action_type);
    void destroyAction();
    
    XrAction getAction() const;
    
private:
    XrAction m_action;
    bool m_created;
};

// OpenXR interaction profile management
class OpenXRInteractionProfile {
public:
    OpenXRInteractionProfile();
    ~OpenXRInteractionProfile();
    
    bool setInteractionProfile(const OpenXRSession& session,
                             const std::string& path,
                             const std::vector<XrActionSuggestedBinding>& bindings);
    
private:
    std::vector<XrActionSuggestedBinding> m_bindings;
};

// OpenXR space management
class OpenXRSpace {
public:
    OpenXRSpace();
    ~OpenXRSpace();
    
    bool createReferenceSpace(const OpenXRSession& session,
                             XrReferenceSpaceType reference_space_type,
                             const XrPosef& pose_in_reference_space);
    void destroySpace();
    
    XrSpace getSpace() const;
    
private:
    XrSpace m_space;
    bool m_created;
};

// OpenXR composition layer management
class OpenXRCompositionLayer {
public:
    OpenXRCompositionLayer();
    ~OpenXRCompositionLayer();
    
    bool createProjectionLayer(const OpenXRSpace& space,
                              const OpenXRSwapchain& swapchain,
                              const XrPosef& pose);
    void destroyLayer();
    
    XrCompositionLayerProjection getLayer() const;
    
private:
    XrCompositionLayerProjection m_layer;
    bool m_created;
};

// Main OpenXR manager class
class OpenXRManager {
public:
    static OpenXRManager& getInstance();
    
    OpenXRManager();
    ~OpenXRManager();
    
    bool initialize();
    void cleanup();
    bool isInitialized() const;
    
    // Core OpenXR components
    OpenXRInstance& getInstance();
    OpenXRSession& getSession();
    OpenXRSwapchain& getSwapchain();
    
    // Action management
    bool createActionSet(const std::string& name, const std::string& localized_name);
    bool createAction(const std::string& action_set_name,
                     const std::string& action_name,
                     const std::string& localized_name,
                     XrActionType action_type);
    
    // Space management
    bool createReferenceSpace(XrReferenceSpaceType reference_space_type,
                             const XrPosef& pose_in_reference_space);
    
    // Frame management
    bool beginFrame();
    void endFrame();
    
    // Input handling
    bool pollEvents();
    bool syncActions();
    
    // Rendering
    bool beginSession();
    void endSession();
    bool waitFrame();
    bool beginFrame();
    void endFrame();
    
private:
    std::unique_ptr<OpenXRInstance> m_instance;
    std::unique_ptr<OpenXRSession> m_session;
    std::unique_ptr<OpenXRSwapchain> m_swapchain;
    std::unique_ptr<OpenXRActionSet> m_action_set;
    std::unique_ptr<OpenXRSpace> m_view_space;
    std::unique_ptr<OpenXRSpace> m_stage_space;
    
    bool m_initialized;
    bool m_session_running;
    XrFrameState m_frame_state;
};

} // namespace VR
} // namespace uevr
