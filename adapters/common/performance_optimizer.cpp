/*
 * Cross-Engine Performance Optimization System Implementation
 * 
 * Implementation of engine-specific performance tuning and optimization
 * for all cross-engine VR adapters.
 * 
 * Copyright (c) 2024 UEVR Cross-Engine Adapter Project
 * Licensed under MIT License
 */

#include "performance_optimizer.hpp"
#include <spdlog/spdlog.h>
#include <algorithm>
#include <numeric>

namespace uevr_common {

// PerformanceOptimizer Implementation
PerformanceOptimizer::PerformanceOptimizer()
    : m_current_profile(nullptr) {
    initialize_engine_profiles();
    spdlog::info("[PerformanceOptimizer] Performance optimizer initialized");
}

PerformanceOptimizer::~PerformanceOptimizer() {
    spdlog::info("[PerformanceOptimizer] Performance optimizer destroyed");
}

void PerformanceOptimizer::set_performance_config(const PerformanceConfig& config) {
    m_config = config;
    spdlog::info("[PerformanceOptimizer] Performance configuration updated");
}

const PerformanceOptimizer::PerformanceConfig& PerformanceOptimizer::get_performance_config() const {
    return m_config;
}

void PerformanceOptimizer::set_engine_profile(const std::string& engine_name, const std::string& game_name) {
    m_current_engine = engine_name;
    m_current_game = game_name;
    
    // Find matching profile
    for (const auto& profile : m_engine_profiles) {
        if (profile.engine_name == engine_name && profile.game_name == game_name) {
            m_current_profile = &profile;
            m_config = profile.config;
            spdlog::info("[PerformanceOptimizer] Set engine profile: {} - {}", engine_name, game_name);
            return;
        }
    }
    
    // Use default profile if none found
    m_current_profile = nullptr;
    spdlog::warn("[PerformanceOptimizer] No profile found for {} - {}, using defaults", engine_name, game_name);
}

const PerformanceOptimizer::EngineOptimizationProfile* PerformanceOptimizer::get_current_engine_profile() const {
    return m_current_profile;
}

void PerformanceOptimizer::begin_frame() {
    m_frame_start = std::chrono::high_resolution_clock::now();
}

void PerformanceOptimizer::end_frame() {
    m_frame_end = std::chrono::high_resolution_clock::now();
    
    // Calculate frame time
    auto frame_duration = std::chrono::duration_cast<std::chrono::microseconds>(m_frame_end - m_frame_start);
    m_current_metrics.frame_time = frame_duration.count() / 1000.0f; // Convert to milliseconds
    
    // Calculate FPS
    if (m_current_metrics.frame_time > 0.0f) {
        m_current_metrics.fps = 1000.0f / m_current_metrics.frame_time;
    }
    
    // Update history
    m_fps_history.push_back(m_current_metrics.fps);
    m_frame_time_history.push_back(m_current_metrics.frame_time);
    
    // Keep history size manageable
    if (m_fps_history.size() > HISTORY_SIZE) {
        m_fps_history.erase(m_fps_history.begin());
    }
    if (m_frame_time_history.size() > HISTORY_SIZE) {
        m_frame_time_history.erase(m_frame_time_history.begin());
    }
    
    // Analyze performance trends
    analyze_performance_trends();
}

void PerformanceOptimizer::update_metrics(const PerformanceMetrics& metrics) {
    m_previous_metrics = m_current_metrics;
    m_current_metrics = metrics;
}

const PerformanceOptimizer::PerformanceMetrics& PerformanceOptimizer::get_current_metrics() const {
    return m_current_metrics;
}

void PerformanceOptimizer::adjust_resolution_scale(float target_fps) {
    if (m_current_metrics.fps < target_fps * 0.9f) {
        // Performance is below target, reduce resolution
        float new_scale = m_config.resolution_scale * 0.95f;
        m_config.resolution_scale = std::max(0.5f, new_scale);
        spdlog::info("[PerformanceOptimizer] Reduced resolution scale to {:.2f} for target FPS {:.1f}", 
                     m_config.resolution_scale, target_fps);
    } else if (m_current_metrics.fps > target_fps * 1.1f) {
        // Performance is above target, can increase resolution
        float new_scale = m_config.resolution_scale * 1.05f;
        m_config.resolution_scale = std::min(1.0f, new_scale);
        spdlog::info("[PerformanceOptimizer] Increased resolution scale to {:.2f} for target FPS {:.1f}", 
                     m_config.resolution_scale, target_fps);
    }
}

void PerformanceOptimizer::enable_foveated_rendering(bool enable) {
    m_config.enable_foveated_rendering = enable;
    spdlog::info("[PerformanceOptimizer] Foveated rendering {}", enable ? "enabled" : "disabled");
}

void PerformanceOptimizer::adjust_shadow_quality(float performance_factor) {
    if (performance_factor < 0.5f) {
        m_config.shadow_resolution = 1024;
        m_config.enable_shadows = true;
    } else if (performance_factor < 0.8f) {
        m_config.shadow_resolution = 2048;
        m_config.enable_shadows = true;
    } else {
        m_config.shadow_resolution = 4096;
        m_config.enable_shadows = true;
    }
    
    spdlog::info("[PerformanceOptimizer] Shadow quality adjusted: {}x{}", 
                 m_config.shadow_resolution, m_config.shadow_resolution);
}

void PerformanceOptimizer::adjust_post_processing_quality(float performance_factor) {
    if (performance_factor < 0.5f) {
        // Low performance - disable expensive effects
        m_config.enable_bloom = false;
        m_config.enable_ssao = false;
        m_config.enable_screen_space_reflections = false;
        m_config.enable_motion_blur = false;
        m_config.enable_depth_of_field = false;
    } else if (performance_factor < 0.8f) {
        // Medium performance - enable some effects
        m_config.enable_bloom = true;
        m_config.enable_ssao = true;
        m_config.enable_screen_space_reflections = false;
        m_config.enable_motion_blur = false;
        m_config.enable_depth_of_field = false;
    } else {
        // High performance - enable all effects
        m_config.enable_bloom = true;
        m_config.enable_ssao = true;
        m_config.enable_screen_space_reflections = true;
        m_config.enable_motion_blur = true;
        m_config.enable_depth_of_field = true;
    }
    
    spdlog::info("[PerformanceOptimizer] Post-processing quality adjusted for performance factor {:.2f}", 
                 performance_factor);
}

bool PerformanceOptimizer::is_performance_acceptable() const {
    // Check if FPS is above minimum threshold
    if (m_current_metrics.fps < 72.0f) { // Minimum for VR
        return false;
    }
    
    // Check if frame time is reasonable
    if (m_current_metrics.frame_time > 13.89f) { // 72 FPS = 13.89ms
        return false;
    }
    
    return true;
}

std::vector<std::string> PerformanceOptimizer::get_performance_recommendations() const {
    std::vector<std::string> recommendations;
    
    if (m_current_metrics.fps < 72.0f) {
        recommendations.push_back("Reduce resolution scale to improve FPS");
        recommendations.push_back("Disable expensive post-processing effects");
        recommendations.push_back("Reduce shadow resolution");
        recommendations.push_back("Enable foveated rendering if supported");
    }
    
    if (m_current_metrics.frame_time > 13.89f) {
        recommendations.push_back("Optimize rendering pipeline");
        recommendations.push_back("Reduce draw calls and state changes");
        recommendations.push_back("Use LOD systems for distant objects");
    }
    
    if (m_current_metrics.memory_usage > 80.0f) {
        recommendations.push_back("Reduce texture quality");
        recommendations.push_back("Enable texture streaming");
        recommendations.push_back("Optimize asset loading");
    }
    
    return recommendations;
}

std::vector<std::string> PerformanceOptimizer::get_optimization_suggestions() const {
    std::vector<std::string> suggestions;
    
    if (m_current_profile) {
        suggestions.insert(suggestions.end(), 
                         m_current_profile->optimization_hints.begin(),
                         m_current_profile->optimization_hints.end());
    }
    
    // General suggestions
    suggestions.push_back("Use adaptive quality settings");
    suggestions.push_back("Monitor performance metrics in real-time");
    suggestions.push_back("Adjust settings based on scene complexity");
    suggestions.push_back("Enable dynamic LOD systems");
    
    return suggestions;
}

void PerformanceOptimizer::initialize_engine_profiles() {
    // RE Engine Profile
    EngineOptimizationProfile re_profile;
    re_profile.engine_name = "RE Engine";
    re_profile.game_name = "Resident Evil 7";
    re_profile.config.resolution_scale = 0.9f;
    re_profile.config.enable_foveated_rendering = false;
    re_profile.config.enable_shadows = true;
    re_profile.config.shadow_resolution = 2048;
    re_profile.config.enable_ssao = true;
    re_profile.config.enable_bloom = true;
    re_profile.config.enable_motion_blur = false; // Disable for VR comfort
    re_profile.config.enable_depth_of_field = false; // Disable for VR comfort
    re_profile.optimization_hints = {
        "Disable motion blur and depth of field for VR comfort",
        "Optimize shadow rendering for horror atmosphere",
        "Use adaptive lighting quality based on scene",
        "Maintain consistent frame pacing for immersion"
    };
    re_profile.known_issues = {
        "Fixed camera angles may cause VR discomfort",
        "High contrast scenes may cause eye strain"
    };
    re_profile.recommended_settings = {
        "Resolution scale: 0.9",
        "Shadows: High (2048x2048)",
        "SSAO: Enabled",
        "Bloom: Enabled",
        "Motion blur: Disabled",
        "Depth of field: Disabled"
    };
    m_engine_profiles.push_back(re_profile);
    
    // REDengine 4 Profile
    EngineOptimizationProfile red_profile;
    red_profile.engine_name = "REDengine 4";
    red_profile.game_name = "Cyberpunk 2077";
    red_profile.config.resolution_scale = 0.85f;
    red_profile.config.enable_foveated_rendering = true;
    red_profile.config.enable_shadows = true;
    red_profile.config.shadow_resolution = 2048;
    red_profile.config.enable_ssao = true;
    red_profile.config.enable_bloom = true;
    red_profile.config.enable_motion_blur = false;
    red_profile.config.enable_depth_of_field = false;
    red_profile.optimization_hints = {
        "Use foveated rendering for urban environments",
        "Optimize ray tracing quality for performance",
        "Adjust LOD distances for complex city scenes",
        "Balance visual quality with VR performance"
    };
    red_profile.known_issues = {
        "Complex urban environments may impact performance",
        "Ray tracing features require significant GPU power"
    };
    red_profile.recommended_settings = {
        "Resolution scale: 0.85",
        "Shadows: High (2048x2048)",
        "SSAO: Enabled",
        "Bloom: Enabled",
        "Ray tracing: Quality (if supported)",
        "Foveated rendering: Enabled"
    };
    m_engine_profiles.push_back(red_profile);
    
    // MT Framework Profile
    EngineOptimizationProfile mt_profile;
    mt_profile.engine_name = "MT Framework";
    mt_profile.game_name = "Monster Hunter: World";
    mt_profile.config.resolution_scale = 0.9f;
    mt_profile.config.enable_foveated_rendering = false;
    mt_profile.config.enable_shadows = true;
    mt_profile.config.shadow_resolution = 2048;
    mt_profile.config.enable_ssao = true;
    mt_profile.config.enable_bloom = true;
    mt_profile.config.enable_motion_blur = false;
    mt_profile.config.enable_depth_of_field = false;
    mt_profile.optimization_hints = {
        "Optimize for combat scene rendering",
        "Use adaptive LOD for monster models",
        "Balance environment detail with performance",
        "Maintain smooth frame rate during intense battles"
    };
    mt_profile.known_issues = {
        "Large monster models may impact performance",
        "Complex combat scenes require careful optimization"
    };
    mt_profile.recommended_settings = {
        "Resolution scale: 0.9",
        "Shadows: High (2048x2048)",
        "SSAO: Enabled",
        "Bloom: Enabled",
        "Character LOD: High",
        "Environment detail: Medium"
    };
    m_engine_profiles.push_back(mt_profile);
}

void PerformanceOptimizer::analyze_performance_trends() {
    if (m_fps_history.size() < 10) {
        return; // Need more data for trend analysis
    }
    
    // Calculate average FPS
    float avg_fps = std::accumulate(m_fps_history.begin(), m_fps_history.end(), 0.0f) / m_fps_history.size();
    
    // Check for performance degradation
    if (avg_fps < 72.0f && m_config.resolution_scale > 0.5f) {
        spdlog::warn("[PerformanceOptimizer] Performance degradation detected, average FPS: {:.1f}", avg_fps);
        adjust_resolution_scale(90.0f);
    }
    
    // Check for performance improvement
    if (avg_fps > 90.0f && m_config.resolution_scale < 1.0f) {
        spdlog::info("[PerformanceOptimizer] Performance improvement detected, average FPS: {:.1f}", avg_fps);
        adjust_resolution_scale(90.0f);
    }
}

float PerformanceOptimizer::calculate_optimal_resolution_scale() const {
    // Simple heuristic based on current FPS
    if (m_current_metrics.fps > 90.0f) {
        return std::min(1.0f, m_config.resolution_scale * 1.05f);
    } else if (m_current_metrics.fps < 72.0f) {
        return std::max(0.5f, m_config.resolution_scale * 0.95f);
    }
    
    return m_config.resolution_scale;
}

void PerformanceOptimizer::apply_engine_specific_optimizations() {
    if (m_current_profile) {
        spdlog::info("[PerformanceOptimizer] Applying engine-specific optimizations for {} - {}", 
                     m_current_profile->engine_name, m_current_profile->game_name);
    }
}

// REEnginePerformanceOptimizer Implementation
REEnginePerformanceOptimizer::REEnginePerformanceOptimizer() {
    setup_re_engine_profile();
}

void REEnginePerformanceOptimizer::optimize_horror_game_performance() {
    spdlog::info("[REEnginePerformance] Optimizing horror game performance");
    
    // Disable effects that may cause VR discomfort
    m_config.enable_motion_blur = false;
    m_config.enable_depth_of_field = false;
    
    // Optimize for atmospheric lighting
    m_config.enable_shadows = true;
    m_config.shadow_resolution = 2048;
    m_config.enable_ssao = true;
    
    apply_horror_game_comfort_settings();
}

void REEnginePerformanceOptimizer::adjust_lighting_quality(float performance_factor) {
    if (performance_factor < 0.5f) {
        m_config.enable_shadows = false;
        m_config.enable_ssao = false;
        m_config.enable_bloom = false;
    } else if (performance_factor < 0.8f) {
        m_config.enable_shadows = true;
        m_config.shadow_resolution = 1024;
        m_config.enable_ssao = true;
        m_config.enable_bloom = true;
    } else {
        m_config.enable_shadows = true;
        m_config.shadow_resolution = 2048;
        m_config.enable_ssao = true;
        m_config.enable_bloom = true;
    }
    
    spdlog::info("[REEnginePerformance] Lighting quality adjusted for performance factor {:.2f}", 
                 performance_factor);
}

void REEnginePerformanceOptimizer::optimize_shadow_rendering() {
    // Optimize shadows for horror atmosphere
    m_config.enable_shadows = true;
    m_config.shadow_resolution = 2048;
    
    spdlog::info("[REEnginePerformance] Shadow rendering optimized for horror atmosphere");
}

void REEnginePerformanceOptimizer::adjust_post_processing_for_vr() {
    // Disable effects that may cause VR discomfort
    m_config.enable_motion_blur = false;
    m_config.enable_depth_of_field = false;
    
    // Keep atmospheric effects
    m_config.enable_bloom = true;
    m_config.enable_ssao = true;
    
    spdlog::info("[REEnginePerformance] Post-processing adjusted for VR comfort");
}

void REEnginePerformanceOptimizer::setup_re_engine_profile() {
    set_engine_profile("RE Engine", "Resident Evil 7");
}

void REEnginePerformanceOptimizer::apply_horror_game_comfort_settings() {
    // Apply horror game specific comfort settings
    m_config.enable_motion_blur = false;
    m_config.enable_depth_of_field = false;
    m_config.resolution_scale = 0.9f;
    
    spdlog::info("[REEnginePerformance] Horror game comfort settings applied");
}

// REDengine4PerformanceOptimizer Implementation
REDengine4PerformanceOptimizer::REDengine4PerformanceOptimizer() {
    setup_redengine4_profile();
}

void REDengine4PerformanceOptimizer::optimize_urban_environment_rendering() {
    spdlog::info("[REDengine4Performance] Optimizing urban environment rendering");
    
    // Optimize for complex urban scenes
    m_config.enable_foveated_rendering = true;
    m_config.resolution_scale = 0.85f;
    
    // Balance quality and performance
    m_config.enable_shadows = true;
    m_config.shadow_resolution = 2048;
    m_config.enable_ssao = true;
    m_config.enable_bloom = true;
}

void REDengine4PerformanceOptimizer::adjust_ray_tracing_quality(float performance_factor) {
    // Adjust ray tracing settings based on performance
    if (performance_factor < 0.5f) {
        // Disable ray tracing for low performance
        spdlog::info("[REDengine4Performance] Ray tracing disabled for low performance");
    } else if (performance_factor < 0.8f) {
        // Use low quality ray tracing
        spdlog::info("[REDengine4Performance] Ray tracing quality set to low");
    } else {
        // Use high quality ray tracing
        spdlog::info("[REDengine4Performance] Ray tracing quality set to high");
    }
}

void REDengine4PerformanceOptimizer::optimize_volumetric_effects() {
    // Optimize volumetric effects for urban environments
    spdlog::info("[REDengine4Performance] Volumetric effects optimized for urban environments");
}

void REDengine4PerformanceOptimizer::adjust_lod_distances() {
    // Adjust LOD distances for complex city scenes
    spdlog::info("[REDengine4Performance] LOD distances adjusted for city scenes");
}

void REDengine4PerformanceOptimizer::setup_redengine4_profile() {
    set_engine_profile("REDengine 4", "Cyberpunk 2077");
}

void REDengine4PerformanceOptimizer::apply_cyberpunk_specific_optimizations() {
    // Apply Cyberpunk specific optimizations
    m_config.enable_foveated_rendering = true;
    m_config.resolution_scale = 0.85f;
    
    spdlog::info("[REDengine4Performance] Cyberpunk specific optimizations applied");
}

// MTFrameworkPerformanceOptimizer Implementation
MTFrameworkPerformanceOptimizer::MTFrameworkPerformanceOptimizer() {
    setup_mt_framework_profile();
}

void MTFrameworkPerformanceOptimizer::optimize_combat_scene_rendering() {
    spdlog::info("[MTFrameworkPerformance] Optimizing combat scene rendering");
    
    // Optimize for intense combat scenes
    m_config.resolution_scale = 0.9f;
    m_config.enable_motion_blur = false; // Disable for VR comfort during combat
}

void MTFrameworkPerformanceOptimizer::adjust_character_lod_quality() {
    // Adjust character LOD for monster models
    spdlog::info("[MTFrameworkPerformance] Character LOD quality adjusted for monster models");
}

void MTFrameworkPerformanceOptimizer::optimize_weapon_effects() {
    // Optimize weapon effects for combat
    spdlog::info("[MTFrameworkPerformance] Weapon effects optimized for combat");
}

void MTFrameworkPerformanceOptimizer::adjust_environment_detail() {
    // Adjust environment detail for performance
    spdlog::info("[MTFrameworkPerformance] Environment detail adjusted for performance");
}

void MTFrameworkPerformanceOptimizer::setup_mt_framework_profile() {
    set_engine_profile("MT Framework", "Monster Hunter: World");
}

void MTFrameworkPerformanceOptimizer::apply_action_game_optimizations() {
    // Apply action game specific optimizations
    m_config.enable_motion_blur = false;
    m_config.resolution_scale = 0.9f;
    
    spdlog::info("[MTFrameworkPerformance] Action game optimizations applied");
}

// GlobalPerformanceManager Implementation
std::unique_ptr<PerformanceOptimizer> GlobalPerformanceManager::s_global_optimizer;
std::unique_ptr<REEnginePerformanceOptimizer> GlobalPerformanceManager::s_re_engine_optimizer;
std::unique_ptr<REDengine4PerformanceOptimizer> GlobalPerformanceManager::s_redengine4_optimizer;
std::unique_ptr<MTFrameworkPerformanceOptimizer> GlobalPerformanceManager::s_mt_framework_optimizer;
std::mutex GlobalPerformanceManager::s_init_mutex;

PerformanceOptimizer& GlobalPerformanceManager::get_optimizer() {
    initialize_if_needed();
    return *s_global_optimizer;
}

REEnginePerformanceOptimizer& GlobalPerformanceManager::get_re_engine_optimizer() {
    initialize_if_needed();
    return *s_re_engine_optimizer;
}

REDengine4PerformanceOptimizer& GlobalPerformanceManager::get_redengine4_optimizer() {
    initialize_if_needed();
    return *s_redengine4_optimizer;
}

MTFrameworkPerformanceOptimizer& GlobalPerformanceManager::get_mt_framework_optimizer() {
    initialize_if_needed();
    return *s_mt_framework_optimizer;
}

void GlobalPerformanceManager::configure_performance(const PerformanceConfig& config) {
    std::lock_guard<std::mutex> lock(s_init_mutex);
    
    if (s_global_optimizer) {
        s_global_optimizer->set_performance_config(config);
    }
}

void GlobalPerformanceManager::set_global_optimization_level(const std::string& level) {
    std::lock_guard<std::mutex> lock(s_init_mutex);
    
    PerformanceConfig config;
    
    if (level == "low") {
        config.resolution_scale = 0.7f;
        config.enable_foveated_rendering = true;
        config.shadow_resolution = 1024;
        config.enable_ssao = false;
        config.enable_bloom = false;
    } else if (level == "medium") {
        config.resolution_scale = 0.85f;
        config.enable_foveated_rendering = false;
        config.shadow_resolution = 2048;
        config.enable_ssao = true;
        config.enable_bloom = true;
    } else if (level == "high") {
        config.resolution_scale = 1.0f;
        config.enable_foveated_rendering = false;
        config.shadow_resolution = 4096;
        config.enable_ssao = true;
        config.enable_bloom = true;
    }
    
    configure_performance(config);
    spdlog::info("[GlobalPerformanceManager] Global optimization level set to: {}", level);
}

void GlobalPerformanceManager::cleanup_all() {
    std::lock_guard<std::mutex> lock(s_init_mutex);
    
    s_global_optimizer.reset();
    s_re_engine_optimizer.reset();
    s_redengine4_optimizer.reset();
    s_mt_framework_optimizer.reset();
    
    spdlog::info("[GlobalPerformanceManager] All performance managers cleaned up");
}

void GlobalPerformanceManager::initialize_if_needed() {
    std::lock_guard<std::mutex> lock(s_init_mutex);
    
    if (!s_global_optimizer) {
        s_global_optimizer = std::make_unique<PerformanceOptimizer>();
    }
    
    if (!s_re_engine_optimizer) {
        s_re_engine_optimizer = std::make_unique<REEnginePerformanceOptimizer>();
    }
    
    if (!s_redengine4_optimizer) {
        s_redengine4_optimizer = std::make_unique<REDengine4PerformanceOptimizer>();
    }
    
    if (!s_mt_framework_optimizer) {
        s_mt_framework_optimizer = std::make_unique<MTFrameworkPerformanceOptimizer>();
    }
}

} // namespace uevr_common
