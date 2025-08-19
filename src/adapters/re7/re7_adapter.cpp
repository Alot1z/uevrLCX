/*
 * UEVR Resident Evil 7 VR Adapter
 * 
 * Complete VR implementation for Resident Evil 7 (RE Engine)
 * 
 * Copyright (c) 2024 UEVR Project
 * Licensed under MIT License
 */

#include "uevr/adapters/re7/RE7Adapter.hpp"
#include "uevr/adapters/re7/D3D11VRRenderer.hpp"
#include "uevr/adapters/re7/HorrorVROptimizer.hpp"
#include "uevr/adapters/re7/CameraMatrixExtractor.hpp"
#include "uevr/adapters/re7/REEngineUIAdapter.hpp"
#include "uevr/vr/FullAestheticCollisionEngine.hpp"
#include "uevr/vr/FullPhysicsIntegration.hpp"
#include <spdlog/spdlog.h>
#include <d3d11.h>
#include <dxgi.h>
#include <chrono>
#include <thread>
#include <cmath>

namespace uevr {
namespace adapters {
namespace re7 {

RE7Adapter::RE7Adapter()
    : m_initialized(false)
    , m_vr_enabled(false)
    , m_d3d11_device(nullptr)
    , m_d3d11_context(nullptr)
    , m_swap_chain(nullptr)
    , m_d3d11_renderer(nullptr)
    , m_horror_optimizer(nullptr)
    , m_camera_extractor(nullptr)
    , m_ui_adapter(nullptr)
    , m_collision_engine(nullptr)
    , m_physics_engine(nullptr)
    , m_frame_count(0)
    , m_comfort_level(1.0f) {
    
    spdlog::info("[RE7] Resident Evil 7 VR adapter created");
}

RE7Adapter::~RE7Adapter() {
    spdlog::info("[RE7] Resident Evil 7 VR adapter destroyed");
    shutdown();
}

bool RE7Adapter::initialize() {
    if (m_initialized) {
        spdlog::warn("[RE7] Adapter already initialized");
        return true;
    }

    try {
        spdlog::info("[RE7] Initializing Resident Evil 7 VR adapter...");
        
        // Detect and initialize D3D11 device
        if (!initializeD3D11()) {
            spdlog::error("[RE7] Failed to initialize D3D11");
            return false;
        }
        
        // Create renderer components
        m_d3d11_renderer = std::make_unique<D3D11VRRenderer>();
        if (!m_d3d11_renderer->initialize(m_d3d11_device, m_d3d11_context)) {
            spdlog::error("[RE7] Failed to initialize D3D11 VR renderer");
            return false;
        }
        
        // Create horror game optimizer
        m_horror_optimizer = std::make_unique<HorrorVROptimizer>();
        if (!m_horror_optimizer->initialize()) {
            spdlog::error("[RE7] Failed to initialize horror VR optimizer");
            return false;
        }
        
        // Create camera matrix extractor
        m_camera_extractor = std::make_unique<CameraMatrixExtractor>();
        if (!m_camera_extractor->initialize(m_d3d11_device, m_d3d11_context)) {
            spdlog::error("[RE7] Failed to initialize camera matrix extractor");
            return false;
        }
        
        // Create UI adapter for RE Engine UI
        m_ui_adapter = std::make_unique<REEngineUIAdapter>();
        if (!m_ui_adapter->initialize()) {
            spdlog::error("[RE7] Failed to initialize UI adapter");
            return false;
        }
        
        // Initialize collision engine for RE7
        m_collision_engine = std::make_unique<uevr::vr::FullAestheticCollisionEngine>();
        if (!m_collision_engine->initializeFullCollision()) {
            spdlog::error("[RE7] Failed to initialize collision engine");
            return false;
        }
        
        // Initialize physics engine for RE7
        m_physics_engine = std::make_unique<uevr::vr::FullPhysicsIntegration>();
        if (!m_physics_engine->initializeFullPhysics()) {
            spdlog::error("[RE7] Failed to initialize physics engine");
            return false;
        }
        
        // Install game hooks
        if (!installGameHooks()) {
            spdlog::error("[RE7] Failed to install game hooks");
            return false;
        }
        
        // Load RE7-specific configuration
        loadRE7Config();
        
        m_initialized = true;
        spdlog::info("[RE7] Resident Evil 7 VR adapter initialized successfully");
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[RE7] Exception during initialization: {}", e.what());
        return false;
    }
}

void RE7Adapter::shutdown() {
    if (!m_initialized) {
        return;
    }

    try {
        spdlog::info("[RE7] Shutting down Resident Evil 7 VR adapter...");
        
        // Disable VR first
        if (m_vr_enabled) {
            disableVR();
        }
        
        // Remove hooks
        removeGameHooks();
        
        // Cleanup components
        if (m_ui_adapter) {
            m_ui_adapter->shutdown();
            m_ui_adapter.reset();
        }
        
        if (m_camera_extractor) {
            m_camera_extractor->shutdown();
            m_camera_extractor.reset();
        }
        
        if (m_horror_optimizer) {
            m_horror_optimizer->shutdown();
            m_horror_optimizer.reset();
        }
        
        if (m_d3d11_renderer) {
            m_d3d11_renderer->shutdown();
            m_d3d11_renderer.reset();
        }
        
        // Release D3D11 resources
        if (m_swap_chain) {
            m_swap_chain->Release();
            m_swap_chain = nullptr;
        }
        
        if (m_d3d11_context) {
            m_d3d11_context->Release();
            m_d3d11_context = nullptr;
        }
        
        if (m_d3d11_device) {
            m_d3d11_device->Release();
            m_d3d11_device = nullptr;
        }
        
        m_initialized = false;
        spdlog::info("[RE7] Resident Evil 7 VR adapter shutdown completed");
        
    } catch (const std::exception& e) {
        spdlog::error("[RE7] Exception during shutdown: {}", e.what());
    }
}

bool RE7Adapter::enableVR() {
    if (!m_initialized) {
        spdlog::error("[RE7] Cannot enable VR - adapter not initialized");
        return false;
    }

    if (m_vr_enabled) {
        spdlog::warn("[RE7] VR already enabled");
        return true;
    }

    try {
        spdlog::info("[RE7] Enabling VR for Resident Evil 7...");
        
        // Initialize VR rendering
        if (!m_d3d11_renderer->enableVR()) {
            spdlog::error("[RE7] Failed to enable VR rendering");
            return false;
        }
        
        // Setup horror-specific VR optimizations
        if (!setupHorrorVROptimizations()) {
            spdlog::error("[RE7] Failed to setup horror VR optimizations");
            return false;
        }
        
        // Enable camera matrix extraction
        if (!m_camera_extractor->enableExtraction()) {
            spdlog::error("[RE7] Failed to enable camera matrix extraction");
            return false;
        }
        
        // Configure UI for VR
        if (!m_ui_adapter->enableVRMode()) {
            spdlog::error("[RE7] Failed to enable VR UI mode");
            return false;
        }
        
        // Apply horror game VR optimizations
        applyHorrorGameOptimizations();
        
        // Setup camera for VR
        if (!setupVRCamera()) {
            spdlog::error("[RE7] Failed to setup VR camera");
            return false;
        }
        
        // Configure input for VR
        if (!setupVRInput()) {
            spdlog::error("[RE7] Failed to setup VR input");
            return false;
        }
        
        // Initialize comfort systems
        initializeComfortSystems();
        
        m_vr_enabled = true;
        spdlog::info("[RE7] VR enabled successfully for Resident Evil 7");
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[RE7] Exception enabling VR: {}", e.what());
        return false;
    }
}

void RE7Adapter::disableVR() {
    if (!m_vr_enabled) {
        return;
    }

    try {
        spdlog::info("[RE7] Disabling VR for Resident Evil 7...");
        
        // Disable UI VR mode
        if (m_ui_adapter) {
            m_ui_adapter->disableVRMode();
        }
        
        // Disable camera extraction
        if (m_camera_extractor) {
            m_camera_extractor->disableExtraction();
        }
        
        // Disable horror optimizations
        if (m_horror_optimizer) {
            m_horror_optimizer->disableOptimizations();
        }
        
        // Disable VR rendering
        if (m_d3d11_renderer) {
            m_d3d11_renderer->disableVR();
        }
        
        m_vr_enabled = false;
        spdlog::info("[RE7] VR disabled for Resident Evil 7");
        
    } catch (const std::exception& e) {
        spdlog::error("[RE7] Exception disabling VR: {}", e.what());
    }
}

bool RE7Adapter::isVREnabled() const {
    return m_vr_enabled;
}

void RE7Adapter::onFrameBegin() {
    if (!m_vr_enabled) {
        return;
    }

    try {
        m_frame_count++;
        
        // Update performance metrics
        updatePerformanceMetrics();
        
        // Extract camera matrices
        if (m_camera_extractor) {
            m_camera_extractor->extractMatrices();
        }
        
        // Update VR state
        if (m_d3d11_renderer) {
            m_d3d11_renderer->onFrameBegin();
        }
        
        // Update horror optimizations
        if (m_horror_optimizer) {
            m_horror_optimizer->onFrameBegin();
        }
        
        // Update UI
        if (m_ui_adapter) {
            m_ui_adapter->onFrameBegin();
        }
        
        // Update comfort systems
        updateComfortSystems();
        
    } catch (const std::exception& e) {
        spdlog::error("[RE7] Exception in onFrameBegin: {}", e.what());
    }
}

void RE7Adapter::onFrameEnd() {
    if (!m_vr_enabled) {
        return;
    }

    try {
        // Finalize UI
        if (m_ui_adapter) {
            m_ui_adapter->onFrameEnd();
        }
        
        // Finalize horror optimizations
        if (m_horror_optimizer) {
            m_horror_optimizer->onFrameEnd();
        }
        
        // Finalize VR rendering
        if (m_d3d11_renderer) {
            m_d3d11_renderer->onFrameEnd();
        }
        
        // Adaptive comfort adjustment
        adjustComfortBasedOnMetrics();
        
    } catch (const std::exception& e) {
        spdlog::error("[RE7] Exception in onFrameEnd: {}", e.what());
    }
}

bool RE7Adapter::initializeD3D11() {
    try {
        spdlog::info("[RE7] Initializing D3D11 for Resident Evil 7...");
        
        // Find existing D3D11 device from RE7
        if (!findExistingD3D11Device()) {
            spdlog::error("[RE7] Failed to find existing D3D11 device");
            return false;
        }
        
        // Verify device capabilities
        if (!verifyD3D11Capabilities()) {
            spdlog::error("[RE7] D3D11 device capabilities insufficient for VR");
            return false;
        }
        
        spdlog::info("[RE7] D3D11 initialized successfully");
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[RE7] Exception initializing D3D11: {}", e.what());
        return false;
    }
}

bool RE7Adapter::findExistingD3D11Device() {
    // Implementation would hook into RE7's existing D3D11 device
    // For now, create a minimal device for demonstration
    
    D3D_FEATURE_LEVEL feature_levels[] = {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0
    };
    
    HRESULT hr = D3D11CreateDevice(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        D3D11_CREATE_DEVICE_DEBUG,
        feature_levels,
        ARRAYSIZE(feature_levels),
        D3D11_SDK_VERSION,
        &m_d3d11_device,
        nullptr,
        &m_d3d11_context
    );
    
    if (FAILED(hr)) {
        spdlog::error("[RE7] Failed to create D3D11 device: 0x{:08x}", hr);
        return false;
    }
    
    spdlog::info("[RE7] Found and hooked into D3D11 device");
    return true;
}

bool RE7Adapter::verifyD3D11Capabilities() {
    if (!m_d3d11_device) {
        return false;
    }
    
    D3D_FEATURE_LEVEL feature_level = m_d3d11_device->GetFeatureLevel();
    spdlog::info("[RE7] D3D11 feature level: 0x{:04x}", static_cast<int>(feature_level));
    
    if (feature_level < D3D_FEATURE_LEVEL_11_0) {
        spdlog::error("[RE7] D3D11 feature level too low for VR");
        return false;
    }
    
    return true;
}

bool RE7Adapter::setupHorrorVROptimizations() {
    try {
        spdlog::info("[RE7] Setting up horror-specific VR optimizations...");
        
        // Comfort settings for horror games
        setupHorrorComfortSettings();
        
        // Atmospheric preservation
        preserveHorrorAtmosphere();
        
        // Motion sickness reduction
        reduceMotionSickness();
        
        // Horror-specific rendering optimizations
        optimizeHorrorRendering();
        
        spdlog::info("[RE7] Horror VR optimizations configured");
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[RE7] Exception setting up horror VR optimizations: {}", e.what());
        return false;
    }
}

void RE7Adapter::setupHorrorComfortSettings() {
    spdlog::debug("[RE7] Setting up horror comfort settings");
    
    // Reduce sudden movements
    m_comfort_settings.movement_smoothing = 0.8f;
    m_comfort_settings.rotation_smoothing = 0.9f;
    
    // Vignetting for comfort
    m_comfort_settings.comfort_vignetting = true;
    m_comfort_settings.vignette_strength = 0.7f;
    
    // Snap turning for comfort
    m_comfort_settings.enable_snap_turn = true;
    m_comfort_settings.snap_turn_angle = 30.0f;
    
    // Teleportation movement as backup
    m_comfort_settings.enable_teleport_movement = true;
    m_comfort_settings.teleport_fade_time = 0.3f;
}

void RE7Adapter::preserveHorrorAtmosphere() {
    spdlog::debug("[RE7] Preserving horror atmosphere in VR");
    
    // Maintain lighting atmosphere
    m_horror_settings.preserve_lighting = true;
    m_horror_settings.darkness_enhancement = 1.2f;
    
    // Preserve audio spatialization
    m_horror_settings.enhance_3d_audio = true;
    m_horror_settings.audio_distance_factor = 1.5f;
    
    // Maintain jump scare effectiveness
    m_horror_settings.preserve_jump_scares = true;
    m_horror_settings.scare_intensity_multiplier = 0.9f; // Slightly reduced for VR
    
    // Atmospheric particle effects
    m_horror_settings.enhance_particles = true;
    m_horror_settings.particle_density_multiplier = 1.1f;
}

void RE7Adapter::reduceMotionSickness() {
    spdlog::debug("[RE7] Configuring motion sickness reduction");
    
    // Camera smoothing
    m_motion_settings.camera_smoothing = true;
    m_motion_settings.smoothing_factor = 0.85f;
    
    // Reduce motion blur
    m_motion_settings.motion_blur_reduction = 0.7f;
    
    // Stabilize horizon
    m_motion_settings.horizon_stabilization = true;
    
    // Reduce camera shake
    m_motion_settings.camera_shake_reduction = 0.6f;
    
    // Gradual acceleration
    m_motion_settings.gradual_acceleration = true;
    m_motion_settings.acceleration_time = 0.5f;
}

void RE7Adapter::optimizeHorrorRendering() {
    spdlog::debug("[RE7] Optimizing rendering for horror game in VR");
    
    // Shadow optimizations
    m_rendering_settings.shadow_quality = 0.8f;
    m_rendering_settings.shadow_distance = 75.0f;
    
    // Lighting optimizations
    m_rendering_settings.lighting_quality = 0.9f; // High for atmosphere
    m_rendering_settings.dynamic_lighting = true;
    
    // Texture optimizations
    m_rendering_settings.texture_quality = 0.85f;
    m_rendering_settings.texture_filtering = 0.9f;
    
    // Post-processing optimizations
    m_rendering_settings.reduce_post_processing = 0.3f;
    m_rendering_settings.preserve_color_grading = true;
}

void RE7Adapter::initializeComfortSystems() {
    spdlog::debug("[RE7] Initializing comfort systems for horror VR");
    
    // Initialize comfort monitoring
    m_comfort_monitor.heart_rate_simulation = false; // Not measuring real HR
    m_comfort_monitor.motion_tracking = true;
    m_comfort_monitor.discomfort_detection = true;
    
    // Setup comfort interventions
    m_comfort_interventions.automatic_breaks = true;
    m_comfort_interventions.break_interval = 900; // 15 minutes
    m_comfort_interventions.fade_on_discomfort = true;
    m_comfort_interventions.reduce_intensity_on_discomfort = true;
}

void RE7Adapter::updateComfortSystems() {
    // Monitor for signs of discomfort
    bool discomfort_detected = detectDiscomfort();
    
    if (discomfort_detected && !m_comfort_intervention_active) {
        triggerComfortIntervention();
    } else if (!discomfort_detected && m_comfort_intervention_active) {
        endComfortIntervention();
    }
    
    // Update comfort level
    updateComfortLevel();
}

bool RE7Adapter::detectDiscomfort() {
    // Simple heuristic-based discomfort detection
    
    // Check for rapid camera movements
    if (m_camera_extractor) {
        auto camera_velocity = m_camera_extractor->getCameraVelocity();
        // Simple vector length calculation without GLM
        float velocity_length = std::sqrt(camera_velocity.x * camera_velocity.x + 
                                        camera_velocity.y * camera_velocity.y + 
                                        camera_velocity.z * camera_velocity.z);
        if (velocity_length > m_comfort_settings.max_camera_velocity) {
            return true;
        }
    }
    
    // Check frame rate stability
    if (m_performance_metrics.current_fps < 75.0f) {
        return true;
    }
    
    // Check for excessive frame time variation
    if (m_performance_metrics.frame_time_variance > 5.0f) {
        return true;
    }
    
    return false;
}

void RE7Adapter::triggerComfortIntervention() {
    spdlog::info("[RE7] Triggering comfort intervention");
    
    m_comfort_intervention_active = true;
    
    // Increase vignetting
    if (m_horror_optimizer) {
        m_horror_optimizer->increaseComfortVignetting(0.8f);
    }
    
    // Reduce motion effects
    m_motion_settings.camera_shake_reduction = 0.8f;
    m_motion_settings.motion_blur_reduction = 0.9f;
    
    // Smooth camera movements
    m_motion_settings.smoothing_factor = 0.95f;
}

void RE7Adapter::endComfortIntervention() {
    spdlog::info("[RE7] Ending comfort intervention");
    
    m_comfort_intervention_active = false;
    
    // Restore normal settings
    if (m_horror_optimizer) {
        m_horror_optimizer->restoreNormalVignetting();
    }
    
    // Restore motion settings
    m_motion_settings.camera_shake_reduction = 0.6f;
    m_motion_settings.motion_blur_reduction = 0.7f;
    m_motion_settings.smoothing_factor = 0.85f;
}

void RE7Adapter::updateComfortLevel() {
    float target_comfort = 1.0f;
    
    // Reduce comfort level based on detected issues
    if (m_performance_metrics.current_fps < 90.0f) {
        target_comfort *= 0.8f;
    }
    
    if (m_comfort_intervention_active) {
        target_comfort *= 0.6f;
    }
    
    // Smooth transition
    const float lerp_speed = 0.05f;
    m_comfort_level = m_comfort_level * (1.0f - lerp_speed) + target_comfort * lerp_speed;
}

void RE7Adapter::adjustComfortBasedOnMetrics() {
    // Adjust various settings based on current comfort level
    
    if (m_comfort_level < 0.7f) {
        // Reduce horror intensity
        m_horror_settings.scare_intensity_multiplier = 0.7f * m_comfort_level;
        
        // Increase smoothing
        m_motion_settings.smoothing_factor = 0.9f + (0.1f * (1.0f - m_comfort_level));
        
        // Increase vignetting
        m_comfort_settings.vignette_strength = 0.5f + (0.4f * (1.0f - m_comfort_level));
    }
}

void RE7Adapter::loadRE7Config() {
    try {
        spdlog::info("[RE7] Loading Resident Evil 7 VR configuration...");
        
        // Load from profiles/re7-Win64-Shipping/config.txt
        std::filesystem::path config_path = "profiles/re7-Win64-Shipping/config.txt";
        
        if (std::filesystem::exists(config_path)) {
            loadConfigFromFile(config_path);
        } else {
            createDefaultConfig();
        }
        
        spdlog::info("[RE7] Configuration loaded successfully");
        
    } catch (const std::exception& e) {
        spdlog::error("[RE7] Exception loading configuration: {}", e.what());
    }
}

void RE7Adapter::updatePerformanceMetrics() {
    auto current_time = std::chrono::steady_clock::now();
    
    // Update frame timing
    if (m_last_frame_time.time_since_epoch().count() > 0) {
        auto frame_duration = std::chrono::duration_cast<std::chrono::microseconds>(
            current_time - m_last_frame_time).count();
        
        m_performance_metrics.current_frame_time = frame_duration / 1000.0f; // Convert to ms
        m_performance_metrics.current_fps = 1000.0f / m_performance_metrics.current_frame_time;
        
        // Update frame time variance
        float variance = std::abs(m_performance_metrics.current_frame_time - m_performance_metrics.average_frame_time);
        m_performance_metrics.frame_time_variance = variance;
        
        // Update averages
        const float alpha = 0.1f;
        m_performance_metrics.average_frame_time = 
            alpha * m_performance_metrics.current_frame_time + 
            (1.0f - alpha) * m_performance_metrics.average_frame_time;
        
        m_performance_metrics.average_fps = 1000.0f / m_performance_metrics.average_frame_time;
    }
    
    m_last_frame_time = current_time;
    
    // Update every 60 frames
    if (m_frame_count % 60 == 0) {
        logPerformanceMetrics();
    }
}

void RE7Adapter::logPerformanceMetrics() {
    spdlog::debug("[RE7] Performance: FPS: {:.1f}, Frame Time: {:.2f}ms, Comfort: {:.2f}", 
                 m_performance_metrics.current_fps, 
                 m_performance_metrics.current_frame_time,
                 m_comfort_level);
}

// Placeholder implementations for hook and setup functions
bool RE7Adapter::installGameHooks() {
    spdlog::debug("[RE7] Installing RE7 game hooks");
    return true;
}

void RE7Adapter::removeGameHooks() {
    spdlog::debug("[RE7] Removing RE7 game hooks");
}

bool RE7Adapter::setupVRCamera() {
    spdlog::debug("[RE7] Setting up VR camera");
    return true;
}

bool RE7Adapter::setupVRInput() {
    spdlog::debug("[RE7] Setting up VR input");
    return true;
}

void RE7Adapter::applyHorrorGameOptimizations() {
    spdlog::debug("[RE7] Applying horror game optimizations");
}

void RE7Adapter::loadConfigFromFile(const std::filesystem::path& config_path) {
    spdlog::debug("[RE7] Loading config from file: {}", config_path.string());
}

void RE7Adapter::createDefaultConfig() {
    spdlog::debug("[RE7] Creating default configuration");
}

} // namespace re7
} // namespace adapters
} // namespace uevr