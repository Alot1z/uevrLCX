/*
 * Cross-Engine UI Projection System Implementation
 * 
 * Implementation of engine-specific UI projection and HUD rendering
 * for all cross-engine VR adapters.
 * 
 * Copyright (c) 2024 UEVR Cross-Engine Adapter Project
 * Licensed under MIT License
 */

#include "ui_projection.hpp"
#include <spdlog/spdlog.h>
#include <algorithm>

namespace uevr_common {

// UIProjectionManager Implementation
UIProjectionManager::UIProjectionManager() {
    setup_ui_rendering_pipeline();
    spdlog::info("[UIProjection] UI projection manager initialized");
}

UIProjectionManager::~UIProjectionManager() {
    cleanup_ui_rendering_pipeline();
    spdlog::info("[UIProjection] UI projection manager destroyed");
}

void UIProjectionManager::set_ui_projection_config(const UIProjectionConfig& config) {
    m_config = config;
    spdlog::info("[UIProjection] UI projection configuration updated");
}

const UIProjectionManager::UIProjectionConfig& UIProjectionManager::get_ui_projection_config() const {
    return m_config;
}

void UIProjectionManager::add_hud_element(const HUDElement& element) {
    // Check if element already exists
    auto it = m_hud_element_map.find(element.name);
    if (it != m_hud_element_map.end()) {
        // Update existing element
        m_hud_elements[it->second] = element;
        spdlog::debug("[UIProjection] Updated HUD element: {}", element.name);
    } else {
        // Add new element
        m_hud_element_map[element.name] = m_hud_elements.size();
        m_hud_elements.push_back(element);
        spdlog::debug("[UIProjection] Added HUD element: {} ({})", element.name, element.type);
    }
}

void UIProjectionManager::remove_hud_element(const std::string& name) {
    auto it = m_hud_element_map.find(name);
    if (it != m_hud_element_map.end()) {
        size_t index = it->second;
        m_hud_elements.erase(m_hud_elements.begin() + index);
        m_hud_element_map.erase(it);
        
        // Update indices for remaining elements
        for (auto& pair : m_hud_element_map) {
            if (pair.second > index) {
                pair.second--;
            }
        }
        
        spdlog::debug("[UIProjection] Removed HUD element: {}", name);
    }
}

void UIProjectionManager::update_hud_element(const std::string& name, const HUDElement& element) {
    auto it = m_hud_element_map.find(name);
    if (it != m_hud_element_map.end()) {
        m_hud_elements[it->second] = element;
        spdlog::debug("[UIProjection] Updated HUD element: {}", name);
    }
}

const UIProjectionManager::HUDElement* UIProjectionManager::get_hud_element(const std::string& name) const {
    auto it = m_hud_element_map.find(name);
    if (it != m_hud_element_map.end()) {
        return &m_hud_elements[it->second];
    }
    return nullptr;
}

void UIProjectionManager::set_ui_rendering_state(const UIRenderingState& state) {
    m_state = state;
    spdlog::info("[UIProjection] UI rendering state updated");
}

const UIProjectionManager::UIRenderingState& UIProjectionManager::get_ui_rendering_state() const {
    return m_state;
}

DirectX::XMMATRIX UIProjectionManager::calculate_projection_matrix(const DirectX::XMFLOAT3& camera_position, 
                                                                 const DirectX::XMFLOAT3& camera_direction) const {
    DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&camera_position);
    DirectX::XMVECTOR dir = DirectX::XMLoadFloat3(&camera_direction);
    
    // Calculate UI position based on camera
    DirectX::XMVECTOR ui_pos = pos + dir * m_config.distance;
    
    // Create projection matrix based on type
    switch (m_config.type) {
        case UIProjectionConfig::ProjectionType::Billboard:
            return create_billboard_matrix(DirectX::XMFLOAT3{}, camera_position);
        case UIProjectionConfig::ProjectionType::Cylindrical:
            return create_cylindrical_matrix(DirectX::XMFLOAT3{}, camera_position);
        case UIProjectionConfig::ProjectionType::Spherical:
            return create_spherical_matrix(DirectX::XMFLOAT3{}, camera_position);
        case UIProjectionConfig::ProjectionType::Fixed:
            return DirectX::XMMatrixTranslation(m_config.offset.x, m_config.offset.y, m_config.offset.z);
        case UIProjectionConfig::ProjectionType::FollowView:
            return DirectX::XMMatrixTranslation(ui_pos.m128_f32[0], ui_pos.m128_f32[1], ui_pos.m128_f32[2]);
        default:
            return DirectX::XMMatrixIdentity();
    }
}

