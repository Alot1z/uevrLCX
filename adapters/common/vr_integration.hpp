/*
 * VR Integration Utility for UEVR Cross-Engine Adapters
 * 
 * This header provides utilities for integrating extracted matrices with UEVR's
 * VR framework, including stereo rendering, motion controllers, and UI projection.
 * 
 * Copyright (c) 2024 UEVR Cross-Engine Adapter Project
 * Licensed under MIT License
 */

#pragma once

#include <windows.h>
#include <d3d11.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <vector>
#include <memory>
#include <string>

namespace uevr_common {

/**
 * VR integration utility class
 * 
 * Provides common functionality for integrating extracted matrices with UEVR's
 * VR framework and handling VR-specific rendering and input.
 */
class VRIntegrator {
public:
    // VR configuration
    struct VRConfig {
        float ipd = 0.064f; // Interpupillary distance in meters (64mm default)
        float near_clip = 0.01f; // Near clipping plane in meters
        float far_clip = 1000.0f; // Far clipping plane in meters
        float fov_horizontal = 90.0f; // Horizontal field of view in degrees
        float fov_vertical = 90.0f; // Vertical field of view in degrees
        bool enable_stereo = true;
        bool enable_motion_controllers = true;
        bool enable_ui_projection = true;
    };

    // Stereo rendering matrices
    struct StereoMatrices {
        DirectX::XMMATRIX view_left, view_right;
        DirectX::XMMATRIX projection_left, projection_right;
        DirectX::XMMATRIX view_projection_left, view_projection_right;
        bool valid = false;
    };

    // Motion controller configuration
    struct MotionControllerConfig {
        bool weapon_right = true; // Weapon attached to right controller
        bool utility_left = true; // Utility items attached to left controller
        float attachment_lerp_speed = 15.0f; // Attachment smoothing speed
        bool permanent_changes = false; // Whether to make permanent changes
    };

    // UI projection configuration
    struct UIProjectionConfig {
        enum class ProjectionType {
            Billboard,      // Always face camera
            Depth,          // Project onto 3D surfaces
            Raycast,        // Raycast-based projection
            Cylinder       // Cylindrical projection
        };
        
        ProjectionType type = ProjectionType::Billboard;
        float distance = 2.0f; // Distance from camera
        float scale = 1.0f; // UI scale factor
        bool follow_view = false; // Follow camera view
        float angle = 90.0f; // Cylinder angle for cylindrical projection
    };

public:
    VRIntegrator() = default;
    ~VRIntegrator() = default;

    // Configuration
    void set_vr_config(const VRConfig& config) { m_vr_config = config; }
    void set_motion_controller_config(const MotionControllerConfig& config) { m_motion_controller_config = config; }
    void set_ui_projection_config(const UIProjectionConfig& config) { m_ui_projection_config = config; }

    // Stereo rendering
    StereoMatrices generate_stereo_matrices(
        const DirectX::XMMATRIX& view_matrix,
        const DirectX::XMMATRIX& projection_matrix
    );

    // Motion controller integration
    bool attach_weapon_to_controller(
        const DirectX::XMMATRIX& weapon_matrix,
        bool right_controller,
        DirectX::XMMATRIX& controller_matrix
    );

    bool attach_utility_to_controller(
        const DirectX::XMMATRIX& utility_matrix,
        bool left_controller,
        DirectX::XMMATRIX& controller_matrix
    );

    // UI projection
    DirectX::XMMATRIX project_ui_to_3d(
        const DirectX::XMMATRIX& ui_matrix,
        const DirectX::XMMATRIX& camera_matrix,
        const DirectX::XMVECTOR& ui_position
    );

    // VR comfort and accessibility
    struct ComfortSettings {
        bool decoupled_pitch = true; // Decouple pitch from head movement
        bool snap_turn = false; // Enable snap turning
        float snap_turn_angle = 45.0f; // Snap turn angle in degrees
        float joystick_deadzone = 0.2f; // Joystick deadzone
        bool roomscale_movement = true; // Enable roomscale movement
        float world_scale = 1.0f; // World scale factor
    };

    void set_comfort_settings(const ComfortSettings& settings) { m_comfort_settings = settings; }
    
    // Apply comfort settings to matrices
    DirectX::XMMATRIX apply_comfort_settings(
        const DirectX::XMMATRIX& view_matrix,
        const DirectX::XMMATRIX& projection_matrix
    );

    // Performance optimization
    struct PerformanceSettings {
        bool enable_foveated_rendering = false; // Enable foveated rendering
        float resolution_scale = 0.9f; // Resolution scale for VR
        bool uncap_framerate = true; // Uncap framerate
        bool show_fps_overlay = true; // Show FPS overlay
        bool show_stats_overlay = false; // Show performance stats
    };

