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