DirectX::XMFLOAT3 UIProjectionManager::calculate_ui_position(const DirectX::XMFLOAT3& world_position,
                                                           const DirectX::XMFLOAT3& camera_position) const {
    DirectX::XMVECTOR world_pos = DirectX::XMLoadFloat3(&world_position);
    DirectX::XMVECTOR cam_pos = DirectX::XMLoadFloat3(&camera_position);
    
    // Calculate direction from camera to world position
    DirectX::XMVECTOR direction = DirectX::XMVector3Normalize(world_pos - cam_pos);
    
    // Apply distance and offset
    DirectX::XMVECTOR ui_pos = cam_pos + direction * m_config.distance;
    ui_pos += DirectX::XMLoadFloat3(&m_config.offset);
    
    DirectX::XMFLOAT3 result;
    DirectX::XMStoreFloat3(&result, ui_pos);
    return result;
}

void UIProjectionManager::render_hud_elements(ID3D11DeviceContext* context, ID3D11Texture2D* render_target) {
    if (!m_state.hud_enabled) {
        return;
    }
    
    spdlog::debug("[UIProjection] Rendering {} HUD elements (D3D11)", m_hud_elements.size());
    
    // TODO: Implement D3D11 HUD rendering
    // This would include:
    // 1. Setting up render states
    // 2. Creating vertex buffers for UI elements
    // 3. Applying UI projection matrices
    // 4. Rendering each HUD element
}

void UIProjectionManager::render_hud_elements_d3d12(ID3D12GraphicsCommandList* command_list, ID3D12Resource* render_target) {
    if (!m_state.hud_enabled) {
        return;
    }
    
    spdlog::debug("[UIProjection] Rendering {} HUD elements (D3D12)", m_hud_elements.size());
    
    // TODO: Implement D3D12 HUD rendering
    // This would include:
    // 1. Setting up render states
    // 2. Creating vertex buffers for UI elements
    // 3. Applying UI projection matrices
    // 4. Rendering each HUD element
}

void UIProjectionManager::adapt_ui_for_engine(const std::string& engine_name, const std::string& game_name) {
    spdlog::info("[UIProjection] Adapting UI for engine: {} - {}", engine_name, game_name);
    
    // Set default HUD elements based on engine
    if (engine_name == "RE Engine") {
        // RE Engine specific HUD elements
        add_hud_element({"health_bar", "progress_bar", {0.1f, 0.9f}, {0.2f, 0.05f}, true, false, "", {"health"}});
        add_hud_element({"ammo_counter", "text", {0.9f, 0.9f}, {0.1f, 0.05f}, true, false, "", {"ammo"}});
        add_hud_element({"inventory", "grid", {0.5f, 0.5f}, {0.4f, 0.4f}, false, true, "", {"items"}});
    } else if (engine_name == "REDengine 4") {
        // REDengine 4 specific HUD elements
        add_hud_element({"health_bar", "progress_bar", {0.1f, 0.9f}, {0.2f, 0.05f}, true, false, "", {"health"}});
        add_hud_element({"stamina_bar", "progress_bar", {0.1f, 0.85f}, {0.2f, 0.03f}, true, false, "", {"stamina"}});
        add_hud_element({"minimap", "overlay", {0.8f, 0.1f}, {0.18f, 0.18f}, true, false, "", {"map"}});
        add_hud_element({"quest_log", "list", {0.7f, 0.3f}, {0.28f, 0.4f}, false, true, "", {"quests"}});
    } else if (engine_name == "MT Framework") {
        // MT Framework specific HUD elements
        add_hud_element({"health_bar", "progress_bar", {0.1f, 0.9f}, {0.2f, 0.05f}, true, false, "", {"health"}});
        add_hud_element({"stamina_bar", "progress_bar", {0.1f, 0.85f}, {0.2f, 0.03f}, true, false, "", {"stamina"}});
        add_hud_element({"sharpness_meter", "progress_bar", {0.1f, 0.8f}, {0.2f, 0.03f}, true, false, "", {"sharpness"}});
        add_hud_element({"monster_info", "panel", {0.7f, 0.1f}, {0.28f, 0.2f}, true, false, "", {"monster"}});
        add_hud_element({"item_bar", "grid", {0.5f, 0.9f}, {0.3f, 0.08f}, true, true, "", {"items"}});
    }
    
    optimize_ui_for_vr();
}

