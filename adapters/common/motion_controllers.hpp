/*
 * Cross-Engine Motion Controller Integration System for UEVR Adapters
 * 
 * This header provides engine-specific motion controller support
 * for all cross-engine VR adapters, ensuring optimal VR interaction.
 * 
 * Copyright (c) 2024 UEVR Cross-Engine Adapter Project
 * Licensed under MIT License
 */

#pragma once

#include <windows.h>
#include <d3d11.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <memory>
#include <vector>
#include <unordered_map>
#include <string>

namespace uevr_common {

/**
 * Motion controller manager for cross-engine adapters
 * 
 * Provides engine-specific motion controller support
 * to ensure optimal VR interaction across different engines.
 */
class MotionControllerManager {
public:
    // Controller configuration
    struct ControllerConfig {
        enum class Hand {
            Left,
            Right,
            Both
        };
        
        Hand primary_hand = Hand::Right;
        bool enable_haptic_feedback = true;
        bool enable_gesture_recognition = true;
        float deadzone = 0.1f;
        float sensitivity = 1.0f;
        bool invert_y_axis = false;
        bool enable_controller_visualization = true;
    };

    // Controller state
    struct ControllerState {
        bool connected = false;
        bool tracking = false;
        DirectX::XMFLOAT3 position = {0.0f, 0.0f, 0.0f};
        DirectX::XMFLOAT4 rotation = {0.0f, 0.0f, 0.0f, 1.0f}; // Quaternion
        DirectX::XMFLOAT2 thumbstick = {0.0f, 0.0f};
        DirectX::XMFLOAT2 trackpad = {0.0f, 0.0f};
        float trigger_value = 0.0f;
        float grip_value = 0.0f;
        bool primary_button = false;
        bool secondary_button = false;
        bool menu_button = false;
        bool system_button = false;
        bool thumbstick_click = false;
        bool trackpad_click = false;
        bool trackpad_touch = false;
    };

    // Attachment configuration
    struct AttachmentConfig {
        std::string component_name;
        std::string component_path;
        DirectX::XMFLOAT3 offset = {0.0f, 0.0f, 0.0f};
        DirectX::XMFLOAT4 rotation_offset = {0.0f, 0.0f, 0.0f, 1.0f}; // Quaternion
        float lerp_speed = 15.0f;
        bool permanent = false;
        bool visible = true;
        std::vector<std::string> properties;
    };

public:
    MotionControllerManager();
    ~MotionControllerManager();

    // Configuration management
    void set_controller_config(const ControllerConfig& config);
    const ControllerConfig& get_controller_config() const;
    
    // Controller state management
    void update_controller_state(ControllerState& left_state, ControllerState& right_state);
    const ControllerState& get_controller_state(bool is_right_hand) const;
    
    // Attachment management
    void attach_component(const std::string& name, const AttachmentConfig& config, bool is_right_hand);
    void detach_component(const std::string& name, bool is_right_hand);
    void update_attachment(const std::string& name, const AttachmentConfig& config, bool is_right_hand);
    const AttachmentConfig* get_attachment(const std::string& name, bool is_right_hand) const;
    
    // Input mapping
    void map_input_action(const std::string& action, const std::string& input, bool is_right_hand);
    void unmap_input_action(const std::string& action, bool is_right_hand);
    bool is_action_triggered(const std::string& action, bool is_right_hand) const;
    
    // Haptic feedback
    void trigger_haptic_feedback(bool is_right_hand, float intensity, float duration);
    void stop_haptic_feedback(bool is_right_hand);
    
    // Engine-specific integration
    void integrate_with_engine(const std::string& engine_name, const std::string& game_name);
    void optimize_for_vr_interaction();

private:
    ControllerConfig m_config;
    ControllerState m_left_controller;
    ControllerState m_right_controller;
    
    std::unordered_map<std::string, AttachmentConfig> m_left_attachments;
    std::unordered_map<std::string, AttachmentConfig> m_right_attachments;
    std::unordered_map<std::string, std::string> m_input_mappings;
    
    // Internal helpers
    void setup_default_attachments();
    void cleanup_attachments();
    DirectX::XMMATRIX calculate_attachment_transform(const AttachmentConfig& config, 
                                                    const ControllerState& controller_state) const;
    void apply_haptic_feedback(bool is_right_hand, float intensity, float duration);
};

/**
 * RE Engine specific motion controller manager
 * 
 * Provides motion controller support specific to Capcom's RE Engine games.
 */
class REEngineMotionControllers : public MotionControllerManager {
public:
    REEngineMotionControllers();
    
    // RE Engine specific motion controller features
    void setup_horror_game_controls();
    void optimize_weapon_interaction();
    void adapt_inventory_controls();
    void setup_vr_comfort_settings();
    
private:
    void setup_re_engine_controller_profile();
    void apply_horror_game_controller_optimizations();
};

/**
 * REDengine 4 specific motion controller manager
 * 
 * Provides motion controller support specific to CD Projekt Red's REDengine 4 games.
 */
class REDengine4MotionControllers : public MotionControllerManager {
public:
    REDengine4MotionControllers();
    
    // REDengine 4 specific motion controller features
    void setup_cyberpunk_controls();
    void optimize_hacking_interface();
    void adapt_vehicle_controls();
    void setup_weapon_customization();
    
private:
    void setup_redengine4_controller_profile();
    void apply_cyberpunk_controller_optimizations();
};

/**
 * MT Framework specific motion controller manager
 * 
 * Provides motion controller support specific to Capcom's MT Framework games.
 */
class MTFrameworkMotionControllers : public MotionControllerManager {
public:
    MTFrameworkMotionControllers();
    
    // MT Framework specific motion controller features
    void setup_hunting_controls();
    void optimize_weapon_switching();
    void adapt_monster_interaction();
    void setup_crafting_controls();
    
private:
    void setup_mt_framework_controller_profile();
    void apply_action_game_controller_optimizations();
};

/**
 * Global motion controller manager
 * 
 * Provides centralized access to motion controller features.
 */
class GlobalMotionControllerManager {
public:
    static MotionControllerManager& get_controller_manager();
    static REEngineMotionControllers& get_re_engine_controller_manager();
    static REDengine4MotionControllers& get_redengine4_controller_manager();
    static MTFrameworkMotionControllers& get_mt_framework_controller_manager();
    
    // Global configuration
    static void configure_motion_controllers(const ControllerConfig& config);
    static void set_global_controller_profile(const std::string& profile);
    
    // Global cleanup
    static void cleanup_all();

private:
    static std::unique_ptr<MotionControllerManager> s_global_controller_manager;
    static std::unique_ptr<REEngineMotionControllers> s_re_engine_controller_manager;
    static std::unique_ptr<REDengine4MotionControllers> s_redengine4_controller_manager;
    static std::unique_ptr<MTFrameworkMotionControllers> s_mt_framework_controller_manager;
    static std::mutex s_init_mutex;
    
    static void initialize_if_needed();
};

} // namespace uevr_common
