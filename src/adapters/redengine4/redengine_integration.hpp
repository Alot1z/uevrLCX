/*
 * REDengine 4 Cross-Engine VR Integration Header
 * Main interface for integrating REDengine 4 with uevr's cross-engine framework
 * 
 * Copyright (c) 2024 uevr Cross-Engine Adapter Project
 */

#pragma once

#include <windows.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <DirectXMath.h>
#include <memory>
#include <vector>
#include <string>

// uevr Integration includes
#include <uevr/Plugin.hpp>
#include <uevr/API.hpp>

using namespace DirectX;

namespace uevr {
namespace redengine4 {

// Main REDengine 4 integration interface
class REDengine4Integration {
public:
    // Core integration methods
    virtual bool initialize_integration() = 0;
    virtual void shutdown_integration() = 0;
    virtual bool is_integration_active() const = 0;
    
    // VR mode management
    virtual void enable_vr_mode() = 0;
    virtual void disable_vr_mode() = 0;
    virtual bool is_vr_mode_active() const = 0;
    
    // Camera system integration
    virtual bool extract_camera_matrices() = 0;
    virtual XMMATRIX get_view_matrix() const = 0;
    virtual XMMATRIX get_projection_matrix() const = 0;
    virtual XMMATRIX get_view_projection_matrix() const = 0;
    
    // Rendering pipeline integration
    virtual bool hook_rendering_pipeline() = 0;
    virtual void unhook_rendering_pipeline() = 0;
    virtual bool apply_vr_rendering(ID3D12GraphicsCommandList* command_list) = 0;
    
    // Performance optimization
    virtual void set_performance_profile(const std::string& profile) = 0;
    virtual void optimize_for_vr() = 0;
    virtual float get_current_fps() const = 0;
    
    // VR comfort settings
    virtual void set_comfort_settings(const std::string& settings) = 0;
    virtual void adjust_motion_comfort(float intensity) = 0;
    virtual void set_foveated_rendering(bool enabled, float strength) = 0;
    
    virtual ~REDengine4Integration() = default;
};

// REDengine 4 specific VR configuration
struct REDengine4VRConfig {
    // Camera settings
    float world_scale = 100.0f;
    float ipd = 0.064f;
    float near_clip = 0.1f;
    float far_clip = 2000.0f;
    float fov_y = 85.0f;
    
    // Performance settings
    bool dynamic_resolution = true;
    bool foveated_rendering = true;
    float foveated_rendering_strength = 0.7f;
    bool ray_tracing_optimization = true;
    
    // Comfort settings
    bool comfort_mode = true;
    float motion_blur_reduction = 0.6f;
    bool bright_light_adaptation = true;
    float comfort_radius = 0.8f;
    
    // Advanced features
    bool dlss_integration = true;
    bool ray_tracing_quality_balance = true;
    bool advanced_foveated_rendering = true;
};

// REDengine 4 performance profile
struct REDengine4PerformanceProfile {
    std::string name;
    float target_fps = 90.0f;
    float min_fps = 80.0f;
    float max_fps = 120.0f;
    
    // Resolution scaling
    float resolution_scale_min = 0.6f;
    float resolution_scale_max = 1.3f;
    float current_resolution_scale = 1.0f;
    
    // Quality settings
    std::string shadow_quality = "ultra";
    std::string post_processing_quality = "ultra";
    std::string texture_quality = "ultra";
    std::string anti_aliasing = "dlss";
    
    // Ray tracing
    bool ray_tracing_enabled = true;
    std::string ray_tracing_quality = "balanced";
    
    // Memory optimization
    size_t target_memory_usage = 4 * 1024 * 1024 * 1024ULL; // 4GB
    float memory_efficiency_target = 0.78f;
};

// REDengine 4 camera system integration
class REDengine4CameraIntegration {
public:
    virtual bool initialize_camera_system() = 0;
    virtual void shutdown_camera_system() = 0;
    
    // Matrix extraction
    virtual bool extract_view_matrix() = 0;
    virtual bool extract_projection_matrix() = 0;
    virtual bool extract_view_projection_matrix() = 0;
    
    // VR transformation
    virtual XMMATRIX create_vr_view_matrix() = 0;
    virtual XMMATRIX create_vr_projection_matrix(int eye_index) = 0;
    virtual XMMATRIX create_stereo_projection_matrix(float eye_offset) = 0;
    