void UIProjectionManager::optimize_ui_for_vr() {
    spdlog::info("[UIProjection] Optimizing UI for VR");
    
    // Adjust UI projection for VR comfort
    m_config.distance = 2.0f; // Optimal distance for VR
    m_config.scale = 1.2f;    // Slightly larger for VR readability
    
    // Enable alpha blending for better VR integration
    m_config.enable_alpha_blending = true;
    m_config.enable_depth_test = false; // UI should always be on top
    
    // Set billboard projection for best VR readability
    m_config.type = UIProjectionConfig::ProjectionType::Billboard;
    
    spdlog::info("[UIProjection] VR optimization applied: distance={}, scale={}, type=Billboard", 
                 m_config.distance, m_config.scale);
}

void UIProjectionManager::setup_ui_rendering_pipeline() {
    spdlog::debug("[UIProjection] Setting up UI rendering pipeline");
    
    // TODO: Initialize UI rendering pipeline
    // This would include:
    // 1. Creating shaders for UI rendering
    // 2. Setting up vertex buffers and index buffers
    // 3. Creating render states
    // 4. Setting up texture samplers
}

void UIProjectionManager::cleanup_ui_rendering_pipeline() {
    spdlog::debug("[UIProjection] Cleaning up UI rendering pipeline");
    
    // TODO: Clean up UI rendering resources
    // This would include:
    // 1. Releasing shaders
    // 2. Releasing vertex buffers and index buffers
    // 3. Releasing render states
    // 4. Releasing texture samplers
}

DirectX::XMMATRIX UIProjectionManager::create_billboard_matrix(const DirectX::XMFLOAT3& position,
                                                             const DirectX::XMFLOAT3& camera_position) const {
    DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&position);
    DirectX::XMVECTOR cam_pos = DirectX::XMLoadFloat3(&camera_position);
    
    // Calculate direction from camera to position
    DirectX::XMVECTOR direction = DirectX::XMVector3Normalize(pos - cam_pos);
    
    // Create billboard matrix that always faces the camera
    DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    DirectX::XMVECTOR right = DirectX::XMVector3Cross(up, direction);
    up = DirectX::XMVector3Cross(direction, right);
    
    DirectX::XMMATRIX rotation = DirectX::XMMatrixSet(
        right.m128_f32[0], right.m128_f32[1], right.m128_f32[2], 0.0f,
        up.m128_f32[0], up.m128_f32[1], up.m128_f32[2], 0.0f,
        direction.m128_f32[0], direction.m128_f32[1], direction.m128_f32[2], 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );
    
    return DirectX::XMMatrixScaling(m_config.scale, m_config.scale, m_config.scale) * 
           rotation * 
           DirectX::XMMatrixTranslation(position.x, position.y, position.z);
}

DirectX::XMMATRIX UIProjectionManager::create_cylindrical_matrix(const DirectX::XMFLOAT3& position,
                                                                const DirectX::XMFLOAT3& camera_position) const {
    // Simplified cylindrical projection
    DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&position);
    DirectX::XMVECTOR cam_pos = DirectX::XMLoadFloat3(&camera_position);
    
    // Calculate angle around Y axis
    DirectX::XMVECTOR diff = pos - cam_pos;
    float angle = atan2f(diff.m128_f32[0], diff.m128_f32[2]);
    
    // Create cylindrical projection matrix
    DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationY(angle);
    DirectX::XMMATRIX translation = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
    
    return DirectX::XMMatrixScaling(m_config.scale, m_config.scale, m_config.scale) * 
           rotation * 
           translation;
}

