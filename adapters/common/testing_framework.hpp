/*
 * Cross-Engine Comprehensive Testing Framework for UEVR Adapters
 * 
 * This header provides comprehensive testing capabilities for all
 * cross-engine VR adapters, ensuring quality and compatibility.
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
#include <functional>
#include <chrono>
#include <filesystem>
#include <mutex>
#include <spdlog/spdlog.h>

// Forward declarations for UEVR systems
namespace uevr {
    class VRSystem;
    class GraphicsSystem;
    class CPUMonitor;
    class MemoryMonitor;
    class PerformanceMonitor;
    class CrossEngineAdapter;
    class PluginManager;
    class InputSystem;
    class AudioSystem;
    class NetworkMonitor;
    class FileSystem;
    class ConfigManager;
    class LoggingSystem;
}

// Forward declarations for resource types
namespace uevr {
    class RenderTarget;
    class Shader;
    class Texture;
    class Buffer;
    class AudioBuffer;
    class AudioStream;
    class NetworkConnection;
    class NetworkBuffer;
}

namespace uevr_common {

/**
 * Comprehensive testing framework for cross-engine adapters
 * 
 * Provides testing capabilities for all cross-engine VR adapters
 * to ensure quality, compatibility, and performance.
 */
class TestingFramework {
public:
    // Test configuration
    struct TestConfig {
        bool enable_performance_testing = true;
        bool enable_compatibility_testing = true;
        bool enable_stability_testing = true;
        bool enable_vr_comfort_testing = true;
        bool enable_input_testing = true;
        bool enable_rendering_testing = true;
        bool enable_memory_testing = true;
        bool enable_hook_testing = true;
        UINT test_duration_seconds = 300; // 5 minutes
        UINT performance_sample_count = 1000;
        bool generate_test_reports = true;
        std::string test_output_directory = "test_results/";
    };

    // Test result
    struct TestResult {
        std::string test_name;
        bool passed = false;
        std::string error_message;
        float performance_score = 0.0f;
        UINT execution_time_ms = 0;
        std::vector<std::string> warnings;
        std::vector<std::string> recommendations;
    };

    // Performance metrics
    struct PerformanceMetrics {
        float average_fps = 0.0f;
        float min_fps = 0.0f;
        float max_fps = 0.0f;
        float frame_time_95th_percentile = 0.0f;
        float cpu_usage = 0.0f;
        float gpu_usage = 0.0f;
        float memory_usage = 0.0f;
        float vram_usage = 0.0f;
        UINT dropped_frames = 0;
        UINT total_frames = 0;
        
        // Enhanced VR metrics
        float vr_fps = 0.0f;
        float vr_frame_time = 0.0f;
        float vr_dropped_frames = 0.0f;
        float vr_reprojection_ratio = 0.0f;
        float vr_compositor_latency = 0.0f;
        std::string vr_performance_grade;
        bool vr_optimization_needed = false;
        
        // Enhanced GPU metrics
        float gpu_temperature = 0.0f;
        float gpu_memory_usage = 0.0f;
        float gpu_memory_total = 0.0f;
        float gpu_clock_speed = 0.0f;
        float gpu_power_consumption = 0.0f;
        UINT active_render_targets = 0;
        float render_target_memory = 0.0f;
        
        // Enhanced CPU metrics
        float cpu_temperature = 0.0f;
        float cpu_clock_speed = 0.0f;
        float cpu_power_consumption = 0.0f;
        UINT cpu_thread_count = 0;
        float cpu_cache_miss_rate = 0.0f;
        
        // Enhanced memory metrics
        float memory_available = 0.0f;
        float memory_total = 0.0f;
        float page_file_usage = 0.0f;
        float memory_fragmentation = 0.0f;
        float memory_pressure = 0.0f;
        
        // Enhanced frame metrics
        float frame_time_99th_percentile = 0.0f;
        float frame_time_variance = 0.0f;
        float frame_time_stability = 0.0f;
        
        // Cross-engine adapter metrics
        float adapter_processing_time = 0.0f;
        float adapter_memory_usage = 0.0f;
        UINT adapter_hook_count = 0;
        std::string adapter_optimization_level;
        
        // Plugin system metrics
        UINT active_plugins = 0;
        float plugin_memory_usage = 0.0f;
        float plugin_processing_time = 0.0f;
        UINT plugin_error_count = 0;
        
        // Input system metrics
        float input_latency = 0.0f;
        float input_update_rate = 0.0f;
        UINT input_device_count = 0;
        
