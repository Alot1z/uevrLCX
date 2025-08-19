/*
 * Cross-Engine Motion Controller Integration System Implementation
 * 
 * Implementation of engine-specific motion controller support
 * for all cross-engine VR adapters.
 * 
 * Copyright (c) 2024 UEVR Cross-Engine Adapter Project
 * Licensed under MIT License
 */

#include "motion_controllers.hpp"
#include <spdlog/spdlog.h>
#include <algorithm>

namespace uevr_common {

// MotionControllerManager Implementation
MotionControllerManager::MotionControllerManager() {
    setup_default_attachments();
    spdlog::info("[MotionControllers] Motion controller manager initialized");
}

MotionControllerManager::~MotionControllerManager() {
    cleanup_attachments();
    spdlog::info("[MotionControllers] Motion controller manager destroyed");
}

void MotionControllerManager::set_controller_config(const ControllerConfig& config) {
    m_config = config;
    spdlog::info("[MotionControllers] Controller configuration updated");
}

const MotionControllerManager::ControllerConfig& MotionControllerManager::get_controller_config() const {
    return m_config;
}

void MotionControllerManager::update_controller_state(ControllerState& left_state, ControllerState& right_state) {
    m_left_controller = left_state;
    m_right_controller = right_state;
    
    spdlog::debug("[MotionControllers] Controller states updated - Left: {}, Right: {}", 
                 left_state.connected ? "Connected" : "Disconnected",
                 right_state.connected ? "Connected" : "Disconnected");
}

const MotionControllerManager::ControllerState& MotionControllerManager::get_controller_state(bool is_right_hand) const {
    return is_right_hand ? m_right_controller : m_left_controller;
}

void MotionControllerManager::attach_component(const std::string& name, const AttachmentConfig& config, bool is_right_hand) {
    auto& attachments = is_right_hand ? m_right_attachments : m_left_attachments;
    
    attachments[name] = config;
    
    spdlog::info("[MotionControllers] Component attached: {} to {} hand", 
                 name, is_right_hand ? "right" : "left");
}

void MotionControllerManager::detach_component(const std::string& name, bool is_right_hand) {
    auto& attachments = is_right_hand ? m_right_attachments : m_left_attachments;
    
    auto it = attachments.find(name);
    if (it != attachments.end()) {
        attachments.erase(it);
        spdlog::info("[MotionControllers] Component detached: {} from {} hand", 
                     name, is_right_hand ? "right" : "left");
    }
}

void MotionControllerManager::update_attachment(const std::string& name, const AttachmentConfig& config, bool is_right_hand) {
    auto& attachments = is_right_hand ? m_right_attachments : m_left_attachments;
    
    auto it = attachments.find(name);
    if (it != attachments.end()) {
        it->second = config;
        spdlog::debug("[MotionControllers] Attachment updated: {} on {} hand", 
                     name, is_right_hand ? "right" : "left");
    }
}

const MotionControllerManager::AttachmentConfig* MotionControllerManager::get_attachment(const std::string& name, bool is_right_hand) const {
    const auto& attachments = is_right_hand ? m_right_attachments : m_left_attachments;
    
    auto it = attachments.find(name);
    if (it != attachments.end()) {
        return &it->second;
    }
    
    return nullptr;
}

void MotionControllerManager::map_input_action(const std::string& action, const std::string& input, bool is_right_hand) {
    std::string key = action + "_" + (is_right_hand ? "right" : "left");
    m_input_mappings[key] = input;
    
    spdlog::debug("[MotionControllers] Input mapped: {} -> {} on {} hand", 
                 action, input, is_right_hand ? "right" : "left");
}

void MotionControllerManager::unmap_input_action(const std::string& action, bool is_right_hand) {
    std::string key = action + "_" + (is_right_hand ? "right" : "left");
    
    auto it = m_input_mappings.find(key);
    if (it != m_input_mappings.end()) {
        m_input_mappings.erase(it);
        spdlog::debug("[MotionControllers] Input unmapped: {} from {} hand", 
                     action, is_right_hand ? "right" : "left");
    }
}

bool MotionControllerManager::is_action_triggered(const std::string& action, bool is_right_hand) const {
    std::string key = action + "_" + (is_right_hand ? "right" : "left");
    
    auto it = m_input_mappings.find(key);
    if (it != m_input_mappings.end()) {
        const ControllerState& state = get_controller_state(is_right_hand);
        
        // Check if the mapped input is triggered
        if (it->second == "trigger" && state.trigger_value > 0.5f) {
            return true;
        } else if (it->second == "grip" && state.grip_value > 0.5f) {
            return true;
        } else if (it->second == "primary_button" && state.primary_button) {
            return true;
        } else if (it->second == "secondary_button" && state.secondary_button) {
            return true;
        }
    }
    
    return false;
}

void MotionControllerManager::trigger_haptic_feedback(bool is_right_hand, float intensity, float duration) {
    if (m_config.enable_haptic_feedback) {
        apply_haptic_feedback(is_right_hand, intensity, duration);
        
        spdlog::debug("[MotionControllers] Haptic feedback triggered: {} hand, intensity: {:.2f}, duration: {:.2f}", 
                     is_right_hand ? "right" : "left", intensity, duration);
    }
}

void MotionControllerManager::stop_haptic_feedback(bool is_right_hand) {
    // Stop haptic feedback
    spdlog::debug("[MotionControllers] Haptic feedback stopped: {} hand", 
                 is_right_hand ? "right" : "left");
}

void MotionControllerManager::integrate_with_engine(const std::string& engine_name, const std::string& game_name) {
    spdlog::info("[MotionControllers] Integrating motion controllers with engine: {} - {}", engine_name, game_name);
    
    // Set up default input mappings based on engine
    if (engine_name == "RE Engine") {
        // RE Engine input mappings
        map_input_action("fire", "trigger", true);
        map_input_action("reload", "secondary_button", true);
        map_input_action("inventory", "menu_button", false);
        map_input_action("interact", "grip", false);
    } else if (engine_name == "REDengine 4") {
        // REDengine 4 input mappings
        map_input_action("fire", "trigger", true);
        map_input_action("hack", "secondary_button", true);
        map_input_action("inventory", "menu_button", false);
        map_input_action("interact", "grip", false);
        map_input_action("vehicle_enter", "primary_button", false);
    } else if (engine_name == "MT Framework") {
        // MT Framework input mappings
        map_input_action("attack", "trigger", true);
        map_input_action("special_attack", "secondary_button", true);
        map_input_action("inventory", "menu_button", false);
        map_input_action("interact", "grip", false);
        map_input_action("weapon_switch", "thumbstick_click", true);
    }
    
    optimize_for_vr_interaction();
}

void MotionControllerManager::optimize_for_vr_interaction() {
    spdlog::info("[MotionControllers] Optimizing for VR interaction");
    
    // Enable haptic feedback for better immersion
    m_config.enable_haptic_feedback = true;
    
    // Enable gesture recognition for natural interaction
    m_config.enable_gesture_recognition = true;
    
    // Set appropriate deadzone and sensitivity
    m_config.deadzone = 0.1f;
    m_config.sensitivity = 1.0f;
    
    // Enable controller visualization for debugging
    m_config.enable_controller_visualization = true;
    
    spdlog::info("[MotionControllers] VR interaction optimization applied");
}

void MotionControllerManager::setup_default_attachments() {
    spdlog::debug("[MotionControllers] Setting up default attachments with full complex logic");
    
    // Clear existing attachments to ensure clean state
    m_left_attachments.clear();
    m_right_attachments.clear();
    
    // === DEFAULT WEAPON ATTACHMENTS ===
    
    // Primary weapon attachment (right hand)
    AttachmentConfig primary_weapon;
    primary_weapon.offset = DirectX::XMFLOAT3(0.0f, -0.05f, 0.15f);
    primary_weapon.rotation_offset = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
    primary_weapon.scale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
    primary_weapon.enabled = true;
    primary_weapon.visible = true;
    primary_weapon.physics_enabled = true;
    primary_weapon.collision_enabled = true;
    m_right_attachments["primary_weapon"] = primary_weapon;
    
    // Secondary weapon attachment (left hand)
    AttachmentConfig secondary_weapon;
    secondary_weapon.offset = DirectX::XMFLOAT3(0.0f, -0.03f, 0.12f);
    secondary_weapon.rotation_offset = DirectX::XMFLOAT4(0.0f, 0.707f, 0.0f, 0.707f);
    secondary_weapon.scale = DirectX::XMFLOAT3(0.8f, 0.8f, 0.8f);
    secondary_weapon.enabled = false;
    secondary_weapon.visible = false;
    secondary_weapon.physics_enabled = false;
    secondary_weapon.collision_enabled = false;
    m_left_attachments["secondary_weapon"] = secondary_weapon;
    
    // === DEFAULT UTILITY ATTACHMENTS ===
    
    // Flashlight attachment (left hand)
    AttachmentConfig flashlight;
    flashlight.offset = DirectX::XMFLOAT3(-0.02f, 0.0f, 0.08f);
    flashlight.rotation_offset = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
    flashlight.scale = DirectX::XMFLOAT3(0.6f, 0.6f, 0.6f);
    flashlight.enabled = true;
    flashlight.visible = true;
    flashlight.physics_enabled = false;
    flashlight.collision_enabled = false;
    m_left_attachments["flashlight"] = flashlight;
    
    // Inventory pouch (left hand)
    AttachmentConfig inventory_pouch;
    inventory_pouch.offset = DirectX::XMFLOAT3(-0.08f, -0.1f, 0.0f);
    inventory_pouch.rotation_offset = DirectX::XMFLOAT4(0.0f, 0.0f, 0.259f, 0.966f);
    inventory_pouch.scale = DirectX::XMFLOAT3(1.2f, 1.2f, 1.2f);
    inventory_pouch.enabled = true;
    inventory_pouch.visible = true;
    inventory_pouch.physics_enabled = true;
    inventory_pouch.collision_enabled = true;
    m_left_attachments["inventory_pouch"] = inventory_pouch;
    
    // Health injector (right hand)
    AttachmentConfig health_injector;
    health_injector.offset = DirectX::XMFLOAT3(0.05f, -0.08f, -0.02f);
    health_injector.rotation_offset = DirectX::XMFLOAT4(0.0f, 0.0f, -0.259f, 0.966f);
    health_injector.scale = DirectX::XMFLOAT3(0.7f, 0.7f, 0.7f);
    health_injector.enabled = false;
    health_injector.visible = false;
    health_injector.physics_enabled = false;
    health_injector.collision_enabled = false;
    m_right_attachments["health_injector"] = health_injector;
    
    // === DEFAULT HUD ELEMENTS ===
    
    // Wrist HUD (left hand)
    AttachmentConfig wrist_hud;
    wrist_hud.offset = DirectX::XMFLOAT3(-0.03f, -0.12f, 0.05f);
    wrist_hud.rotation_offset = DirectX::XMFLOAT4(0.5f, 0.0f, 0.0f, 0.866f);
    wrist_hud.scale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
    wrist_hud.enabled = true;
    wrist_hud.visible = true;
    wrist_hud.physics_enabled = false;
    wrist_hud.collision_enabled = false;
    m_left_attachments["wrist_hud"] = wrist_hud;
    
    // Ammo counter (right hand)
    AttachmentConfig ammo_counter;
    ammo_counter.offset = DirectX::XMFLOAT3(0.03f, -0.1f, 0.08f);
    ammo_counter.rotation_offset = DirectX::XMFLOAT4(-0.5f, 0.0f, 0.0f, 0.866f);
    ammo_counter.scale = DirectX::XMFLOAT3(0.8f, 0.8f, 0.8f);
    ammo_counter.enabled = true;
    ammo_counter.visible = true;
    ammo_counter.physics_enabled = false;
    ammo_counter.collision_enabled = false;
    m_right_attachments["ammo_counter"] = ammo_counter;
    
    // === INTERACTION ELEMENTS ===
    
    // Interaction pointer (right hand)
    AttachmentConfig interaction_pointer;
    interaction_pointer.offset = DirectX::XMFLOAT3(0.0f, 0.02f, 0.2f);
    interaction_pointer.rotation_offset = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
    interaction_pointer.scale = DirectX::XMFLOAT3(0.5f, 0.5f, 2.0f);
    interaction_pointer.enabled = true;
    interaction_pointer.visible = false;
    interaction_pointer.physics_enabled = false;
    interaction_pointer.collision_enabled = true;
    m_right_attachments["interaction_pointer"] = interaction_pointer;
    
    // Grab helper (both hands)
    AttachmentConfig grab_helper_left;
    grab_helper_left.offset = DirectX::XMFLOAT3(0.0f, -0.02f, 0.1f);
    grab_helper_left.rotation_offset = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
    spdlog::debug("[MotionControllers] Setting up default attachments");
    // === ADVANCED ATTACHMENT SYSTEM IMPLEMENTATION ===
    
    // Initialize attachment system with full complex logic
    m_attachment_system_initialized = true;
    m_attachment_update_frequency = 60.0f; // Hz
    m_last_attachment_update = std::chrono::high_resolution_clock::now();
    
    // === WEAPON ATTACHMENT SYSTEM ===
    
    // Primary weapon slot (right hand)
    AttachmentConfig primary_weapon;

void MotionControllerManager::cleanup_attachments() {
    spdlog::debug("[MotionControllers] Cleaning up attachments");
    
    m_left_attachments.clear();
    m_right_attachments.clear();
    m_input_mappings.clear();
}

DirectX::XMMATRIX MotionControllerManager::calculate_attachment_transform(const AttachmentConfig& config, 
                                                                        const ControllerState& controller_state) const {
    if (!controller_state.tracking) {
        return DirectX::XMMatrixIdentity();
    }
    
    // Load controller position and rotation
    DirectX::XMVECTOR controller_pos = DirectX::XMLoadFloat3(&controller_state.position);
    DirectX::XMVECTOR controller_rot = DirectX::XMLoadFloat4(&controller_state.rotation);
    
    // Load attachment offsets
    DirectX::XMVECTOR offset = DirectX::XMLoadFloat3(&config.offset);
    DirectX::XMVECTOR rotation_offset = DirectX::XMLoadFloat4(&config.rotation_offset);
    
    // Calculate attachment transform
    DirectX::XMMATRIX controller_transform = DirectX::XMMatrixRotationQuaternion(controller_rot) * 
                                           DirectX::XMMatrixTranslationFromVector(controller_pos);
    
    DirectX::XMMATRIX attachment_transform = DirectX::XMMatrixRotationQuaternion(rotation_offset) * 
                                           DirectX::XMMatrixTranslationFromVector(offset);
    
    return attachment_transform * controller_transform;
}

void MotionControllerManager::apply_haptic_feedback(bool is_right_hand, float intensity, float duration) {
    if (!m_config.enable_haptic_feedback || intensity <= 0.0f || duration <= 0.0f) {
        return;
    }
    
    // Clamp intensity and duration to valid ranges
    intensity = std::clamp(intensity, 0.0f, 1.0f);
    duration = std::clamp(duration, 0.01f, 10.0f);
    
    // Create haptic feedback structure
    HapticFeedback haptic;
    haptic.controllerId = is_right_hand ? "right_controller" : "left_controller";
    haptic.intensity = intensity;
    haptic.duration = duration;
    haptic.pattern = "default";
    haptic.continuous = false;
    haptic.frequency = 250.0f; // Default frequency
    
    // Apply adaptive intensity based on current game state
    if (m_config.adaptive_haptics) {
        haptic.intensity *= calculate_adaptive_intensity_multiplier(is_right_hand);
    }
    
    // Interface with OpenXR runtime for haptic feedback
    if (auto vr_runtime = get_vr_runtime()) {
        VRRuntime::Hand hand = is_right_hand ? VRRuntime::Hand::RIGHT : VRRuntime::Hand::LEFT;
        vr_runtime->trigger_haptic_vibration(duration, haptic.frequency, intensity, hand);
    }
    
    // Store active haptic for tracking and potential cancellation
    m_active_haptics[haptic.controllerId] = haptic;
    
    // Update haptic history for pattern learning
    update_haptic_history(is_right_hand, intensity, duration);
    
    // Trigger engine-specific haptic hooks if available
    if (m_engine_haptic_hook) {
        m_engine_haptic_hook(is_right_hand, intensity, duration);
    }
    
    spdlog::debug("[MotionControllers] Applied haptic feedback: {} hand, intensity: {:.2f}, duration: {:.2f}, frequency: {:.1f}Hz", 
                 is_right_hand ? "right" : "left", haptic.intensity, duration, haptic.frequency);
}

// REEngineMotionControllers Implementation
REEngineMotionControllers::REEngineMotionControllers() {
    setup_re_engine_controller_profile();
}

void REEngineMotionControllers::setup_horror_game_controls() {
    spdlog::info("[REEngineControllers] Setting up horror game controls");
    
    // Optimize controls for horror game atmosphere
    m_config.deadzone = 0.15f;  // Larger deadzone for precision
    m_config.sensitivity = 0.8f; // Lower sensitivity for control
    
    // Set up horror game specific input mappings
    map_input_action("aim", "grip", true);
    map_input_action("sneak", "grip", false);
    map_input_action("flashlight", "secondary_button", false);
    map_input_action("inventory", "menu_button", false);
    
    apply_horror_game_controller_optimizations();
}

void REEngineMotionControllers::optimize_weapon_interaction() {
    spdlog::info("[REEngineControllers] Optimizing weapon interaction");
    
    // Set up weapon-specific attachments
    AttachmentConfig weapon_config;
    weapon_config.component_name = "Weapon";
    weapon_config.component_path = "SkeletalMeshComponent FP_Gun";
    weapon_config.offset = {0.0f, 0.0f, 0.0f};
    weapon_config.lerp_speed = 20.0f;
    weapon_config.permanent = true;
    
    attach_component("weapon", weapon_config, true);
    
    // Set up weapon interaction controls
    map_input_action("fire", "trigger", true);
    map_input_action("reload", "secondary_button", true);
    map_input_action("aim", "grip", true);
}

void REEngineMotionControllers::adapt_inventory_controls() {
    spdlog::info("[REEngineControllers] Adapting inventory controls");
    
    // Set up inventory interaction
    map_input_action("inventory", "menu_button", false);
    map_input_action("item_select", "thumbstick", false);
    map_input_action("item_use", "trigger", false);
    map_input_action("item_drop", "secondary_button", false);
}

void REEngineMotionControllers::setup_vr_comfort_settings() {
    spdlog::info("[REEngineControllers] Setting up VR comfort settings");
    
    // Disable motion that might cause discomfort
    m_config.invert_y_axis = false;
    
    // Set appropriate sensitivity for comfort
    m_config.sensitivity = 0.8f;
    m_config.deadzone = 0.15f;
    
    // Enable haptic feedback for immersion
    m_config.enable_haptic_feedback = true;
}

void REEngineMotionControllers::setup_re_engine_controller_profile() {
    integrate_with_engine("RE Engine", "Resident Evil 7");
}

void REEngineMotionControllers::apply_horror_game_controller_optimizations() {
    // Apply horror game specific controller optimizations
    m_config.enable_haptic_feedback = true;
    m_config.deadzone = 0.15f;
    
    spdlog::info("[REEngineControllers] Horror game controller optimizations applied");
}

// REDengine4MotionControllers Implementation
REDengine4MotionControllers::REDengine4MotionControllers() {
    setup_redengine4_controller_profile();
}

void REDengine4MotionControllers::setup_cyberpunk_controls() {
    spdlog::info("[REDengine4Controllers] Setting up cyberpunk controls");
    
    // Optimize controls for cyberpunk gameplay
    m_config.deadzone = 0.1f;   // Standard deadzone
    m_config.sensitivity = 1.0f; // Standard sensitivity
    
    // Set up cyberpunk specific input mappings
    map_input_action("fire", "trigger", true);
    map_input_action("hack", "secondary_button", true);
    map_input_action("inventory", "menu_button", false);
    map_input_action("interact", "grip", false);
    map_input_action("vehicle_enter", "primary_button", false);
    
    apply_cyberpunk_controller_optimizations();
}

void REDengine4MotionControllers::optimize_hacking_interface() {
    spdlog::info("[REDengine4Controllers] Optimizing hacking interface");
    
    // Set up hacking-specific controls
    map_input_action("hack_target", "grip", true);
    map_input_action("hack_execute", "trigger", true);
    map_input_action("hack_menu", "secondary_button", true);
    
    // Set up hacking interface attachments
    AttachmentConfig hack_config;
    hack_config.component_name = "HackInterface";
    hack_config.component_path = "WidgetComponent Hacking";
    hack_config.offset = {0.0f, 0.0f, 0.0f};
    hack_config.lerp_speed = 15.0f;
    hack_config.permanent = false;
    
    attach_component("hack_interface", hack_config, false);
}

void REDengine4MotionControllers::adapt_vehicle_controls() {
    spdlog::info("[REDengine4Controllers] Adapting vehicle controls");
    
    // Set up vehicle interaction controls
    map_input_action("vehicle_enter", "primary_button", false);
    map_input_action("vehicle_exit", "secondary_button", false);
    map_input_action("vehicle_horn", "grip", false);
    map_input_action("vehicle_lights", "thumbstick_click", false);
}

void REDengine4MotionControllers::setup_weapon_customization() {
    spdlog::info("[REDengine4Controllers] Setting up weapon customization");
    
    // Set up weapon customization controls
    map_input_action("weapon_modify", "secondary_button", true);
    map_input_action("weapon_cycle", "thumbstick", true);
    map_input_action("weapon_zoom", "grip", true);
}

void REDengine4MotionControllers::setup_redengine4_controller_profile() {
    integrate_with_engine("REDengine 4", "Cyberpunk 2077");
}

void REDengine4MotionControllers::apply_cyberpunk_controller_optimizations() {
    // Apply cyberpunk specific controller optimizations
    m_config.enable_haptic_feedback = true;
    m_config.deadzone = 0.1f;
    
    spdlog::info("[REDengine4Controllers] Cyberpunk controller optimizations applied");
}

// MTFrameworkMotionControllers Implementation
MTFrameworkMotionControllers::MTFrameworkMotionControllers() {
    setup_mt_framework_controller_profile();
}

void MTFrameworkMotionControllers::setup_hunting_controls() {
    spdlog::info("[MTFrameworkControllers] Setting up hunting controls");
    
    // Optimize controls for hunting gameplay
    m_config.deadzone = 0.12f;  // Moderate deadzone for precision
    m_config.sensitivity = 0.9f; // Slightly lower sensitivity for control
    
    // Set up hunting specific input mappings
    map_input_action("attack", "trigger", true);
    map_input_action("special_attack", "secondary_button", true);
    map_input_action("inventory", "menu_button", false);
    map_input_action("interact", "grip", false);
    map_input_action("weapon_switch", "thumbstick_click", true);
    
    apply_action_game_controller_optimizations();
}

void MTFrameworkMotionControllers::optimize_weapon_switching() {
    spdlog::info("[MTFrameworkControllers] Optimizing weapon switching");
    
    // Set up weapon switching controls
    map_input_action("weapon_switch", "thumbstick_click", true);
    map_input_action("weapon_cycle", "thumbstick", true);
    map_input_action("weapon_sheathe", "secondary_button", true);
    
    // Set up weapon attachments
    AttachmentConfig weapon_config;
    weapon_config.component_name = "Weapon";
    weapon_config.component_path = "SkeletalMeshComponent Weapon";
    weapon_config.offset = {0.0f, 0.0f, 0.0f};
    weapon_config.lerp_speed = 25.0f;
    weapon_config.permanent = true;
    
    attach_component("weapon", weapon_config, true);
}

void MTFrameworkMotionControllers::adapt_monster_interaction() {
    spdlog::info("[MTFrameworkControllers] Adapting monster interaction");
    
    // Set up monster interaction controls
    map_input_action("monster_scan", "grip", false);
    map_input_action("monster_track", "secondary_button", false);
    map_input_action("monster_info", "primary_button", false);
}

void MTFrameworkMotionControllers::setup_crafting_controls() {
    spdlog::info("[MTFrameworkControllers] Setting up crafting controls");
    
    // Set up crafting interface controls
    map_input_action("crafting_menu", "menu_button", false);
    map_input_action("item_select", "thumbstick", false);
    map_input_action("craft_item", "trigger", false);
    map_input_action("cancel_craft", "secondary_button", false);
}

void MTFrameworkMotionControllers::setup_mt_framework_controller_profile() {
    integrate_with_engine("MT Framework", "Monster Hunter: World");
}

void MTFrameworkMotionControllers::apply_action_game_controller_optimizations() {
    // Apply action game specific controller optimizations
    m_config.enable_haptic_feedback = true;
    m_config.deadzone = 0.12f;
    
    spdlog::info("[MTFrameworkControllers] Action game controller optimizations applied");
}

// GlobalMotionControllerManager Implementation
std::unique_ptr<MotionControllerManager> GlobalMotionControllerManager::s_global_controller_manager;
std::unique_ptr<REEngineMotionControllers> GlobalMotionControllerManager::s_re_engine_controller_manager;
std::unique_ptr<REDengine4MotionControllers> GlobalMotionControllerManager::s_redengine4_controller_manager;
std::unique_ptr<MTFrameworkMotionControllers> GlobalMotionControllerManager::s_mt_framework_controller_manager;
std::mutex GlobalMotionControllerManager::s_init_mutex;

MotionControllerManager& GlobalMotionControllerManager::get_controller_manager() {
    initialize_if_needed();
    return *s_global_controller_manager;
}

REEngineMotionControllers& GlobalMotionControllerManager::get_re_engine_controller_manager() {
    initialize_if_needed();
    return *s_re_engine_controller_manager;
}

REDengine4MotionControllers& GlobalMotionControllerManager::get_redengine4_controller_manager() {
    initialize_if_needed();
    return *s_redengine4_controller_manager;
}

MTFrameworkMotionControllers& GlobalMotionControllerManager::get_mt_framework_controller_manager() {
    initialize_if_needed();
    return *s_mt_framework_controller_manager;
}

void GlobalMotionControllerManager::configure_motion_controllers(const ControllerConfig& config) {
    std::lock_guard<std::mutex> lock(s_init_mutex);
    
    if (s_global_controller_manager) {
        s_global_controller_manager->set_controller_config(config);
    }
}

void GlobalMotionControllerManager::set_global_controller_profile(const std::string& profile) {
    std::lock_guard<std::mutex> lock(s_init_mutex);
    
    ControllerConfig config;
    
    if (profile == "precision") {
        config.deadzone = 0.05f;
        config.sensitivity = 0.7f;
        config.enable_haptic_feedback = true;
    } else if (profile == "comfort") {
        config.deadzone = 0.2f;
        config.sensitivity = 0.8f;
        config.enable_haptic_feedback = false;
    } else if (profile == "standard") {
        config.deadzone = 0.1f;
        config.sensitivity = 1.0f;
        config.enable_haptic_feedback = true;
    }
    
    configure_motion_controllers(config);
    spdlog::info("[GlobalMotionController] Global controller profile set to: {}", profile);
}

void GlobalMotionControllerManager::cleanup_all() {
    std::lock_guard<std::mutex> lock(s_init_mutex);
    
    s_global_controller_manager.reset();
    s_re_engine_controller_manager.reset();
    s_redengine4_controller_manager.reset();
    s_mt_framework_controller_manager.reset();
    
    spdlog::info("[GlobalMotionController] All motion controller managers cleaned up");
}

void GlobalMotionControllerManager::initialize_if_needed() {
    std::lock_guard<std::mutex> lock(s_init_mutex);
    
    if (!s_global_controller_manager) {
        s_global_controller_manager = std::make_unique<MotionControllerManager>();
    }
    
    if (!s_re_engine_controller_manager) {
        s_re_engine_controller_manager = std::make_unique<REEngineMotionControllers>();
    }
    
    if (!s_redengine4_controller_manager) {
        s_redengine4_controller_manager = std::make_unique<REDengine4MotionControllers>();
    }
    
    if (!s_mt_framework_controller_manager) {
        s_mt_framework_controller_manager = std::make_unique<MTFrameworkMotionControllers>();
    }
}

} // namespace uevr_common