DirectX::XMMATRIX UIProjectionManager::create_spherical_matrix(const DirectX::XMFLOAT3& position,
                                                               const DirectX::XMFLOAT3& camera_position) const {
    // Simplified spherical projection
    DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&position);
    DirectX::XMVECTOR cam_pos = DirectX::XMLoadFloat3(&camera_position);
    
    // Calculate spherical coordinates
    DirectX::XMVECTOR diff = pos - cam_pos;
    float distance = DirectX::XMVectorGetX(DirectX::XMVector3Length(diff));
    float theta = atan2f(diff.m128_f32[1], sqrtf(diff.m128_f32[0] * diff.m128_f32[0] + diff.m128_f32[2] * diff.m128_f32[2]));
    float phi = atan2f(diff.m128_f32[0], diff.m128_f32[2]);
    
    // Create spherical projection matrix
    DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationY(phi) * DirectX::XMMatrixRotationX(theta);
    DirectX::XMMATRIX translation = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
    
    return DirectX::XMMatrixScaling(m_config.scale, m_config.scale, m_config.scale) * 
           rotation * 
           translation;
}

// REEngineUIProjection Implementation
REEngineUIProjection::REEngineUIProjection() {
    setup_re_engine_ui_profile();
}

void REEngineUIProjection::optimize_horror_game_ui() {
    spdlog::info("[REEngineUI] Optimizing horror game UI");
    
    // Optimize UI for horror game atmosphere
    m_config.distance = 1.8f;  // Closer for immersion
    m_config.scale = 1.1f;     // Slightly larger for readability in dark scenes
    
    // Use billboard projection for consistent readability
    m_config.type = UIProjectionConfig::ProjectionType::Billboard;
    
    // Enable alpha blending for atmospheric effects
    m_config.enable_alpha_blending = true;
    
    apply_horror_game_ui_optimizations();
}

void REEngineUIProjection::adjust_ui_for_fixed_cameras() {
    spdlog::info("[REEngineUI] Adjusting UI for fixed cameras");
    
    // Since RE Engine uses fixed cameras, adjust UI positioning
    m_config.follow_view = false;
    m_config.type = UIProjectionConfig::ProjectionType::Fixed;
    
    // Position UI elements relative to camera view
    m_state.hud_position = {0.0f, 0.0f, 1.5f};
    m_state.menu_position = {0.0f, 0.0f, 1.2f};
}

void REEngineUIProjection::optimize_hud_for_atmospheric_lighting() {
    spdlog::info("[REEngineUI] Optimizing HUD for atmospheric lighting");
    
    // Adjust HUD opacity for dark scenes
    m_state.hud_opacity = 0.9f;
    m_state.menu_opacity = 0.95f;
    
    // Ensure HUD elements are visible in low-light conditions
    for (auto& element : m_hud_elements) {
        if (element.type == "progress_bar" || element.type == "text") {
            element.properties.push_back("high_contrast");
        }
    }
}

void REEngineUIProjection::adapt_menu_system_for_vr() {
    spdlog::info("[REEngineUI] Adapting menu system for VR");
    
    // Optimize menu positioning for VR
    m_state.menu_position = {0.0f, 0.0f, 1.2f};
    
    // Add VR-specific menu elements
    add_hud_element({"vr_settings", "panel", {0.5f, 0.5f}, {0.4f, 0.6f}, false, true, "", {"vr_options"}});
    add_hud_element({"comfort_settings", "panel", {0.5f, 0.5f}, {0.4f, 0.6f}, false, true, "", {"comfort"}});
}

void REEngineUIProjection::setup_re_engine_ui_profile() {
    adapt_ui_for_engine("RE Engine", "Resident Evil 7");
}

void REEngineUIProjection::apply_horror_game_ui_optimizations() {
    // Apply horror game specific UI optimizations
    m_config.enable_alpha_blending = true;
    m_config.distance = 1.8f;
    
    spdlog::info("[REEngineUI] Horror game UI optimizations applied");
}

// REDengine4UIProjection Implementation
REDengine4UIProjection::REDengine4UIProjection() {
    setup_redengine4_ui_profile();
}

void REDengine4UIProjection::optimize_urban_environment_ui() {
    spdlog::info("[REDengine4UI] Optimizing urban environment UI");
    
    // Optimize UI for complex urban environments
    m_config.distance = 2.2f;  // Further for complex scenes
    m_config.scale = 1.0f;     // Standard scale for urban clarity
    
    // Use cylindrical projection for urban environments
    m_config.type = UIProjectionConfig::ProjectionType::Cylindrical;
    
    apply_cyberpunk_ui_optimizations();
}

