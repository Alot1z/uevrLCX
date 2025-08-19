/*
 * Cross-Engine UI Projection System for UEVR Adapters
 * 
 * This header provides engine-specific UI projection and HUD rendering
 * for all cross-engine VR adapters, ensuring optimal VR UI readability.
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
 * UI projection manager for cross-engine adapters
 * 
 * Provides engine-specific UI projection and HUD rendering
 * to ensure optimal VR UI readability across different engines.
 */
class UIProjectionManager {
public:
    // UI projection configuration
    struct UIProjectionConfig {
        enum class ProjectionType {
            Billboard,      // Always face the camera
            Cylindrical,    // Curved around the view
            Spherical,      // Curved in all directions
            Fixed,          // Fixed position in world space
            FollowView      // Follows head movement
        };
        
        ProjectionType type = ProjectionType::Billboard;
        float distance = 2.0f;
        float scale = 1.0f;
        bool follow_view = false;
        bool enable_depth_test = false;
        bool enable_alpha_blending = true;
        DirectX::XMFLOAT3 offset = {0.0f, 0.0f, 0.0f};
        DirectX::XMFLOAT4 rotation = {0.0f, 0.0f, 0.0f, 1.0f}; // Quaternion
    };

    // HUD element information
    struct HUDElement {
        std::string name;
        std::string type;
        DirectX::XMFLOAT2 position;
        DirectX::XMFLOAT2 size;
        bool visible = true;
        bool interactive = false;
        std::string texture_path;
        std::vector<std::string> properties;
    };

    // UI rendering state
    struct UIRenderingState {
        bool hud_enabled = true;
        bool menu_enabled = true;
        bool notifications_enabled = true;
        float hud_opacity = 1.0f;
        float menu_opacity = 1.0f;
        DirectX::XMFLOAT3 hud_position = {0.0f, 0.0f, 2.0f};
        DirectX::XMFLOAT3 menu_position = {0.0f, 0.0f, 1.5f};
    };

public:
    UIProjectionManager();
    ~UIProjectionManager();

    // Configuration management
    void set_ui_projection_config(const UIProjectionConfig& config);
    const UIProjectionConfig& get_ui_projection_config() const;
    
    // HUD element management
    void add_hud_element(const HUDElement& element);
    void remove_hud_element(const std::string& name);
    void update_hud_element(const std::string& name, const HUDElement& element);
    const HUDElement* get_hud_element(const std::string& name) const;
    
    // UI rendering state
    void set_ui_rendering_state(const UIRenderingState& state);
    const UIRenderingState& get_ui_rendering_state() const;
    
    // Projection calculations
    DirectX::XMMATRIX calculate_projection_matrix(const DirectX::XMFLOAT3& camera_position, 
                                                 const DirectX::XMFLOAT3& camera_direction) const;
    DirectX::XMFLOAT3 calculate_ui_position(const DirectX::XMFLOAT3& world_position,
                                           const DirectX::XMFLOAT3& camera_position) const;
    
    // Rendering
    void render_hud_elements(ID3D11DeviceContext* context, ID3D11Texture2D* render_target);
    void render_hud_elements_d3d12(ID3D12GraphicsCommandList* command_list, ID3D12Resource* render_target);
    
    // Engine-specific UI adaptation
    void adapt_ui_for_engine(const std::string& engine_name, const std::string& game_name);
    void optimize_ui_for_vr();

private:
    UIProjectionConfig m_config;
    UIRenderingState m_state;
    std::vector<HUDElement> m_hud_elements;
    std::unordered_map<std::string, size_t> m_hud_element_map;
    
    // Internal rendering helpers
    void setup_ui_rendering_pipeline();
    void cleanup_ui_rendering_pipeline();
    DirectX::XMMATRIX create_billboard_matrix(const DirectX::XMFLOAT3& position,
                                             const DirectX::XMFLOAT3& camera_position) const;
    DirectX::XMMATRIX create_cylindrical_matrix(const DirectX::XMFLOAT3& position,
                                                const DirectX::XMFLOAT3& camera_position) const;
    DirectX::XMMATRIX create_spherical_matrix(const DirectX::XMFLOAT3& position,
                                             const DirectX::XMFLOAT3& camera_position) const;
};

/**
 * RE Engine specific UI projection manager
 * 
 * Provides UI projection optimizations specific to Capcom's RE Engine games.
 */
class REEngineUIProjection : public UIProjectionManager {
public:
    REEngineUIProjection();
    
    // RE Engine specific UI adaptations
    void optimize_horror_game_ui();
    void adjust_ui_for_fixed_cameras();
    void optimize_hud_for_atmospheric_lighting();
    void adapt_menu_system_for_vr();
    
private:
    void setup_re_engine_ui_profile();
    void apply_horror_game_ui_optimizations();
};

/**
 * REDengine 4 specific UI projection manager
 * 
 * Provides UI projection optimizations specific to CD Projekt Red's REDengine 4 games.
 */
class REDengine4UIProjection : public UIProjectionManager {
public:
    REDengine4UIProjection();
    
    // REDengine 4 specific UI adaptations
    void optimize_urban_environment_ui();
    void adapt_cyberpunk_hud_for_vr();
    void optimize_menu_system_for_complex_ui();
    void adjust_notification_system();
    
private:
    void setup_redengine4_ui_profile();
    void apply_cyberpunk_ui_optimizations();
};

/**
 * MT Framework specific UI projection manager
 * 
 * Provides UI projection optimizations specific to Capcom's MT Framework games.
 */
class MTFrameworkUIProjection : public UIProjectionManager {
public:
    MTFrameworkUIProjection();
    
    // MT Framework specific UI adaptations
    void optimize_combat_hud_for_vr();
    void adapt_weapon_ui_for_motion_controllers();
    void optimize_monster_information_display();
    void adjust_inventory_system_for_vr();
    
private:
    void setup_mt_framework_ui_profile();
    void apply_action_game_ui_optimizations();
};

/**
 * Global UI projection manager
 * 
 * Provides centralized access to UI projection features.
 */
class GlobalUIProjectionManager {
public:
    static UIProjectionManager& get_ui_manager();
    static REEngineUIProjection& get_re_engine_ui_manager();
    static REDengine4UIProjection& get_redengine4_ui_manager();
    static MTFrameworkUIProjection& get_mt_framework_ui_manager();
    
    // Global configuration
    static void configure_ui_projection(const UIProjectionConfig& config);
    static void set_global_ui_style(const std::string& style);
    
    // Global cleanup
    static void cleanup_all();

private:
    static std::unique_ptr<UIProjectionManager> s_global_ui_manager;
    static std::unique_ptr<REEngineUIProjection> s_re_engine_ui_manager;
    static std::unique_ptr<REDengine4UIProjection> s_redengine4_ui_manager;
    static std::unique_ptr<MTFrameworkUIProjection> s_mt_framework_ui_manager;
    static std::mutex s_init_mutex;
    
    static void initialize_if_needed();
};

} // namespace uevr_common