    void set_performance_settings(const PerformanceSettings& settings) { m_performance_settings = settings; }

    // VR state management
    bool is_vr_active() const { return m_vr_active; }
    void set_vr_active(bool active) { m_vr_active = active; }
    
    bool is_stereo_enabled() const { return m_vr_config.enable_stereo; }
    bool are_motion_controllers_enabled() const { return m_vr_config.enable_motion_controllers; }
    bool is_ui_projection_enabled() const { return m_vr_config.enable_ui_projection; }

    // Debug and logging
    void log_vr_state();
    void log_stereo_matrices(const StereoMatrices& matrices);
    std::string matrix_to_string(const DirectX::XMMATRIX& matrix, const std::string& name = "Matrix");

private:
    // Internal VR state
    VRConfig m_vr_config;
    MotionControllerConfig m_motion_controller_config;
    UIProjectionConfig m_ui_projection_config;
    ComfortSettings m_comfort_settings;
    PerformanceSettings m_performance_settings;
    
    bool m_vr_active = false;
    
    // Stereo rendering helpers
    DirectX::XMMATRIX create_left_eye_view_matrix(
        const DirectX::XMMATRIX& base_view_matrix
    );
    
    DirectX::XMMATRIX create_right_eye_view_matrix(
        const DirectX::XMMATRIX& base_view_matrix
    );
    
    DirectX::XMMATRIX create_left_eye_projection_matrix(
        const DirectX::XMMATRIX& base_projection_matrix
    );
    
    DirectX::XMMATRIX create_right_eye_projection_matrix(
        const DirectX::XMMATRIX& base_projection_matrix
    );
    
    // Motion controller helpers
    DirectX::XMMATRIX calculate_controller_attachment(
        const DirectX::XMMATRIX& object_matrix,
        const DirectX::XMMATRIX& controller_matrix,
        float lerp_speed
    );
    
    // UI projection helpers
    DirectX::XMMATRIX create_billboard_matrix(
        const DirectX::XMVECTOR& position,
        const DirectX::XMVECTOR& camera_position,
        const DirectX::XMVECTOR& up_vector
    );
    
    DirectX::XMMATRIX create_cylinder_matrix(
        const DirectX::XMVECTOR& position,
        const DirectX::XMVECTOR& camera_position,
        float radius,
        float angle
    );
    
    // Comfort and accessibility helpers
    DirectX::XMMATRIX apply_decoupled_pitch(
        const DirectX::XMMATRIX& view_matrix
    );
    
    DirectX::XMMATRIX apply_snap_turn(
        const DirectX::XMMATRIX& view_matrix,
        float turn_angle
    );
    
    DirectX::XMMATRIX apply_world_scale(
        const DirectX::XMMATRIX& world_matrix,
        float scale_factor
    );
};

/**
 * Engine-specific VR integrators
 * 
 * These classes provide specialized VR integration for specific engines
 * based on their unique requirements and characteristics.
 */
class REEngineVRIntegrator : public VRIntegrator {
public:
    // RE Engine specific VR integration
    StereoMatrices generate_re_engine_stereo_matrices(
        const DirectX::XMMATRIX& view_matrix,
        const DirectX::XMMATRIX& projection_matrix
    );
    
    // Horror game specific comfort settings
    void apply_horror_game_comfort_settings();
    
private:
    // RE Engine specific VR adjustments
    void adjust_for_horror_game_mechanics();
    void optimize_for_fixed_camera_angles();
};

class REDengine4VRIntegrator : public VRIntegrator {
public:
    // REDengine 4 specific VR integration
    StereoMatrices generate_redengine4_stereo_matrices(
        const DirectX::XMMATRIX& view_matrix,
        const DirectX::XMMATRIX& projection_matrix
    );
    
    // Cyberpunk specific VR features
    void enable_cyberpunk_vr_features();
    
private:
    // REDengine 4 specific VR adjustments
    void adjust_for_first_person_perspective();
    void optimize_for_urban_environment();
};

class MTFrameworkVRIntegrator : public VRIntegrator {
public:
    // MT Framework specific VR integration
    StereoMatrices generate_mt_framework_stereo_matrices(
        const DirectX::XMMATRIX& view_matrix,
        const DirectX::XMMATRIX& projection_matrix
    );
    
    // Third-person to first-person conversion
    DirectX::XMMATRIX convert_third_person_to_first_person(
        const DirectX::XMMATRIX& third_person_matrix,
        const DirectX::XMVECTOR& camera_offset
    );
    
private:
    // MT Framework specific VR adjustments
    void adjust_for_third_person_gameplay();
    void optimize_for_action_combat();
};

} // namespace uevr_common