void REDengine4UIProjection::adapt_cyberpunk_hud_for_vr() {
    spdlog::info("[REDengine4UI] Adapting cyberpunk HUD for VR");
    
    // Optimize cyberpunk HUD elements for VR
    m_state.hud_opacity = 0.85f;
    m_state.menu_opacity = 0.9f;
    
    // Add cyberpunk-specific HUD elements
    add_hud_element({"hack_interface", "overlay", {0.5f, 0.5f}, {0.6f, 0.6f}, false, true, "", {"hacking"}});
    add_hud_element({"augmentation_status", "panel", {0.8f, 0.1f}, {0.18f, 0.15f}, true, false, "", {"augs"}});
}

void REDengine4UIProjection::optimize_menu_system_for_complex_ui() {
    spdlog::info("[REDengine4UI] Optimizing menu system for complex UI");
    
    // Optimize for complex cyberpunk menus
    m_state.menu_position = {0.0f, 0.0f, 1.8f};
    
    // Add complex menu elements
    add_hud_element({"inventory_grid", "grid", {0.5f, 0.5f}, {0.7f, 0.7f}, false, true, "", {"items"}});
    add_hud_element({"skill_tree", "tree", {0.5f, 0.5f}, {0.8f, 0.8f}, false, true, "", {"skills"}});
}

void REDengine4UIProjection::adjust_notification_system() {
    spdlog::info("[REDengine4UI] Adjusting notification system");
    
    // Optimize notification positioning for VR
    m_state.notifications_enabled = true;
    
    // Add notification elements
    add_hud_element({"notification", "text", {0.5f, 0.1f}, {0.4f, 0.08f}, false, false, "", {"alert"}});
    add_hud_element({"quest_update", "text", {0.5f, 0.2f}, {0.4f, 0.08f}, false, false, "", {"quest"}});
}

void REDengine4UIProjection::setup_redengine4_ui_profile() {
    adapt_ui_for_engine("REDengine 4", "Cyberpunk 2077");
}

void REDengine4UIProjection::apply_cyberpunk_ui_optimizations() {
    // Apply cyberpunk specific UI optimizations
    m_config.enable_alpha_blending = true;
    m_config.distance = 2.2f;
    
    spdlog::info("[REDengine4UI] Cyberpunk UI optimizations applied");
}

// MTFrameworkUIProjection Implementation
MTFrameworkUIProjection::MTFrameworkUIProjection() {
    setup_mt_framework_ui_profile();
}

void MTFrameworkUIProjection::optimize_combat_hud_for_vr() {
    spdlog::info("[MTFrameworkUI] Optimizing combat HUD for VR");
    
    // Optimize UI for intense combat scenes
    m_config.distance = 1.5f;  // Closer for quick access during combat
    m_config.scale = 1.3f;     // Larger for quick reading during combat
    
    // Use billboard projection for combat clarity
    m_config.type = UIProjectionConfig::ProjectionType::Billboard;
    
    apply_action_game_ui_optimizations();
}

void MTFrameworkUIProjection::adapt_weapon_ui_for_motion_controllers() {
    spdlog::info("[MTFrameworkUI] Adapting weapon UI for motion controllers");
    
    // Optimize weapon UI for motion controller interaction
    m_state.hud_opacity = 0.9f;
    
    // Add weapon-specific UI elements
    add_hud_element({"weapon_status", "panel", {0.8f, 0.8f}, {0.18f, 0.15f}, true, false, "", {"weapon"}});
    add_hud_element({"ammo_display", "text", {0.8f, 0.7f}, {0.18f, 0.08f}, true, false, "", {"ammo"}});
}

void MTFrameworkUIProjection::optimize_monster_information_display() {
    spdlog::info("[MTFrameworkUI] Optimizing monster information display");
    
    // Optimize monster info display for VR
    add_hud_element({"monster_health", "progress_bar", {0.7f, 0.15f}, {0.28f, 0.03f}, true, false, "", {"monster_health"}});
    add_hud_element({"monster_status", "text", {0.7f, 0.12f}, {0.28f, 0.03f}, true, false, "", {"monster_status"}});
    add_hud_element({"hunt_timer", "text", {0.7f, 0.09f}, {0.28f, 0.03f}, true, false, "", {"timer"}});
}