        // Audio system metrics
        float audio_latency = 0.0f;
        UINT audio_buffer_underruns = 0;
        UINT audio_sample_rate = 0;
        
        // Network metrics
        float network_latency = 0.0f;
        float network_bandwidth = 0.0f;
        float network_packet_loss = 0.0f;
        
        // Overall performance
        float overall_performance_score = 0.0f;
        std::vector<std::string> performance_recommendations;
    };

    // Test suite
    struct TestSuite {
        std::string name;
        std::string description;
        std::vector<std::function<TestResult()>> tests;
        bool enabled = true;
    };

public:
    TestingFramework();
    ~TestingFramework();

    // Configuration management
    void set_test_config(const TestConfig& config);
    const TestConfig& get_test_config() const;
    
    // Test suite management
    void add_test_suite(const TestSuite& suite);
    void remove_test_suite(const std::string& name);
    void enable_test_suite(const std::string& name, bool enable);
    
    // Test execution
    std::vector<TestResult> run_all_tests();
    std::vector<TestResult> run_test_suite(const std::string& suite_name);
    TestResult run_single_test(const std::string& test_name);
    
    // Test reporting
    void generate_test_report(const std::vector<TestResult>& results);
    void export_test_results(const std::vector<TestResult>& results, const std::string& filename);
    
    // Built-in test suites
    void setup_performance_test_suite();
    void setup_compatibility_test_suite();
    void setup_stability_test_suite();
    void setup_vr_comfort_test_suite();
    void setup_input_test_suite();
    void setup_rendering_test_suite();
    void setup_memory_test_suite();
    void setup_hook_test_suite();

private:
    TestConfig m_config;
    std::vector<TestSuite> m_test_suites;
    std::unordered_map<std::string, size_t> m_test_suite_map;
    
    // System integration pointers
    std::shared_ptr<uevr::VRSystem> m_vr_system;
    std::shared_ptr<uevr::GraphicsSystem> m_graphics_system;
    std::shared_ptr<uevr::CPUMonitor> m_cpu_monitor;
    std::shared_ptr<uevr::MemoryMonitor> m_memory_monitor;
    std::shared_ptr<uevr::PerformanceMonitor> m_performance_monitor;
    std::shared_ptr<uevr::CrossEngineAdapter> m_cross_engine_adapter;
    std::shared_ptr<uevr::PluginManager> m_plugin_manager;
    std::shared_ptr<uevr::InputSystem> m_input_system;
    std::shared_ptr<uevr::AudioSystem> m_audio_system;
    std::shared_ptr<uevr::NetworkMonitor> m_network_monitor;
    std::shared_ptr<uevr::FileSystem> m_file_system;
    std::shared_ptr<uevr::ConfigManager> m_config_manager;
    std::shared_ptr<uevr::LoggingSystem> m_logging_system;
    
    // Test resource management
    std::vector<std::shared_ptr<uevr::RenderTarget>> m_vr_render_targets;
    std::vector<std::shared_ptr<uevr::Texture>> m_vr_test_textures;
    std::vector<std::shared_ptr<uevr::RenderTarget>> m_test_render_targets;
    std::vector<std::shared_ptr<uevr::Shader>> m_test_shaders;
    std::vector<std::shared_ptr<uevr::Texture>> m_test_textures;
    std::vector<std::shared_ptr<uevr::Buffer>> m_test_buffers;
    std::vector<std::string> m_test_game_profiles;
    std::vector<std::string> m_test_adapter_configs;
    std::vector<std::shared_ptr<void>> m_test_plugins;
    std::vector<std::string> m_test_plugin_configs;
    std::vector<std::shared_ptr<void>> m_test_input_devices;
    std::vector<std::string> m_test_input_mappings;
    std::vector<std::shared_ptr<uevr::AudioBuffer>> m_test_audio_buffers;
    std::vector<std::shared_ptr<uevr::AudioStream>> m_test_audio_streams;
    std::vector<std::shared_ptr<void>> m_test_memory_allocations;
    std::vector<std::string> m_test_memory_pools;
    std::vector<std::string> m_test_performance_data;
    std::vector<std::shared_ptr<uevr::NetworkConnection>> m_test_network_connections;
    std::vector<std::shared_ptr<uevr::NetworkBuffer>> m_test_network_buffers;
    std::vector<std::filesystem::path> m_test_files;
    std::vector<std::filesystem::path> m_test_directories;
    std::vector<std::string> m_test_configurations;
    
