/*
 * Cross-Engine Performance Optimization System for UEVR Adapters
 * 
 * This header provides engine-specific performance tuning and optimization
 * for all cross-engine VR adapters, ensuring optimal VR performance.
 * 
 * Copyright (c) 2024 UEVR Cross-Engine Adapter Project
 * Licensed under MIT License
 */

#pragma once

#include <windows.h>
#include <d3d11.h>
#include <d3d12.h>
#include <memory>
#include <vector>
#include <unordered_map>
#include <string>
#include <chrono>

namespace uevr_common {

/**
 * Performance optimization manager for cross-engine adapters
 * 
 * Provides engine-specific performance tuning and monitoring
 * to ensure optimal VR performance across different engines.
 */
class PerformanceOptimizer {
public:
    // Performance configuration
    struct PerformanceConfig {
        bool enable_foveated_rendering = false;
        float resolution_scale = 0.9f;
        bool uncap_framerate = true;
        bool show_fps_overlay = true;
        bool show_stats_overlay = false;
        bool enable_vsync = false;
        bool enable_triple_buffering = false;
        UINT max_anisotropy = 16;
        bool enable_msaa = true;
        UINT msaa_samples = 4;
        bool enable_ssao = true;
        bool enable_bloom = true;
        bool enable_motion_blur = false;
        bool enable_depth_of_field = false;
        bool enable_shadows = true;
        UINT shadow_resolution = 2048;
        bool enable_ambient_occlusion = true;
        bool enable_screen_space_reflections = false;
        bool enable_tessellation = true;
        bool enable_tessellation_culling = true;
        UINT max_tessellation_factor = 16;
    };

    // Performance metrics
    struct PerformanceMetrics {
        float fps = 0.0f;
        float frame_time = 0.0f;
        float cpu_time = 0.0f;
        float gpu_time = 0.0f;
        float memory_usage = 0.0f;
        float vram_usage = 0.0f;
        UINT draw_calls = 0;
        UINT triangles = 0;
        UINT vertices = 0;
        UINT shader_switches = 0;
        UINT texture_switches = 0;
        UINT state_changes = 0;
        bool vsync_enabled = false;
        bool triple_buffering_enabled = false;
        UINT current_resolution_width = 0;
        UINT current_resolution_height = 0;
        float current_resolution_scale = 1.0f;
    };

    // Engine-specific optimization profiles
    struct EngineOptimizationProfile {
        std::string engine_name;
        std::string game_name;
        PerformanceConfig config;
        std::vector<std::string> optimization_hints;
        std::vector<std::string> known_issues;
        std::vector<std::string> recommended_settings;
    };

public:
    PerformanceOptimizer();
    ~PerformanceOptimizer();

    // Configuration management
    void set_performance_config(const PerformanceConfig& config);
    const PerformanceConfig& get_performance_config() const;
    
    // Engine-specific optimization
    void set_engine_profile(const std::string& engine_name, const std::string& game_name);
    const EngineOptimizationProfile* get_current_engine_profile() const;
    
    // Performance monitoring
    void begin_frame();
    void end_frame();
    void update_metrics(const PerformanceMetrics& metrics);
    const PerformanceMetrics& get_current_metrics() const;
    
    // Dynamic optimization
    void adjust_resolution_scale(float target_fps = 90.0f);
    void enable_foveated_rendering(bool enable);
    void adjust_shadow_quality(float performance_factor);
    void adjust_post_processing_quality(float performance_factor);
    
    // Performance analysis
    bool is_performance_acceptable() const;
    std::vector<std::string> get_performance_recommendations() const;
    std::vector<std::string> get_optimization_suggestions() const;

private:
    PerformanceConfig m_config;
    PerformanceMetrics m_current_metrics;
    PerformanceMetrics m_previous_metrics;
    std::string m_current_engine;
    std::string m_current_game;
    
    std::chrono::high_resolution_clock::time_point m_frame_start;
    std::chrono::high_resolution_clock::time_point m_frame_end;
    
    std::vector<EngineOptimizationProfile> m_engine_profiles;
    const EngineOptimizationProfile* m_current_profile;
    
    // Performance history for trend analysis
    std::vector<float> m_fps_history;
    std::vector<float> m_frame_time_history;
    static constexpr size_t HISTORY_SIZE = 60; // 1 second at 60 FPS
    
    // Internal optimization logic
    void initialize_engine_profiles();
    void analyze_performance_trends();
    float calculate_optimal_resolution_scale() const;
    void apply_engine_specific_optimizations();
};

/**
 * RE Engine specific performance optimizer
 * 
 * Provides optimizations specific to Capcom's RE Engine games.
 */
class REEnginePerformanceOptimizer : public PerformanceOptimizer {
public:
    REEnginePerformanceOptimizer();
    
    // RE Engine specific optimizations
    void optimize_horror_game_performance();
    void adjust_lighting_quality(float performance_factor);
    void optimize_shadow_rendering();
    void adjust_post_processing_for_vr();
    
private:
    void setup_re_engine_profile();
    void apply_horror_game_comfort_settings();
};

/**
 * REDengine 4 specific performance optimizer
 * 
 * Provides optimizations specific to CD Projekt Red's REDengine 4 games.
 */
class REDengine4PerformanceOptimizer : public PerformanceOptimizer {
public:
    REDengine4PerformanceOptimizer();
    
    // REDengine 4 specific optimizations
    void optimize_urban_environment_rendering();
    void adjust_ray_tracing_quality(float performance_factor);
    void optimize_volumetric_effects();
    void adjust_lod_distances();
    
private:
    void setup_redengine4_profile();
    void apply_cyberpunk_specific_optimizations();
};

/**
 * MT Framework specific performance optimizer
 * 
 * Provides optimizations specific to Capcom's MT Framework games.
 */
class MTFrameworkPerformanceOptimizer : public PerformanceOptimizer {
public:
    MTFrameworkPerformanceOptimizer();
    
    // MT Framework specific optimizations
    void optimize_combat_scene_rendering();
    void adjust_character_lod_quality();
    void optimize_weapon_effects();
    void adjust_environment_detail();
    
private:
    void setup_mt_framework_profile();
    void apply_action_game_optimizations();
};

/**
 * Global performance optimization manager
 * 
 * Provides centralized access to performance optimization features.
 */
class GlobalPerformanceManager {
public:
    static PerformanceOptimizer& get_optimizer();
    static REEnginePerformanceOptimizer& get_re_engine_optimizer();
    static REDengine4PerformanceOptimizer& get_redengine4_optimizer();
    static MTFrameworkPerformanceOptimizer& get_mt_framework_optimizer();
    
    // Global configuration
    static void configure_performance(const PerformanceConfig& config);
    static void set_global_optimization_level(const std::string& level);
    
    // Global cleanup
    static void cleanup_all();

private:
    static std::unique_ptr<PerformanceOptimizer> s_global_optimizer;
    static std::unique_ptr<REEnginePerformanceOptimizer> s_re_engine_optimizer;
    static std::unique_ptr<REDengine4PerformanceOptimizer> s_redengine4_optimizer;
    static std::unique_ptr<MTFrameworkPerformanceOptimizer> s_mt_framework_optimizer;
    static std::mutex s_init_mutex;
    
    static void initialize_if_needed();
};

} // namespace uevr_common