void MTFrameworkUIProjection::adjust_inventory_system_for_vr() {
    spdlog::info("[MTFrameworkUI] Adjusting inventory system for VR");
    
    // Optimize inventory for VR interaction
    m_state.menu_opacity = 0.95f;
    
    // Add VR-optimized inventory elements
    add_hud_element({"item_grid", "grid", {0.5f, 0.5f}, {0.6f, 0.6f}, false, true, "", {"items"}});
    add_hud_element({"crafting_menu", "panel", {0.5f, 0.5f}, {0.5f, 0.6f}, false, true, "", {"crafting"}});
}

void MTFrameworkUIProjection::setup_mt_framework_ui_profile() {
    adapt_ui_for_engine("MT Framework", "Monster Hunter: World");
}

void MTFrameworkUIProjection::apply_action_game_ui_optimizations() {
    // Apply action game specific UI optimizations
    m_config.enable_alpha_blending = true;
    m_config.distance = 1.5f;
    
    spdlog::info("[MTFrameworkUI] Action game UI optimizations applied");
}

// GlobalUIProjectionManager Implementation
std::unique_ptr<UIProjectionManager> GlobalUIProjectionManager::s_global_ui_manager;
std::unique_ptr<REEngineUIProjection> GlobalUIProjectionManager::s_re_engine_ui_manager;
std::unique_ptr<REDengine4UIProjection> GlobalUIProjectionManager::s_redengine4_ui_manager;
std::unique_ptr<MTFrameworkUIProjection> GlobalUIProjectionManager::s_mt_framework_ui_manager;
std::mutex GlobalUIProjectionManager::s_init_mutex;

UIProjectionManager& GlobalUIProjectionManager::get_ui_manager() {
    initialize_if_needed();
    return *s_global_ui_manager;
}

REEngineUIProjection& GlobalUIProjectionManager::get_re_engine_ui_manager() {
    initialize_if_needed();
    return *s_re_engine_ui_manager;
}

REDengine4UIProjection& GlobalUIProjectionManager::get_redengine4_ui_manager() {
    initialize_if_needed();
    return *s_redengine4_ui_manager;
}

MTFrameworkUIProjection& GlobalUIProjectionManager::get_mt_framework_ui_manager() {
    initialize_if_needed();
    return *s_mt_framework_ui_manager;
}

void GlobalUIProjectionManager::configure_ui_projection(const UIProjectionConfig& config) {
    std::lock_guard<std::mutex> lock(s_init_mutex);
    
    if (s_global_ui_manager) {
        s_global_ui_manager->set_ui_projection_config(config);
    }
}

void GlobalUIProjectionManager::set_global_ui_style(const std::string& style) {
    std::lock_guard<std::mutex> lock(s_init_mutex);
    
    UIProjectionConfig config;
    
    if (style == "minimal") {
        config.distance = 1.5f;
        config.scale = 0.8f;
        config.type = UIProjectionConfig::ProjectionType::Billboard;
    } else if (style == "immersive") {
        config.distance = 2.5f;
        config.scale = 1.5f;
        config.type = UIProjectionConfig::ProjectionType::Cylindrical;
    } else if (style == "standard") {
        config.distance = 2.0f;
        config.scale = 1.0f;
        config.type = UIProjectionConfig::ProjectionType::Billboard;
    }
    
    configure_ui_projection(config);
    spdlog::info("[GlobalUIProjection] Global UI style set to: {}", style);
}

void GlobalUIProjectionManager::cleanup_all() {
    std::lock_guard<std::mutex> lock(s_init_mutex);
    
    s_global_ui_manager.reset();
    s_re_engine_ui_manager.reset();
    s_redengine4_ui_manager.reset();
    s_mt_framework_ui_manager.reset();
    
    spdlog::info("[GlobalUIProjection] All UI projection managers cleaned up");
}

void GlobalUIProjectionManager::initialize_if_needed() {
    std::lock_guard<std::mutex> lock(s_init_mutex);
    
    if (!s_global_ui_manager) {
        s_global_ui_manager = std::make_unique<UIProjectionManager>();
    }
    
    if (!s_re_engine_ui_manager) {
        s_re_engine_ui_manager = std::make_unique<REEngineUIProjection>();
    }
    
    if (!s_redengine4_ui_manager) {
        s_redengine4_ui_manager = std::make_unique<REDengine4UIProjection>();
    }
    
    if (!s_mt_framework_ui_manager) {
        s_mt_framework_ui_manager = std::make_unique<MTFrameworkUIProjection>();
    }
}

} // namespace uevr_common