    // Test state management
    bool m_test_in_progress = false;
    std::chrono::steady_clock::time_point m_test_start_time;
    std::chrono::steady_clock::time_point m_test_end_time;
    std::vector<PerformanceMetrics> m_test_metrics_history;
    std::vector<std::string> m_test_error_logs;
    std::vector<std::string> m_test_warning_logs;
    std::vector<std::string> m_test_debug_logs;
    
    // Test phase tracking
    enum class TestPhase {
        None,
        Initialization,
        Execution,
        Monitoring,
        Cleanup,
        Reporting
    };
    TestPhase m_current_test_phase = TestPhase::None;
    
    // Internal testing helpers
    TestResult run_performance_test();
    TestResult run_compatibility_test();
    TestResult run_stability_test();
    TestResult run_vr_comfort_test();
    TestResult run_input_test();
    TestResult run_rendering_test();
    TestResult run_memory_test();
    TestResult run_hook_test();
    
    PerformanceMetrics collect_performance_metrics();
    void cleanup_test_resources();
    
    // Helper functions for performance metrics
    std::string calculateVRPerformanceGrade(float fps, float frame_time);
    bool needsVROptimization(float fps, float dropped_frames);
    float calculateMemoryPressure(float physical_usage, float page_file_usage);
    float calculateFrameTimeStability(float variance, float fps);
    float calculateOverallPerformanceScore(float fps, float cpu, float gpu, float memory, float stability);
    std::vector<std::string> generatePerformanceRecommendations(const PerformanceMetrics& metrics);
    void updatePerformanceHistory(const PerformanceMetrics& metrics);
    PerformanceMetrics getFallbackPerformanceMetrics();
    
    // Helper functions for resource cleanup
    bool verifyCleanupSuccess();
    void emergencyCleanup();
    
    // System integration helpers
    void initializeSystemIntegrations();
    void releaseSystemIntegrations();
};

/**
 * RE Engine specific testing framework
 * 
 * Provides testing capabilities specific to Capcom's RE Engine games.
 */
class REEngineTestingFramework : public TestingFramework {
public:
    REEngineTestingFramework();
    
    // RE Engine specific tests
    TestResult test_horror_game_comfort();
    TestResult test_fixed_camera_compatibility();
    TestResult test_atmospheric_lighting_performance();
    TestResult test_weapon_interaction_stability();
    
private:
    void setup_re_engine_test_suites();
};

/**
 * REDengine 4 specific testing framework
 * 
 * Provides testing capabilities specific to CD Projekt Red's REDengine 4 games.
 */
class REDengine4TestingFramework : public TestingFramework {
public:
    REDengine4TestingFramework();
    
    // REDengine 4 specific tests
    TestResult test_urban_environment_performance();
    TestResult test_ray_tracing_compatibility();
    TestResult test_volumetric_effects_stability();
    TestResult test_hacking_interface_functionality();
    
private:
    void setup_redengine4_test_suites();
};

/**
 * MT Framework specific testing framework
 * 
 * Provides testing capabilities specific to Capcom's MT Framework games.
 */
class MTFrameworkTestingFramework : public TestingFramework {
public:
    MTFrameworkTestingFramework();
    
    // MT Framework specific tests
    TestResult test_combat_scene_performance();
    TestResult test_third_person_conversion();
    TestResult test_monster_interaction_stability();
    TestResult test_weapon_switching_functionality();
    
private:
    void setup_mt_framework_test_suites();
};

/**
 * Global testing framework manager
 * 
 * Provides centralized access to testing framework features.
 */
class GlobalTestingManager {
public:
    static TestingFramework& get_testing_framework();
    static REEngineTestingFramework& get_re_engine_testing_framework();
    static REDengine4TestingFramework& get_redengine4_testing_framework();
    static MTFrameworkTestingFramework& get_mt_framework_testing_framework();
    
    // Global configuration
    static void configure_testing(const TestConfig& config);
    static void set_global_testing_mode(const std::string& mode);
    
    // Global cleanup
    static void cleanup_all();

private:
    static std::unique_ptr<TestingFramework> s_global_testing_framework;
    static std::unique_ptr<REEngineTestingFramework> s_re_engine_testing_framework;
    static std::unique_ptr<REDengine4TestingFramework> s_redengine4_testing_framework;
    static std::unique_ptr<MTFrameworkTestingFramework> s_mt_framework_testing_framework;
    static std::mutex s_init_mutex;
    
    static void initialize_if_needed();
};

} // namespace uevr_common