    // Camera state
    virtual bool is_camera_active() const = 0;
    virtual XMVECTOR get_camera_position() const = 0;
    virtual XMVECTOR get_camera_direction() const = 0;
    
    virtual ~REDengine4CameraIntegration() = default;
};

// REDengine 4 rendering pipeline integration
class REDengine4RenderingIntegration {
public:
    virtual bool initialize_rendering_pipeline() = 0;
    virtual void shutdown_rendering_pipeline() = 0;
    
    // Pipeline hooks
    virtual bool hook_swap_chain() = 0;
    virtual bool hook_command_queue() = 0;
    virtual bool hook_command_list() = 0;
    
    // VR rendering
    virtual bool setup_vr_rendering_targets() = 0;
    virtual bool apply_stereo_rendering(ID3D12GraphicsCommandList* command_list) = 0;
    virtual bool post_process_vr_rendering(ID3D12GraphicsCommandList* command_list) = 0;
    
    // Performance monitoring
    virtual void begin_frame() = 0;
    virtual void end_frame() = 0;
    virtual float get_frame_time() const = 0;
    virtual float get_gpu_time() const = 0;
    
    virtual ~REDengine4RenderingIntegration() = default;
};

// REDengine 4 memory management integration
class REDengine4MemoryIntegration {
public:
    virtual bool initialize_memory_system() = 0;
    virtual void shutdown_memory_system() = 0;
    
    // Memory safety
    virtual bool validate_memory_access(void* address, size_t size) = 0;
    virtual bool is_memory_safe(void* address) const = 0;
    
    // Constant buffer management
    virtual bool analyze_constant_buffers() = 0;
    virtual bool find_camera_matrices_in_buffers() = 0;
    virtual bool update_constant_buffer_with_vr_data(const void* data, size_t size) = 0;
    
    // Memory optimization
    virtual void optimize_memory_usage() = 0;
    virtual size_t get_current_memory_usage() const = 0;
    virtual size_t get_peak_memory_usage() const = 0;
    
    virtual ~REDengine4MemoryIntegration() = default;
};

// Main REDengine 4 integration manager
class REDengine4IntegrationManager {
private:
    std::unique_ptr<REDengine4Integration> m_integration;
    std::unique_ptr<REDengine4CameraIntegration> m_camera_integration;
    std::unique_ptr<REDengine4RenderingIntegration> m_rendering_integration;
    std::unique_ptr<REDengine4MemoryIntegration> m_memory_integration;
    
    REDengine4VRConfig m_vr_config;
    REDengine4PerformanceProfile m_performance_profile;
    
    bool m_initialized = false;
    bool m_vr_mode_active = false;

public:
    REDengine4IntegrationManager();
    ~REDengine4IntegrationManager();
    
    // Core management
    bool initialize();
    void shutdown();
    bool is_initialized() const { return m_initialized; }
    
    // VR mode management
    bool enable_vr_mode();
    bool disable_vr_mode();
    bool is_vr_mode_active() const { return m_vr_mode_active; }
    
    // Configuration management
    void set_vr_config(const REDengine4VRConfig& config);
    const REDengine4VRConfig& get_vr_config() const { return m_vr_config; }
    
    void set_performance_profile(const REDengine4PerformanceProfile& profile);
    const REDengine4PerformanceProfile& get_performance_profile() const { return m_performance_profile; }
    
    // Integration access
    REDengine4Integration* get_integration() { return m_integration.get(); }
    REDengine4CameraIntegration* get_camera_integration() { return m_camera_integration.get(); }
    REDengine4RenderingIntegration* get_rendering_integration() { return m_rendering_integration.get(); }
    REDengine4MemoryIntegration* get_memory_integration() { return m_memory_integration.get(); }
    
    // Status and monitoring
    bool is_integration_healthy() const;
    std::string get_integration_status() const;
    void log_integration_status() const;
};

// Global integration manager instance
extern std::unique_ptr<REDengine4IntegrationManager> g_integration_manager;

// Integration initialization function
bool initialize_redengine4_integration();

// Integration shutdown function
void shutdown_redengine4_integration();

// Get global integration manager
REDengine4IntegrationManager* get_redengine4_integration_manager();

} // namespace redengine4
} // namespace uevr
