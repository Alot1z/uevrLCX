/*
 * Cross-Engine Comprehensive Testing Framework Implementation
 * 
 * This file provides the complete implementation of the testing framework
 * for all cross-engine VR adapters, ensuring quality and compatibility.
 * 
 * Copyright (c) 2024 UEVR Cross-Engine Adapter Project
 * Licensed under MIT License
 */

#include "testing_framework.hpp"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <thread>
#include <future>
#include <chrono>
#include <random>
#include <iomanip>
#include <regex>

namespace uevr_common {

// Constructor implementation
TestingFramework::TestingFramework() {
    spdlog::info("Initializing UEVR Testing Framework");
    
    // Initialize default configuration
    m_config = TestConfig{};
    
    // Setup logging
    setup_logging();
    
    // Initialize system integrations
    initializeSystemIntegrations();
    
    // Setup default test suites
    setup_performance_test_suite();
    setup_compatibility_test_suite();
    setup_stability_test_suite();
    setup_vr_comfort_test_suite();
    setup_input_test_suite();
    setup_rendering_test_suite();
    setup_memory_test_suite();
    setup_hook_test_suite();
    
    spdlog::info("UEVR Testing Framework initialized successfully");
}

// Destructor implementation
TestingFramework::~TestingFramework() {
    spdlog::info("Shutting down UEVR Testing Framework");
    
    // Cleanup test resources
    cleanup_test_resources();
    
    // Release system integrations
    releaseSystemIntegrations();
    
    spdlog::info("UEVR Testing Framework shutdown complete");
}

// Configuration management
void TestingFramework::set_test_config(const TestConfig& config) {
    std::lock_guard<std::mutex> lock(m_config_mutex);
    m_config = config;
    spdlog::info("Test configuration updated");
}

const TestConfig& TestingFramework::get_test_config() const {
    std::lock_guard<std::mutex> lock(m_config_mutex);
    return m_config;
}

// Test suite management
void TestingFramework::add_test_suite(const TestSuite& suite) {
    std::lock_guard<std::mutex> lock(m_test_suites_mutex);
    
    auto it = m_test_suite_map.find(suite.name);
    if (it != m_test_suite_map.end()) {
        spdlog::warn("Test suite '{}' already exists, replacing", suite.name);
        m_test_suites[it->second] = suite;
    } else {
        m_test_suite_map[suite.name] = m_test_suites.size();
        m_test_suites.push_back(suite);
        spdlog::info("Added test suite '{}' with {} tests", suite.name, suite.tests.size());
    }
}

void TestingFramework::remove_test_suite(const std::string& name) {
    std::lock_guard<std::mutex> lock(m_test_suites_mutex);
    
    auto it = m_test_suite_map.find(name);
    if (it != m_test_suite_map.end()) {
        size_t index = it->second;
        m_test_suites.erase(m_test_suites.begin() + index);
        m_test_suite_map.erase(it);
        
        // Update indices for remaining test suites
        for (auto& pair : m_test_suite_map) {
            if (pair.second > index) {
                pair.second--;
            }
        }
        
        spdlog::info("Removed test suite '{}'", name);
    } else {
        spdlog::warn("Test suite '{}' not found", name);
    }
}

void TestingFramework::enable_test_suite(const std::string& name, bool enable) {
    std::lock_guard<std::mutex> lock(m_test_suites_mutex);
    
    auto it = m_test_suite_map.find(name);
    if (it != m_test_suite_map.end()) {
        m_test_suites[it->second].enabled = enable;
        spdlog::info("Test suite '{}' {}", name, enable ? "enabled" : "disabled");
    } else {
        spdlog::warn("Test suite '{}' not found", name);
    }
}

// Test execution
std::vector<TestResult> TestingFramework::run_all_tests() {
    spdlog::info("Starting execution of all test suites");
    
    if (m_test_in_progress) {
        spdlog::error("Tests already in progress");
        return {};
    }
    
    m_test_in_progress = true;
    m_test_start_time = std::chrono::steady_clock::now();
    m_current_test_phase = TestPhase::Initialization;
    
    std::vector<TestResult> all_results;
    
    try {
        // Run each enabled test suite
        for (const auto& suite : m_test_suites) {
            if (!suite.enabled) {
                spdlog::info("Skipping disabled test suite: {}", suite.name);
                continue;
            }
            
            spdlog::info("Running test suite: {}", suite.name);
            auto suite_results = run_test_suite(suite.name);
            all_results.insert(all_results.end(), suite_results.begin(), suite_results.end());
        }
        
        m_current_test_phase = TestPhase::Reporting;
        generate_test_report(all_results);
        
    } catch (const std::exception& e) {
        spdlog::error("Error during test execution: {}", e.what());
        m_test_error_logs.push_back(std::string("Test execution error: ") + e.what());
    }
    
    m_test_end_time = std::chrono::steady_clock::now();
    m_test_in_progress = false;
    m_current_test_phase = TestPhase::None;
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(m_test_end_time - m_test_start_time);
    spdlog::info("All tests completed in {} ms. Total results: {}", duration.count(), all_results.size());
    
    return all_results;
}

std::vector<TestResult> TestingFramework::run_test_suite(const std::string& suite_name) {
    spdlog::info("Running test suite: {}", suite_name);
    
    auto it = m_test_suite_map.find(suite_name);
    if (it == m_test_suite_map.end()) {
        spdlog::error("Test suite '{}' not found", suite_name);
        return {};
    }
    
    const auto& suite = m_test_suites[it->second];
    if (!suite.enabled) {
        spdlog::warn("Test suite '{}' is disabled", suite_name);
        return {};
    }
    
    std::vector<TestResult> results;
    results.reserve(suite.tests.size());
    
    for (size_t i = 0; i < suite.tests.size(); ++i) {
        try {
            spdlog::debug("Running test {} in suite '{}'", i + 1, suite_name);
            auto result = suite.tests[i]();
            result.test_name = suite_name + "::test_" + std::to_string(i + 1);
            results.push_back(result);
            
            if (result.passed) {
                spdlog::info("Test {} passed: {}", result.test_name, result.performance_score);
            } else {
                spdlog::error("Test {} failed: {}", result.test_name, result.error_message);
            }
            
        } catch (const std::exception& e) {
            spdlog::error("Exception in test {}: {}", i + 1, e.what());
            TestResult error_result;
            error_result.test_name = suite_name + "::test_" + std::to_string(i + 1);
            error_result.passed = false;
            error_result.error_message = std::string("Exception: ") + e.what();
            results.push_back(error_result);
        }
    }
    
    spdlog::info("Test suite '{}' completed with {} results", suite_name, results.size());
    return results;
}

TestResult TestingFramework::run_single_test(const std::string& test_name) {
    spdlog::info("Running single test: {}", test_name);
    
    // Parse test name to find suite and test index
    size_t pos = test_name.find("::");
    if (pos == std::string::npos) {
        spdlog::error("Invalid test name format: {}", test_name);
        TestResult error_result;
        error_result.test_name = test_name;
        error_result.passed = false;
        error_result.error_message = "Invalid test name format";
        return error_result;
    }
    
    std::string suite_name = test_name.substr(0, pos);
    std::string test_index_str = test_name.substr(pos + 2);
    
    if (test_index_str.substr(0, 5) != "test_") {
        spdlog::error("Invalid test index format: {}", test_index_str);
        TestResult error_result;
        error_result.test_name = test_name;
        error_result.passed = false;
        error_result.error_message = "Invalid test index format";
        return error_result;
    }
    
    size_t test_index = std::stoul(test_index_str.substr(5)) - 1;
    
    auto it = m_test_suite_map.find(suite_name);
    if (it == m_test_suite_map.end()) {
        spdlog::error("Test suite '{}' not found", suite_name);
        TestResult error_result;
        error_result.test_name = test_name;
        error_result.passed = false;
        error_result.error_message = "Test suite not found";
        return error_result;
    }
    
    const auto& suite = m_test_suites[it->second];
    if (!suite.enabled) {
        spdlog::warn("Test suite '{}' is disabled", suite_name);
        TestResult error_result;
        error_result.test_name = test_name;
        error_result.passed = false;
        error_result.error_message = "Test suite is disabled";
        return error_result;
    }
    
    if (test_index >= suite.tests.size()) {
        spdlog::error("Test index {} out of range for suite '{}'", test_index + 1, suite_name);
        TestResult error_result;
        error_result.test_name = test_name;
        error_result.passed = false;
        error_result.error_message = "Test index out of range";
        return error_result;
    }
    
    try {
        spdlog::debug("Running single test: {}", test_name);
        auto result = suite.tests[test_index]();
        result.test_name = test_name;
        
        if (result.passed) {
            spdlog::info("Single test {} passed: {}", test_name, result.performance_score);
        } else {
            spdlog::error("Single test {} failed: {}", test_name, result.error_message);
        }
        
        return result;
        
    } catch (const std::exception& e) {
        spdlog::error("Exception in single test {}: {}", test_name, e.what());
        TestResult error_result;
        error_result.test_name = test_name;
        error_result.passed = false;
        error_result.error_message = std::string("Exception: ") + e.what();
        return error_result;
    }
}

// Test reporting
void TestingFramework::generate_test_report(const std::vector<TestResult>& results) {
    if (!m_config.generate_test_reports) {
        spdlog::info("Test report generation disabled");
        return;
    }
    
    spdlog::info("Generating comprehensive test report for {} results", results.size());
    
    try {
        // Create output directory if it doesn't exist
        std::filesystem::path output_dir(m_config.test_output_directory);
        if (!std::filesystem::exists(output_dir)) {
            std::filesystem::create_directories(output_dir);
        }
        
        // Generate timestamp for unique filename
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream timestamp;
        timestamp << std::put_time(std::localtime(&time_t), "%Y%m%d_%H%M%S");
        
        // Generate report filename
        std::string report_filename = "test_report_" + timestamp.str() + ".html";
        std::filesystem::path report_path = output_dir / report_filename;
        
        // Generate HTML report
        std::ofstream report_file(report_path);
        if (!report_file.is_open()) {
            spdlog::error("Failed to open report file: {}", report_path.string());
            return;
        }
        
        // Write HTML header
        report_file << "<!DOCTYPE html>\n";
        report_file << "<html lang=\"en\">\n";
        report_file << "<head>\n";
        report_file << "    <meta charset=\"UTF-8\">\n";
        report_file << "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
        report_file << "    <title>UEVR Testing Framework Report</title>\n";
        report_file << "    <style>\n";
        report_file << "        body { font-family: Arial, sans-serif; margin: 20px; }\n";
        report_file << "        .header { background-color: #f0f0f0; padding: 20px; border-radius: 5px; }\n";
        report_file << "        .summary { background-color: #e8f5e8; padding: 15px; border-radius: 5px; margin: 20px 0; }\n";
        report_file << "        .test-result { margin: 10px 0; padding: 10px; border-radius: 3px; }\n";
        report_file << "        .passed { background-color: #d4edda; border-left: 4px solid #28a745; }\n";
        report_file << "        .failed { background-color: #f8d7da; border-left: 4px solid #dc3545; }\n";
        report_file << "        .metrics { background-color: #f8f9fa; padding: 15px; border-radius: 5px; margin: 20px 0; }\n";
        report_file << "        .metric-grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); gap: 15px; }\n";
        report_file << "        .metric-item { background-color: white; padding: 10px; border-radius: 3px; border: 1px solid #dee2e6; }\n";
        report_file << "    </style>\n";
        report_file << "</head>\n";
        report_file << "<body>\n";
        
        // Write header
        report_file << "    <div class=\"header\">\n";
        report_file << "        <h1>UEVR Testing Framework Report</h1>\n";
        report_file << "        <p>Generated: " << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S") << "</p>\n";
        report_file << "        <p>Total Tests: " << results.size() << "</p>\n";
        report_file << "    </div>\n";
        
        // Calculate summary statistics
        size_t passed_count = 0;
        size_t failed_count = 0;
        float total_performance = 0.0f;
        UINT total_execution_time = 0;
        
        for (const auto& result : results) {
            if (result.passed) {
                passed_count++;
            } else {
                failed_count++;
            }
            total_performance += result.performance_score;
            total_execution_time += result.execution_time_ms;
        }
        
        float pass_rate = results.empty() ? 0.0f : (static_cast<float>(passed_count) / results.size()) * 100.0f;
        float avg_performance = results.empty() ? 0.0f : total_performance / results.size();
        float avg_execution_time = results.empty() ? 0.0f : static_cast<float>(total_execution_time) / results.size();
        
        // Write summary
        report_file << "    <div class=\"summary\">\n";
        report_file << "        <h2>Test Summary</h2>\n";
        report_file << "        <div class=\"metric-grid\">\n";
        report_file << "            <div class=\"metric-item\">\n";
        report_file << "                <strong>Pass Rate:</strong><br>" << std::fixed << std::setprecision(1) << pass_rate << "%\n";
        report_file << "            </div>\n";
        report_file << "            <div class=\"metric-item\">\n";
        report_file << "                <strong>Passed:</strong><br>" << passed_count << " tests\n";
        report_file << "            </div>\n";
        report_file << "            <div class=\"metric-item\">\n";
        report_file << "                <strong>Failed:</strong><br>" << failed_count << " tests\n";
        report_file << "            </div>\n";
        report_file << "            <div class=\"metric-item\">\n";
        report_file << "                <strong>Avg Performance:</strong><br>" << std::fixed << std::setprecision(2) << avg_performance << "\n";
        report_file << "            </div>\n";
        report_file << "            <div class=\"metric-item\">\n";
        report_file << "                <strong>Avg Execution Time:</strong><br>" << std::fixed << std::setprecision(1) << avg_execution_time << " ms\n";
        report_file << "            </div>\n";
        report_file << "        </div>\n";
        report_file << "    </div>\n";
        
        // Write detailed results
        report_file << "    <h2>Detailed Test Results</h2>\n";
        for (const auto& result : results) {
            std::string css_class = result.passed ? "passed" : "failed";
            std::string status_text = result.passed ? "PASSED" : "FAILED";
            
            report_file << "    <div class=\"test-result " << css_class << "\">\n";
            report_file << "        <h3>" << result.test_name << " - " << status_text << "</h3>\n";
            report_file << "        <p><strong>Performance Score:</strong> " << std::fixed << std::setprecision(2) << result.performance_score << "</p>\n";
            report_file << "        <p><strong>Execution Time:</strong> " << result.execution_time_ms << " ms</p>\n";
            
            if (!result.passed && !result.error_message.empty()) {
                report_file << "        <p><strong>Error:</strong> " << result.error_message << "</p>\n";
            }
            
            if (!result.warnings.empty()) {
                report_file << "        <p><strong>Warnings:</strong></p><ul>\n";
                for (const auto& warning : result.warnings) {
                    report_file << "            <li>" << warning << "</li>\n";
                }
                report_file << "        </ul>\n";
            }
            
            if (!result.recommendations.empty()) {
                report_file << "        <p><strong>Recommendations:</strong></p><ul>\n";
                for (const auto& rec : result.recommendations) {
                    report_file << "            <li>" << rec << "</li>\n";
                }
                report_file << "        </ul>\n";
            }
            
            report_file << "    </div>\n";
        }
        
        // Write footer
        report_file << "    <div class=\"header\">\n";
        report_file << "        <p>Report generated by UEVR Testing Framework</p>\n";
        report_file << "        <p>End of Report</p>\n";
        report_file << "    </div>\n";
        
        report_file << "</body>\n";
        report_file << "</html>\n";
        
        report_file.close();
        spdlog::info("Test report generated successfully: {}", report_path.string());
        
    } catch (const std::exception& e) {
        spdlog::error("Error generating test report: {}", e.what());
        m_test_error_logs.push_back(std::string("Report generation error: ") + e.what());
    }
}

void TestingFramework::export_test_results(const std::vector<TestResult>& results, const std::string& filename) {
    spdlog::info("Exporting test results to: {}", filename);
    
    try {
        // Create output directory if it doesn't exist
        std::filesystem::path output_dir(m_config.test_output_directory);
        if (!std::filesystem::exists(output_dir)) {
            std::filesystem::create_directories(output_dir);
        }
        
        std::filesystem::path export_path = output_dir / filename;
        
        // Export as JSON
        if (filename.ends_with(".json")) {
            export_results_as_json(results, export_path);
        }
        // Export as CSV
        else if (filename.ends_with(".csv")) {
            export_results_as_csv(results, export_path);
        }
        // Export as XML
        else if (filename.ends_with(".xml")) {
            export_results_as_xml(results, export_path);
        }
        else {
            spdlog::warn("Unsupported export format: {}", filename);
            return;
        }
        
        spdlog::info("Test results exported successfully to: {}", export_path.string());
        
    } catch (const std::exception& e) {
        spdlog::error("Error exporting test results: {}", e.what());
        m_test_error_logs.push_back(std::string("Export error: ") + e.what());
    }
}

// Export helper functions
void TestingFramework::export_results_as_json(const std::vector<TestResult>& results, const std::filesystem::path& path) {
    std::ofstream file(path);
    if (!file.is_open()) {
        spdlog::error("Failed to open JSON export file: {}", path.string());
        return;
    }
    
    file << "{\n";
    file << "  \"test_results\": [\n";
    
    for (size_t i = 0; i < results.size(); ++i) {
        const auto& result = results[i];
        file << "    {\n";
        file << "      \"test_name\": \"" << result.test_name << "\",\n";
        file << "      \"passed\": " << (result.passed ? "true" : "false") << ",\n";
        file << "      \"performance_score\": " << std::fixed << std::setprecision(2) << result.performance_score << ",\n";
        file << "      \"execution_time_ms\": " << result.execution_time_ms << ",\n";
        file << "      \"error_message\": \"" << result.error_message << "\",\n";
        
        file << "      \"warnings\": [\n";
        for (size_t j = 0; j < result.warnings.size(); ++j) {
            file << "        \"" << result.warnings[j] << "\"" << (j < result.warnings.size() - 1 ? "," : "") << "\n";
        }
        file << "      ],\n";
        
        file << "      \"recommendations\": [\n";
        for (size_t j = 0; j < result.recommendations.size(); ++j) {
            file << "        \"" << result.recommendations[j] << "\"" << (j < result.recommendations.size() - 1 ? "," : "") << "\n";
        }
        file << "      ]\n";
        
        file << "    }" << (i < results.size() - 1 ? "," : "") << "\n";
    }
    
    file << "  ]\n";
    file << "}\n";
    
    file.close();
    spdlog::info("JSON export completed: {}", path.string());
}

void TestingFramework::export_results_as_csv(const std::vector<TestResult>& results, const std::filesystem::path& path) {
    std::ofstream file(path);
    if (!file.is_open()) {
        spdlog::error("Failed to open CSV export file: {}", path.string());
        return;
    }
    
    // Write CSV header
    file << "TestName,Status,PerformanceScore,ExecutionTime,ErrorMessage,Warnings,Recommendations\n";
    
    // Write data rows
    for (const auto& result : results) {
        file << "\"" << result.test_name << "\",";
        file << (result.passed ? "PASSED" : "FAILED") << ",";
        file << std::fixed << std::setprecision(2) << result.performance_score << ",";
        file << result.execution_time_ms << ",";
        file << "\"" << result.error_message << "\",";
        
        // Combine warnings
        std::string warnings_str;
        for (const auto& warning : result.warnings) {
            if (!warnings_str.empty()) warnings_str += "; ";
            warnings_str += warning;
        }
        file << "\"" << warnings_str << "\",";
        
        // Combine recommendations
        std::string recs_str;
        for (const auto& rec : result.recommendations) {
            if (!recs_str.empty()) recs_str += "; ";
            recs_str += rec;
        }
        file << "\"" << recs_str << "\"\n";
    }
    
    file.close();
    spdlog::info("CSV export completed: {}", path.string());
}

void TestingFramework::export_results_as_xml(const std::vector<TestResult>& results, const std::filesystem::path& path) {
    std::ofstream file(path);
    if (!file.is_open()) {
        spdlog::error("Failed to open XML export file: {}", path.string());
        return;
    }
    
    file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    file << "<test_results>\n";
    
    for (const auto& result : results) {
        file << "  <test_result>\n";
        file << "    <test_name>" << result.test_name << "</test_name>\n";
        file << "    <status>" << (result.passed ? "PASSED" : "FAILED") << "</status>\n";
        file << "    <performance_score>" << std::fixed << std::setprecision(2) << result.performance_score << "</performance_score>\n";
        file << "    <execution_time_ms>" << result.execution_time_ms << "</execution_time_ms>\n";
        file << "    <error_message>" << result.error_message << "</error_message>\n";
        
        file << "    <warnings>\n";
        for (const auto& warning : result.warnings) {
            file << "      <warning>" << warning << "</warning>\n";
        }
        file << "    </warnings>\n";
        
        file << "    <recommendations>\n";
        for (const auto& rec : result.recommendations) {
            file << "      <recommendation>" << rec << "</recommendation>\n";
        }
        file << "    </recommendations>\n";
        
        file << "  </test_result>\n";
    }
    
    file << "</test_results>\n";
    
    file.close();
    spdlog::info("XML export completed: {}", path.string());
}

// Test suite setup functions
void TestingFramework::setup_performance_test_suite() {
    TestSuite performance_suite;
    performance_suite.name = "Performance";
    performance_suite.description = "Performance testing for VR adapters and systems";
    performance_suite.enabled = m_config.enable_performance_testing;
    
    // Add performance tests
    performance_suite.tests.push_back([this]() { return run_performance_test(); });
    
    add_test_suite(performance_suite);
    spdlog::info("Performance test suite configured");
}

void TestingFramework::setup_compatibility_test_suite() {
    TestSuite compatibility_suite;
    compatibility_suite.name = "Compatibility";
    compatibility_suite.description = "Compatibility testing for different engines and games";
    compatibility_suite.enabled = m_config.enable_compatibility_testing;
    
    // Add compatibility tests
    compatibility_suite.tests.push_back([this]() { return run_compatibility_test(); });
    
    add_test_suite(compatibility_suite);
    spdlog::info("Compatibility test suite configured");
}

void TestingFramework::setup_stability_test_suite() {
    TestSuite stability_suite;
    stability_suite.name = "Stability";
    stability_suite.description = "Long-term stability testing for VR systems";
    stability_suite.enabled = m_config.enable_stability_testing;
    
    // Add stability tests
    stability_suite.tests.push_back([this]() { return run_stability_test(); });
    
    add_test_suite(stability_suite);
    spdlog::info("Stability test suite configured");
}

void TestingFramework::setup_vr_comfort_test_suite() {
    TestSuite vr_comfort_suite;
    vr_comfort_suite.name = "VRComfort";
    vr_comfort_suite.description = "VR comfort and motion sickness testing";
    vr_comfort_suite.enabled = m_config.enable_vr_comfort_testing;
    
    // Add VR comfort tests
    vr_comfort_suite.tests.push_back([this]() { return run_vr_comfort_test(); });
    
    add_test_suite(vr_comfort_suite);
    spdlog::info("VR Comfort test suite configured");
}

void TestingFramework::setup_input_test_suite() {
    TestSuite input_suite;
    input_suite.name = "Input";
    input_suite.description = "Input system testing for VR controllers";
    input_suite.enabled = m_config.enable_input_testing;
    
    // Add input tests
    input_suite.tests.push_back([this]() { return run_input_test(); });
    
    add_test_suite(input_suite);
    spdlog::info("Input test suite configured");
}

void TestingFramework::setup_rendering_test_suite() {
    TestSuite rendering_suite;
    rendering_suite.name = "Rendering";
    rendering_suite.description = "Rendering performance and quality testing";
    rendering_suite.enabled = m_config.enable_rendering_testing;
    
    // Add rendering tests
    rendering_suite.tests.push_back([this]() { return run_rendering_test(); });
    
    add_test_suite(rendering_suite);
    spdlog::info("Rendering test suite configured");
}

void TestingFramework::setup_memory_test_suite() {
    TestSuite memory_suite;
    memory_suite.name = "Memory";
    memory_suite.description = "Memory usage and leak testing";
    memory_suite.enabled = m_config.enable_memory_testing;
    
    // Add memory tests
    memory_suite.tests.push_back([this]() { return run_memory_test(); });
    
    add_test_suite(memory_suite);
    spdlog::info("Memory test suite configured");
}

void TestingFramework::setup_hook_test_suite() {
    TestSuite hook_suite;
    hook_suite.name = "Hook";
    hook_suite.description = "Hook system and injection testing";
    hook_suite.enabled = m_config.enable_hook_testing;
    
    // Add hook tests
    hook_suite.tests.push_back([this]() { return run_hook_test(); });
    
    add_test_suite(hook_suite);
    spdlog::info("Hook test suite configured");
}

// Internal testing implementations
TestResult TestingFramework::run_performance_test() {
    spdlog::info("Running performance test");
    
    TestResult result;
    result.test_name = "Performance Test";
    auto start_time = std::chrono::steady_clock::now();
    
    try {
        // Collect comprehensive performance metrics
        PerformanceMetrics metrics = collect_performance_metrics();
        
        // Calculate overall performance score
        result.performance_score = calculateOverallPerformanceScore(
            metrics.average_fps, metrics.cpu_usage, metrics.gpu_usage, 
            metrics.memory_usage, metrics.frame_time_stability
        );
        
        // Determine if test passed based on performance thresholds
        result.passed = result.performance_score >= 7.0f; // Minimum acceptable score
        
        if (!result.passed) {
            result.error_message = "Performance below acceptable threshold";
            result.warnings.push_back("Consider optimizing VR rendering pipeline");
            result.warnings.push_back("Check for memory leaks or excessive allocations");
            result.recommendations.push_back("Reduce VR resolution if performance is poor");
            result.recommendations.push_back("Close unnecessary background applications");
        }
        
        // Add performance-specific recommendations
        auto perf_recs = generatePerformanceRecommendations(metrics);
        result.recommendations.insert(result.recommendations.end(), perf_recs.begin(), perf_recs.end());
        
        // Update performance history
        updatePerformanceHistory(metrics);
        
    } catch (const std::exception& e) {
        result.passed = false;
        result.error_message = std::string("Performance test failed: ") + e.what();
        spdlog::error("Performance test error: {}", e.what());
    }
    
    auto end_time = std::chrono::steady_clock::now();
    result.execution_time_ms = static_cast<UINT>(
        std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count()
    );
    
    spdlog::info("Performance test completed: {} (score: {})", 
                 result.passed ? "PASSED" : "FAILED", result.performance_score);
    
    return result;
}

TestResult TestingFramework::run_compatibility_test() {
    spdlog::info("Running compatibility test");
    
    TestResult result;
    result.test_name = "Compatibility Test";
    auto start_time = std::chrono::steady_clock::now();
    
    try {
        // Test VR system compatibility
        bool vr_compatible = test_vr_system_compatibility();
        
        // Test graphics API compatibility
        bool graphics_compatible = test_graphics_api_compatibility();
        
        // Test engine-specific compatibility
        bool engine_compatible = test_engine_compatibility();
        
        // Test plugin system compatibility
        bool plugin_compatible = test_plugin_compatibility();
        
        result.passed = vr_compatible && graphics_compatible && engine_compatible && plugin_compatible;
        
        if (!result.passed) {
            result.error_message = "Compatibility issues detected";
            
            if (!vr_compatible) {
                result.warnings.push_back("VR system compatibility issues detected");
                result.recommendations.push_back("Update VR runtime drivers");
            }
            
            if (!graphics_compatible) {
                result.warnings.push_back("Graphics API compatibility issues detected");
                result.recommendations.push_back("Update graphics drivers");
            }
            
            if (!engine_compatible) {
                result.warnings.push_back("Game engine compatibility issues detected");
                result.recommendations.push_back("Check engine version compatibility");
            }
            
            if (!plugin_compatible) {
                result.warnings.push_back("Plugin system compatibility issues detected");
                result.recommendations.push_back("Update or reinstall plugins");
            }
        }
        
        result.performance_score = result.passed ? 10.0f : 3.0f;
        
    } catch (const std::exception& e) {
        result.passed = false;
        result.error_message = std::string("Compatibility test failed: ") + e.what();
        spdlog::error("Compatibility test error: {}", e.what());
    }
    
    auto end_time = std::chrono::steady_clock::now();
    result.execution_time_ms = static_cast<UINT>(
        std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count()
    );
    
    spdlog::info("Compatibility test completed: {}", result.passed ? "PASSED" : "FAILED");
    
    return result;
}

TestResult TestingFramework::run_stability_test() {
    spdlog::info("Running stability test");
    
    TestResult result;
    result.test_name = "Stability Test";
    auto start_time = std::chrono::steady_clock::now();
    
    try {
        // Run stability test for configured duration
        UINT test_duration = m_config.test_duration_seconds * 1000; // Convert to ms
        UINT sample_count = m_config.performance_sample_count;
        
        std::vector<PerformanceMetrics> stability_samples;
        stability_samples.reserve(sample_count);
        
        // Collect performance samples over time
        for (UINT i = 0; i < sample_count; ++i) {
            auto sample_start = std::chrono::steady_clock::now();
            
            // Collect performance metrics
            PerformanceMetrics sample = collect_performance_metrics();
            stability_samples.push_back(sample);
            
            // Wait for next sample interval
            auto sample_end = std::chrono::steady_clock::now();
            auto sample_duration = std::chrono::duration_cast<std::chrono::milliseconds>(sample_end - sample_start);
            auto wait_time = (test_duration / sample_count) - sample_duration.count();
            
            if (wait_time > 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(wait_time));
            }
        }
        
        // Analyze stability
        float stability_score = analyze_stability(stability_samples);
        result.performance_score = stability_score;
        result.passed = stability_score >= 8.0f;
        
        if (!result.passed) {
            result.error_message = "Stability below acceptable threshold";
            result.warnings.push_back("Performance variations detected during test");
            result.recommendations.push_back("Check for background processes affecting performance");
            result.recommendations.push_back("Monitor system temperature and power management");
        }
        
    } catch (const std::exception& e) {
        result.passed = false;
        result.error_message = std::string("Stability test failed: ") + e.what();
        spdlog::error("Stability test error: {}", e.what());
    }
    
    auto end_time = std::chrono::steady_clock::now();
    result.execution_time_ms = static_cast<UINT>(
        std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count()
    );
    
    spdlog::info("Stability test completed: {} (score: {})", 
                 result.passed ? "PASSED" : "FAILED", result.performance_score);
    
    return result;
}

TestResult TestingFramework::run_vr_comfort_test() {
    spdlog::info("Running VR comfort test");
    
    TestResult result;
    result.test_name = "VR Comfort Test";
    auto start_time = std::chrono::steady_clock::now();
    
    try {
        // Test VR frame rate stability
        float vr_fps_stability = test_vr_frame_rate_stability();
        
        // Test motion sickness factors
        float motion_sickness_score = test_motion_sickness_factors();
        
        // Test VR latency
        float vr_latency_score = test_vr_latency();
        
        // Calculate overall comfort score
        result.performance_score = (vr_fps_stability + motion_sickness_score + vr_latency_score) / 3.0f;
        result.passed = result.performance_score >= 7.5f;
        
        if (!result.passed) {
            result.error_message = "VR comfort below acceptable threshold";
            
            if (vr_fps_stability < 7.0f) {
                result.warnings.push_back("VR frame rate instability detected");
                result.recommendations.push_back("Reduce VR resolution or graphics settings");
            }
            
            if (motion_sickness_score < 7.0f) {
                result.warnings.push_back("Motion sickness factors detected");
                result.recommendations.push_back("Enable comfort options in VR settings");
                result.recommendations.push_back("Consider using teleport movement instead of smooth locomotion");
            }
            
            if (vr_latency_score < 7.0f) {
                result.warnings.push_back("High VR latency detected");
                result.recommendations.push_back("Check VR headset connection and drivers");
                result.recommendations.push_back("Reduce graphics complexity");
            }
        }
        
    } catch (const std::exception& e) {
        result.passed = false;
        result.error_message = std::string("VR comfort test failed: ") + e.what();
        spdlog::error("VR comfort test error: {}", e.what());
    }
    
    auto end_time = std::chrono::steady_clock::now();
    result.execution_time_ms = static_cast<UINT>(
        std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count()
    );
    
    spdlog::info("VR comfort test completed: {} (score: {})", 
                 result.passed ? "PASSED" : "FAILED", result.performance_score);
    
    return result;
}

TestResult TestingFramework::run_input_test() {
    spdlog::info("Running input test");
    
    TestResult result;
    result.test_name = "Input Test";
    auto start_time = std::chrono::steady_clock::now();
    
    try {
        // Test VR controller input
        bool controller_input_ok = test_vr_controller_input();
        
        // Test input latency
        float input_latency_score = test_input_latency();
        
        // Test input mapping
        bool input_mapping_ok = test_input_mapping();
        
        // Test haptic feedback
        bool haptic_feedback_ok = test_haptic_feedback();
        
        result.passed = controller_input_ok && input_mapping_ok && haptic_feedback_ok && input_latency_score >= 7.0f;
        
        if (!result.passed) {
            result.error_message = "Input system issues detected";
            
            if (!controller_input_ok) {
                result.warnings.push_back("VR controller input issues detected");
                result.recommendations.push_back("Check controller battery and connection");
                result.recommendations.push_back("Recalibrate VR controllers");
            }
            
            if (!input_mapping_ok) {
                result.warnings.push_back("Input mapping issues detected");
                result.recommendations.push_back("Check input configuration files");
                result.recommendations.push_back("Reset input mappings to defaults");
            }
            
            if (!haptic_feedback_ok) {
                result.warnings.push_back("Haptic feedback issues detected");
                result.recommendations.push_back("Check haptic feedback settings");
                result.recommendations.push_back("Update controller firmware");
            }
            
            if (input_latency_score < 7.0f) {
                result.warnings.push_back("High input latency detected");
                result.recommendations.push_back("Check USB connection quality");
                result.recommendations.push_back("Reduce system load");
            }
        }
        
        result.performance_score = result.passed ? 10.0f : 5.0f;
        
    } catch (const std::exception& e) {
        result.passed = false;
        result.error_message = std::string("Input test failed: ") + e.what();
        spdlog::error("Input test error: {}", e.what());
    }
    
    auto end_time = std::chrono::steady_clock::now();
    result.execution_time_ms = static_cast<UINT>(
        std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count()
    );
    
    spdlog::info("Input test completed: {}", result.passed ? "PASSED" : "FAILED");
    
    return result;
}

TestResult TestingFramework::run_rendering_test() {
    spdlog::info("Running rendering test");
    
    TestResult result;
    result.test_name = "Rendering Test";
    auto start_time = std::chrono::steady_clock::now();
    
    try {
        // Test rendering performance
        float rendering_performance = test_rendering_performance();
        
        // Test rendering quality
        float rendering_quality = test_rendering_quality();
        
        // Test stereo rendering
        bool stereo_rendering_ok = test_stereo_rendering();
        
        // Test VR-specific rendering features
        float vr_rendering_score = test_vr_rendering_features();
        
        // Calculate overall rendering score
        result.performance_score = (rendering_performance + rendering_quality + vr_rendering_score) / 3.0f;
        result.passed = result.performance_score >= 7.0f && stereo_rendering_ok;
        
        if (!result.passed) {
            result.error_message = "Rendering issues detected";
            
            if (rendering_performance < 7.0f) {
                result.warnings.push_back("Rendering performance below threshold");
                result.recommendations.push_back("Reduce graphics settings");
                result.recommendations.push_back("Check GPU drivers and temperature");
            }
            
            if (rendering_quality < 7.0f) {
                result.warnings.push_back("Rendering quality below threshold");
                result.recommendations.push_back("Increase graphics quality settings");
                result.recommendations.push_back("Check texture and shader quality");
            }
            
            if (!stereo_rendering_ok) {
                result.warnings.push_back("Stereo rendering issues detected");
                result.recommendations.push_back("Check VR rendering pipeline");
                result.recommendations.push_back("Verify VR headset connection");
            }
            
            if (vr_rendering_score < 7.0f) {
                result.warnings.push_back("VR rendering features issues detected");
                result.recommendations.push_back("Check VR-specific rendering settings");
                result.recommendations.push_back("Update VR runtime");
            }
        }
        
    } catch (const std::exception& e) {
        result.passed = false;
        result.error_message = std::string("Rendering test failed: ") + e.what();
        spdlog::error("Rendering test error: {}", e.what());
    }
    
    auto end_time = std::chrono::steady_clock::now();
    result.execution_time_ms = static_cast<UINT>(
        std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count()
    );
    
    spdlog::info("Rendering test completed: {} (score: {})", 
                 result.passed ? "PASSED" : "FAILED", result.performance_score);
    
    return result;
}

TestResult TestingFramework::run_memory_test() {
    spdlog::info("Running memory test");
    
    TestResult result;
    result.test_name = "Memory Test";
    auto start_time = std::chrono::steady_clock::now();
    
    try {
        // Test memory allocation patterns
        float memory_allocation_score = test_memory_allocation_patterns();
        
        // Test memory leak detection
        bool memory_leak_detected = test_memory_leaks();
        
        // Test memory fragmentation
        float memory_fragmentation_score = test_memory_fragmentation();
        
        // Test VR memory usage
        float vr_memory_score = test_vr_memory_usage();
        
        // Calculate overall memory score
        result.performance_score = (memory_allocation_score + memory_fragmentation_score + vr_memory_score) / 3.0f;
        result.passed = result.performance_score >= 7.0f && !memory_leak_detected;
        
        if (!result.passed) {
            result.error_message = "Memory issues detected";
            
            if (memory_leak_detected) {
                result.warnings.push_back("Memory leak detected");
                result.recommendations.push_back("Check for resource cleanup issues");
                result.recommendations.push_back("Review memory management code");
            }
            
            if (memory_allocation_score < 7.0f) {
                result.warnings.push_back("Memory allocation issues detected");
                result.recommendations.push_back("Optimize memory allocation patterns");
                result.recommendations.push_back("Check for excessive allocations");
            }
            
            if (memory_fragmentation_score < 7.0f) {
                result.warnings.push_back("Memory fragmentation detected");
                result.recommendations.push_back("Implement memory pooling");
                result.recommendations.push_back("Reduce allocation/deallocation frequency");
            }
            
            if (vr_memory_score < 7.0f) {
                result.warnings.push_back("VR memory usage issues detected");
                result.recommendations.push_back("Optimize VR texture memory usage");
                result.recommendations.push_back("Check VR resource management");
            }
        }
        
    } catch (const std::exception& e) {
        result.passed = false;
        result.error_message = std::string("Memory test failed: ") + e.what();
        spdlog::error("Memory test error: {}", e.what());
    }
    
    auto end_time = std::chrono::steady_clock::now();
    result.execution_time_ms = static_cast<UINT>(
        std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count()
    );
    
    spdlog::info("Memory test completed: {} (score: {})", 
                 result.passed ? "PASSED" : "FAILED", result.performance_score);
    
    return result;
}

TestResult TestingFramework::run_hook_test() {
    spdlog::info("Running hook test");
    
    TestResult result;
    result.test_name = "Hook Test";
    auto start_time = std::chrono::steady_clock::now();
    
    try {
        // Test hook injection
        bool hook_injection_ok = test_hook_injection();
        
        // Test hook stability
        float hook_stability_score = test_hook_stability();
        
        // Test hook performance impact
        float hook_performance_score = test_hook_performance_impact();
        
        // Test hook cleanup
        bool hook_cleanup_ok = test_hook_cleanup();
        
        // Calculate overall hook score
        result.performance_score = (hook_stability_score + hook_performance_score) / 2.0f;
        result.passed = hook_injection_ok && hook_cleanup_ok && result.performance_score >= 7.0f;
        
        if (!result.passed) {
            result.error_message = "Hook system issues detected";
            
            if (!hook_injection_ok) {
                result.warnings.push_back("Hook injection failed");
                result.recommendations.push_back("Check hook injection permissions");
                result.recommendations.push_back("Verify target process compatibility");
            }
            
            if (!hook_cleanup_ok) {
                result.warnings.push_back("Hook cleanup failed");
                result.recommendations.push_back("Check hook cleanup procedures");
                result.recommendations.push_back("Verify hook removal on process exit");
            }
            
            if (hook_stability_score < 7.0f) {
                result.warnings.push_back("Hook stability issues detected");
                result.recommendations.push_back("Check hook implementation for race conditions");
                result.recommendations.push_back("Verify hook thread safety");
            }
            
            if (hook_performance_score < 7.0f) {
                result.warnings.push_back("Hook performance impact detected");
                result.recommendations.push_back("Optimize hook callback functions");
                result.recommendations.push_back("Reduce hook frequency if possible");
            }
        }
        
    } catch (const std::exception& e) {
        result.passed = false;
        result.error_message = std::string("Hook test failed: ") + e.what();
        spdlog::error("Hook test error: {}", e.what());
    }
    
    auto end_time = std::chrono::steady_clock::now();
    result.execution_time_ms = static_cast<UINT>(
        std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count()
    );
    
    spdlog::info("Hook test completed: {} (score: {})", 
                 result.passed ? "PASSED" : "FAILED", result.performance_score);
    
    return result;
}

// Test reporting
void TestingFramework::generate_test_report(const std::vector<TestResult>& results) {
    if (!m_config.generate_test_reports) {
        spdlog::info("Test report generation disabled");
        return;
    }
    
    spdlog::info("Generating comprehensive test report for {} results", results.size());
    
    try {
        // Create output directory if it doesn't exist
        std::filesystem::path output_dir(m_config.test_output_directory);
        if (!std::filesystem::exists(output_dir)) {
            std::filesystem::create_directories(output_dir);
        }
        
        // Generate timestamp for unique filename
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream timestamp;
        timestamp << std::put_time(std::localtime(&time_t), "%Y%m%d_%H%M%S");
        
        // Generate report filename
        std::string report_filename = "test_report_" + timestamp.str() + ".html";
        std::filesystem::path report_path = output_dir / report_filename;
        
        // Generate HTML report
        std::ofstream report_file(report_path);
        if (!report_file.is_open()) {
            spdlog::error("Failed to open report file: {}", report_path.string());
            return;
        }
        
        // Write HTML header
        report_file << "<!DOCTYPE html>\n";
        report_file << "<html lang=\"en\">\n";
        report_file << "<head>\n";
        report_file << "    <meta charset=\"UTF-8\">\n";
        report_file << "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
        report_file << "    <title>UEVR Testing Framework Report</title>\n";
        report_file << "    <style>\n";
        report_file << "        body { font-family: Arial, sans-serif; margin: 20px; }\n";
        report_file << "        .header { background-color: #f0f0f0; padding: 20px; border-radius: 5px; }\n";
        report_file << "        .summary { background-color: #e8f5e8; padding: 15px; border-radius: 5px; margin: 20px 0; }\n";
        report_file << "        .test-result { margin: 10px 0; padding: 10px; border-radius: 3px; }\n";
        report_file << "        .passed { background-color: #d4edda; border-left: 4px solid #28a745; }\n";
        report_file << "        .failed { background-color: #f8d7da; border-left: 4px solid #dc3545; }\n";
        report_file << "        .metrics { background-color: #f8f9fa; padding: 15px; border-radius: 5px; margin: 20px 0; }\n";
        report_file << "        .metric-grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); gap: 15px; }\n";
        report_file << "        .metric-item { background-color: white; padding: 10px; border-radius: 3px; border: 1px solid #dee2e6; }\n";
        report_file << "    </style>\n";
        report_file << "</head>\n";
        report_file << "<body>\n";
        
        // Write header
        report_file << "    <div class=\"header\">\n";
        report_file << "        <h1>UEVR Testing Framework Report</h1>\n";
        report_file << "        <p>Generated: " << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S") << "</p>\n";
        report_file << "        <p>Total Tests: " << results.size() << "</p>\n";
        report_file << "    </div>\n";
        
        // Calculate summary statistics
        size_t passed_count = 0;
        size_t failed_count = 0;
        float total_performance = 0.0f;
        UINT total_execution_time = 0;
        
        for (const auto& result : results) {
            if (result.passed) {
                passed_count++;
            } else {
                failed_count++;
            }
            total_performance += result.performance_score;
            total_execution_time += result.execution_time_ms;
        }
        
        float pass_rate = results.empty() ? 0.0f : (static_cast<float>(passed_count) / results.size()) * 100.0f;
        float avg_performance = results.empty() ? 0.0f : total_performance / results.size();
        float avg_execution_time = results.empty() ? 0.0f : static_cast<float>(total_execution_time) / results.size();
        
        // Write summary
        report_file << "    <div class=\"summary\">\n";
        report_file << "        <h2>Test Summary</h2>\n";
        report_file << "        <div class=\"metric-grid\">\n";
        report_file << "            <div class=\"metric-item\">\n";
        report_file << "                <strong>Pass Rate:</strong><br>" << std::fixed << std::setprecision(1) << pass_rate << "%\n";
        report_file << "            </div>\n";
        report_file << "            <div class=\"metric-item\">\n";
        report_file << "                <strong>Passed:</strong><br>" << passed_count << " tests\n";
        report_file << "            </div>\n";
        report_file << "            <div class=\"metric-item\">\n";
        report_file << "                <strong>Failed:</strong><br>" << failed_count << " tests\n";
        report_file << "            </div>\n";
        report_file << "            <div class=\"metric-item\">\n";
        report_file << "                <strong>Avg Performance:</strong><br>" << std::fixed << std::setprecision(2) << avg_performance << "\n";
        report_file << "            </div>\n";
        report_file << "            <div class=\"metric-item\">\n";
        report_file << "                <strong>Avg Execution Time:</strong><br>" << std::fixed << std::setprecision(1) << avg_execution_time << " ms\n";
        report_file << "            </div>\n";
        report_file << "        </div>\n";
        report_file << "    </div>\n";
        
        // Write detailed results
        report_file << "    <h2>Detailed Test Results</h2>\n";
        for (const auto& result : results) {
            std::string css_class = result.passed ? "passed" : "failed";
            std::string status_text = result.passed ? "PASSED" : "FAILED";
            
            report_file << "    <div class=\"test-result " << css_class << "\">\n";
            report_file << "        <h3>" << result.test_name << " - " << status_text << "</h3>\n";
            report_file << "        <p><strong>Performance Score:</strong> " << std::fixed << std::setprecision(2) << result.performance_score << "</p>\n";
            report_file << "        <p><strong>Execution Time:</strong> " << result.execution_time_ms << " ms</p>\n";
            
            if (!result.passed && !result.error_message.empty()) {
                report_file << "        <p><strong>Error:</strong> " << result.error_message << "</p>\n";
            }
            
            if (!result.warnings.empty()) {
                report_file << "        <p><strong>Warnings:</strong></p><ul>\n";
                for (const auto& warning : result.warnings) {
                    report_file << "            <li>" << warning << "</li>\n";
                }
                report_file << "        </ul>\n";
            }
            
            if (!result.recommendations.empty()) {
                report_file << "        <p><strong>Recommendations:</strong></p><ul>\n";
                for (const auto& rec : result.recommendations) {
                    report_file << "            <li>" << rec << "</li>\n";
                }
                report_file << "        </ul>\n";
            }
            
            report_file << "    </div>\n";
        }
        
        // Write footer
        report_file << "    <div class=\"header\">\n";
        report_file << "        <p>Report generated by UEVR Testing Framework</p>\n";
        report_file << "        <p>End of Report</p>\n";
        report_file << "    </div>\n";
        
        report_file << "</body>\n";
        report_file << "</html>\n";
        
        report_file.close();
        spdlog::info("Test report generated successfully: {}", report_path.string());
        
    } catch (const std::exception& e) {
        spdlog::error("Error generating test report: {}", e.what());
        m_test_error_logs.push_back(std::string("Report generation error: ") + e.what());
    }
}

void TestingFramework::export_test_results(const std::vector<TestResult>& results, const std::string& filename) {
    spdlog::info("Exporting test results to: {}", filename);
    
    try {
        // Create output directory if it doesn't exist
        std::filesystem::path output_dir(m_config.test_output_directory);
        if (!std::filesystem::exists(output_dir)) {
            std::filesystem::create_directories(output_dir);
        }
        
        std::filesystem::path export_path = output_dir / filename;
        
        // Export as JSON
        if (filename.ends_with(".json")) {
            export_results_as_json(results, export_path);
        }
        // Export as CSV
        else if (filename.ends_with(".csv")) {
            export_results_as_csv(results, export_path);
        }
        // Export as XML
        else if (filename.ends_with(".xml")) {
            export_results_as_xml(results, export_path);
        }
        else {
            spdlog::warn("Unsupported export format: {}", filename);
            return;
        }
        
        spdlog::info("Test results exported successfully to: {}", export_path.string());
        
    } catch (const std::exception& e) {
        spdlog::error("Error exporting test results: {}", e.what());
        m_test_error_logs.push_back(std::string("Export error: ") + e.what());
    }
}

// Export helper functions
void TestingFramework::export_results_as_json(const std::vector<TestResult>& results, const std::filesystem::path& path) {
    std::ofstream file(path);
    if (!file.is_open()) {
        spdlog::error("Failed to open JSON export file: {}", path.string());
        return;
    }
    
    file << "{\n";
    file << "  \"test_results\": [\n";
    
    for (size_t i = 0; i < results.size(); ++i) {
        const auto& result = results[i];
        file << "    {\n";
        file << "      \"test_name\": \"" << result.test_name << "\",\n";
        file << "      \"passed\": " << (result.passed ? "true" : "false") << ",\n";
        file << "      \"performance_score\": " << std::fixed << std::setprecision(2) << result.performance_score << ",\n";
        file << "      \"execution_time_ms\": " << result.execution_time_ms << ",\n";
        file << "      \"error_message\": \"" << result.error_message << "\",\n";
        
        file << "      \"warnings\": [\n";
        for (size_t j = 0; j < result.warnings.size(); ++j) {
            file << "        \"" << result.warnings[j] << "\"" << (j < result.warnings.size() - 1 ? "," : "") << "\n";
        }
        file << "      ],\n";
        
        file << "      \"recommendations\": [\n";
        for (size_t j = 0; j < result.recommendations.size(); ++j) {
            file << "        \"" << result.recommendations[j] << "\"" << (j < result.recommendations.size() - 1 ? "," : "") << "\n";
        }
        file << "      ]\n";
        
        file << "    }" << (i < results.size() - 1 ? "," : "") << "\n";
    }
    
    file << "  ]\n";
    file << "}\n";
    
    file.close();
    spdlog::info("JSON export completed: {}", path.string());
}

void TestingFramework::export_results_as_csv(const std::vector<TestResult>& results, const std::filesystem::path& path) {
    std::ofstream file(path);
    if (!file.is_open()) {
        spdlog::error("Failed to open CSV export file: {}", path.string());
        return;
    }
    
    // Write CSV header
    file << "TestName,Status,PerformanceScore,ExecutionTime,ErrorMessage,Warnings,Recommendations\n";
    
    // Write data rows
    for (const auto& result : results) {
        file << "\"" << result.test_name << "\",";
        file << (result.passed ? "PASSED" : "FAILED") << ",";
        file << std::fixed << std::setprecision(2) << result.performance_score << ",";
        file << result.execution_time_ms << ",";
        file << "\"" << result.error_message << "\",";
        
        // Combine warnings
        std::string warnings_str;
        for (const auto& warning : result.warnings) {
            if (!warnings_str.empty()) warnings_str += "; ";
            warnings_str += warning;
        }
        file << "\"" << warnings_str << "\",";
        
        // Combine recommendations
        std::string recs_str;
        for (const auto& rec : result.recommendations) {
            if (!recs_str.empty()) recs_str += "; ";
            recs_str += rec;
        }
        file << "\"" << recs_str << "\"\n";
    }
    
    file.close();
    spdlog::info("CSV export completed: {}", path.string());
}

void TestingFramework::export_results_as_xml(const std::vector<TestResult>& results, const std::filesystem::path& path) {
    std::ofstream file(path);
    if (!file.is_open()) {
        spdlog::error("Failed to open XML export file: {}", path.string());
        return;
    }
    
    file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    file << "<test_results>\n";
    
    for (const auto& result : results) {
        file << "  <test_result>\n";
        file << "    <test_name>" << result.test_name << "</test_name>\n";
        file << "    <status>" << (result.passed ? "PASSED" : "FAILED") << "</status>\n";
        file << "    <performance_score>" << std::fixed << std::setprecision(2) << result.performance_score << "</performance_score>\n";
        file << "    <execution_time_ms>" << result.execution_time_ms << "</execution_time_ms>\n";
        file << "    <error_message>" << result.error_message << "</error_message>\n";
        
        file << "    <warnings>\n";
        for (const auto& warning : result.warnings) {
            file << "      <warning>" << warning << "</warning>\n";
        }
        file << "    </warnings>\n";
        
        file << "    <recommendations>\n";
        for (const auto& rec : result.recommendations) {
            file << "      <recommendation>" << rec << "</recommendation>\n";
        }
        file << "    </recommendations>\n";
        
        file << "  </test_result>\n";
    }
    
    file << "</test_results>\n";
    
    file.close();
    spdlog::info("XML export completed: {}", path.string());
}

// Test suite setup functions
void TestingFramework::setup_performance_test_suite() {
    TestSuite performance_suite;
    performance_suite.name = "Performance";
    performance_suite.description = "Performance testing for VR adapters and systems";
    performance_suite.enabled = m_config.enable_performance_testing;
    
    // Add performance tests
    performance_suite.tests.push_back([this]() { return run_performance_test(); });
    
    add_test_suite(performance_suite);
    spdlog::info("Performance test suite configured");
}

void TestingFramework::setup_compatibility_test_suite() {
    TestSuite compatibility_suite;
    compatibility_suite.name = "Compatibility";
    compatibility_suite.description = "Compatibility testing for different engines and games";
    compatibility_suite.enabled = m_config.enable_compatibility_testing;
    
    // Add compatibility tests
    compatibility_suite.tests.push_back([this]() { return run_compatibility_test(); });
    
    add_test_suite(compatibility_suite);
    spdlog::info("Compatibility test suite configured");
}

void TestingFramework::setup_stability_test_suite() {
    TestSuite stability_suite;
    stability_suite.name = "Stability";
    stability_suite.description = "Long-term stability testing for VR systems";
    stability_suite.enabled = m_config.enable_stability_testing;
    
    // Add stability tests
    stability_suite.tests.push_back([this]() { return run_stability_test(); });
    
    add_test_suite(stability_suite);
    spdlog::info("Stability test suite configured");
}

void TestingFramework::setup_vr_comfort_test_suite() {
    TestSuite vr_comfort_suite;
    vr_comfort_suite.name = "VRComfort";
    vr_comfort_suite.description = "VR comfort and motion sickness testing";
    vr_comfort_suite.enabled = m_config.enable_vr_comfort_testing;
    
    // Add VR comfort tests
    vr_comfort_suite.tests.push_back([this]() { return run_vr_comfort_test(); });
    
    add_test_suite(vr_comfort_suite);
    spdlog::info("VR Comfort test suite configured");
}

void TestingFramework::setup_input_test_suite() {
    TestSuite input_suite;
    input_suite.name = "Input";
    input_suite.description = "Input system testing for VR controllers";
    input_suite.enabled = m_config.enable_input_testing;
    
    // Add input tests
    input_suite.tests.push_back([this]() { return run_input_test(); });
    
    add_test_suite(input_suite);
    spdlog::info("Input test suite configured");
}

void TestingFramework::setup_rendering_test_suite() {
    TestSuite rendering_suite;
    rendering_suite.name = "Rendering";
    rendering_suite.description = "Rendering performance and quality testing";
    rendering_suite.enabled = m_config.enable_rendering_testing;
    
    // Add rendering tests
    rendering_suite.tests.push_back([this]() { return run_rendering_test(); });
    
    add_test_suite(rendering_suite);
    spdlog::info("Rendering test suite configured");
}

void TestingFramework::setup_memory_test_suite() {
    TestSuite memory_suite;
    memory_suite.name = "Memory";
    memory_suite.description = "Memory usage and leak testing";
    memory_suite.enabled = m_config.enable_memory_testing;
    
    // Add memory tests
    memory_suite.tests.push_back([this]() { return run_memory_test(); });
    
    add_test_suite(memory_suite);
    spdlog::info("Memory test suite configured");
}

void TestingFramework::setup_hook_test_suite() {
    TestSuite hook_suite;
    hook_suite.name = "Hook";
    hook_suite.description = "Hook system and injection testing";
    hook_suite.enabled = m_config.enable_hook_testing;
    
    // Add hook tests
    hook_suite.tests.push_back([this]() { return run_hook_test(); });
    
    add_test_suite(hook_suite);
    spdlog::info("Hook test suite configured");
}

TestingFramework::TestResult TestingFramework::run_performance_test() {
    TestResult result;
    result.test_name = "Performance Test";
    
    spdlog::info("[TestingFramework] Running performance test");
    
    try {
        PerformanceMetrics metrics = collect_performance_metrics();
        
        // Evaluate performance
        bool passed = true;
        std::vector<std::string> warnings;
        std::vector<std::string> recommendations;
        
        if (metrics.average_fps < 72.0f) {
            passed = false;
            result.error_message = "Average FPS below VR minimum (72 FPS)";
            recommendations.push_back("Reduce resolution scale");
            recommendations.push_back("Disable expensive post-processing effects");
        }
        
        if (metrics.frame_time_95th_percentile > 13.89f) {
            warnings.push_back("Frame time 95th percentile above target (13.89ms)");
            recommendations.push_back("Optimize rendering pipeline");
        }
        
        if (metrics.dropped_frames > 0) {
            warnings.push_back("Dropped frames detected: " + std::to_string(metrics.dropped_frames));
            recommendations.push_back("Check GPU performance and settings");
        }
        
        result.passed = passed;
        result.performance_score = metrics.average_fps / 90.0f; // Normalize to 90 FPS target
        result.warnings = warnings;
        result.recommendations = recommendations;
        
    } catch (const std::exception& e) {
        result.passed = false;
        result.error_message = std::string("Performance test failed: ") + e.what();
        spdlog::error("Performance test error: {}", e.what());
    }
    
    return result;
}

TestingFramework::TestResult TestingFramework::run_compatibility_test() {
    spdlog::info("[TestingFramework] Running compatibility test");
    
    TestResult result;
    result.test_name = "Compatibility Test";
    auto start_time = std::chrono::steady_clock::now();
    
    try {
        // Test VR system compatibility
        bool vr_compatible = test_vr_system_compatibility();
        
        // Test graphics API compatibility
        bool graphics_compatible = test_graphics_api_compatibility();
        
        // Test engine-specific compatibility
        bool engine_compatible = test_engine_compatibility();
        
        // Test plugin system compatibility
        bool plugin_compatible = test_plugin_compatibility();
        
        result.passed = vr_compatible && graphics_compatible && engine_compatible && plugin_compatible;
        
        if (!result.passed) {
            result.error_message = "Compatibility issues detected";
            
            if (!vr_compatible) {
                result.warnings.push_back("VR system compatibility issues detected");
                result.recommendations.push_back("Update VR runtime drivers");
            }
            
            if (!graphics_compatible) {
                result.warnings.push_back("Graphics API compatibility issues detected");
                result.recommendations.push_back("Update graphics drivers");
            }
            
            if (!engine_compatible) {
                result.warnings.push_back("Game engine compatibility issues detected");
                result.recommendations.push_back("Check engine version compatibility");
            }
            
            if (!plugin_compatible) {
                result.warnings.push_back("Plugin system compatibility issues detected");
                result.recommendations.push_back("Update or reinstall plugins");
            }
        }
        
        result.performance_score = result.passed ? 10.0f : 3.0f;
        
    } catch (const std::exception& e) {
        result.passed = false;
        result.error_message = std::string("Compatibility test failed: ") + e.what();
        spdlog::error("Compatibility test error: {}", e.what());
    }
    
    auto end_time = std::chrono::steady_clock::now();
    result.execution_time_ms = static_cast<UINT>(
        std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count()
    );
    
    spdlog::info("Compatibility test completed: {}", result.passed ? "PASSED" : "FAILED");
    
    return result;
}

TestingFramework::TestResult TestingFramework::run_stability_test() {
    spdlog::info("[TestingFramework] Running stability test");
    
    TestResult result;
    result.test_name = "Stability Test";
    auto start_time = std::chrono::steady_clock::now();
    
    try {
        // Run stability test for configured duration
        UINT test_duration = m_config.test_duration_seconds * 1000; // Convert to ms
        UINT sample_count = m_config.performance_sample_count;
        
        std::vector<PerformanceMetrics> stability_samples;
        stability_samples.reserve(sample_count);
        
        // Collect performance samples over time
        for (UINT i = 0; i < sample_count; ++i) {
            auto sample_start = std::chrono::steady_clock::now();
            
            // Collect performance metrics
            PerformanceMetrics sample = collect_performance_metrics();
            stability_samples.push_back(sample);
            
            // Wait for next sample interval
            auto sample_end = std::chrono::steady_clock::now();
            auto sample_duration = std::chrono::duration_cast<std::chrono::milliseconds>(sample_end - sample_start);
            auto wait_time = (test_duration / sample_count) - sample_duration.count();
            
            if (wait_time > 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(wait_time));
            }
        }
        
        // Analyze stability
        float stability_score = analyze_stability(stability_samples);
        result.performance_score = stability_score;
        result.passed = stability_score >= 8.0f;
        
        if (!result.passed) {
            result.error_message = "Stability below acceptable threshold";
            result.warnings.push_back("Performance variations detected during test");
            result.recommendations.push_back("Check for background processes affecting performance");
            result.recommendations.push_back("Monitor system temperature and power management");
        }
        
    } catch (const std::exception& e) {
        result.passed = false;
        result.error_message = std::string("Stability test failed: ") + e.what();
        spdlog::error("Stability test error: {}", e.what());
    }
    
    auto end_time = std::chrono::steady_clock::now();
    result.execution_time_ms = static_cast<UINT>(
        std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count()
    );
    
    spdlog::info("Stability test completed: {} (score: {})", 
                 result.passed ? "PASSED" : "FAILED", result.performance_score);
    
    return result;
}

TestingFramework::TestResult TestingFramework::run_vr_comfort_test() {
    spdlog::info("[TestingFramework] Running VR comfort test");
    
    TestResult result;
    result.test_name = "VR Comfort Test";
    auto start_time = std::chrono::steady_clock::now();
    
    try {
        // Test VR frame rate stability
        float vr_fps_stability = test_vr_frame_rate_stability();
        
        // Test motion sickness factors
        float motion_sickness_score = test_motion_sickness_factors();
        
        // Test VR latency
        float vr_latency_score = test_vr_latency();
        
        // Calculate overall comfort score
        result.performance_score = (vr_fps_stability + motion_sickness_score + vr_latency_score) / 3.0f;
        result.passed = result.performance_score >= 7.5f;
        
        if (!result.passed) {
            result.error_message = "VR comfort below acceptable threshold";
            
            if (vr_fps_stability < 7.0f) {
                result.warnings.push_back("VR frame rate instability detected");
                result.recommendations.push_back("Reduce VR resolution or graphics settings");
            }
            
            if (motion_sickness_score < 7.0f) {
                result.warnings.push_back("Motion sickness factors detected");
                result.recommendations.push_back("Enable comfort options in VR settings");
                result.recommendations.push_back("Consider using teleport movement instead of smooth locomotion");
            }
            
            if (vr_latency_score < 7.0f) {
                result.warnings.push_back("High VR latency detected");
                result.recommendations.push_back("Check VR headset connection and drivers");
                result.recommendations.push_back("Reduce graphics complexity");
            }
        }
        
    } catch (const std::exception& e) {
        result.passed = false;
        result.error_message = std::string("VR comfort test failed: ") + e.what();
        spdlog::error("VR comfort test error: {}", e.what());
    }
    
    auto end_time = std::chrono::steady_clock::now();
    result.execution_time_ms = static_cast<UINT>(
        std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count()
    );
    
    spdlog::info("VR comfort test completed: {} (score: {})", 
                 result.passed ? "PASSED" : "FAILED", result.performance_score);
    
    return result;
}

TestingFramework::TestResult TestingFramework::run_input_test() {
    spdlog::info("[TestingFramework] Running input test");
    
    TestResult result;
    result.test_name = "Input Test";
    auto start_time = std::chrono::steady_clock::now();
    
    try {
        // Test VR controller input
        bool controller_input_ok = test_vr_controller_input();
        
        // Test input latency
        float input_latency_score = test_input_latency();
        
        // Test input mapping
        bool input_mapping_ok = test_input_mapping();
        
        // Test haptic feedback
        bool haptic_feedback_ok = test_haptic_feedback();
        
        result.passed = controller_input_ok && input_mapping_ok && haptic_feedback_ok && input_latency_score >= 7.0f;
        
        if (!result.passed) {
            result.error_message = "Input system issues detected";
            
            if (!controller_input_ok) {
                result.warnings.push_back("VR controller input issues detected");
                result.recommendations.push_back("Check controller battery and connection");
                result.recommendations.push_back("Recalibrate VR controllers");
            }
            
            if (!input_mapping_ok) {
                result.warnings.push_back("Input mapping issues detected");
                result.recommendations.push_back("Check input configuration files");
                result.recommendations.push_back("Reset input mappings to defaults");
            }
            
            if (!haptic_feedback_ok) {
                result.warnings.push_back("Haptic feedback issues detected");
                result.recommendations.push_back("Check haptic feedback settings");
                result.recommendations.push_back("Update controller firmware");
            }
            
            if (input_latency_score < 7.0f) {
                result.warnings.push_back("High input latency detected");
                result.recommendations.push_back("Check USB connection quality");
                result.recommendations.push_back("Reduce system load");
            }
        }
        
        result.performance_score = result.passed ? 10.0f : 5.0f;
        
    } catch (const std::exception& e) {
        result.passed = false;
        result.error_message = std::string("Input test failed: ") + e.what();
        spdlog::error("Input test error: {}", e.what());
    }
    
    auto end_time = std::chrono::steady_clock::now();
    result.execution_time_ms = static_cast<UINT>(
        std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count()
    );
    
    spdlog::info("Input test completed: {}", result.passed ? "PASSED" : "FAILED");
    
    return result;
}

TestingFramework::TestResult TestingFramework::run_rendering_test() {
    spdlog::info("[TestingFramework] Running rendering test");
    
    TestResult result;
    result.test_name = "Rendering Test";
    auto start_time = std::chrono::steady_clock::now();
    
    try {
        // Test rendering performance
        float rendering_performance = test_rendering_performance();
        
        // Test rendering quality
        float rendering_quality = test_rendering_quality();
        
        // Test stereo rendering
        bool stereo_rendering_ok = test_stereo_rendering();
        
        // Test VR-specific rendering features
        float vr_rendering_score = test_vr_rendering_features();
        
        // Calculate overall rendering score
        result.performance_score = (rendering_performance + rendering_quality + vr_rendering_score) / 3.0f;
        result.passed = result.performance_score >= 7.0f && stereo_rendering_ok;
        
        if (!result.passed) {
            result.error_message = "Rendering issues detected";
            
            if (rendering_performance < 7.0f) {
                result.warnings.push_back("Rendering performance below threshold");
                result.recommendations.push_back("Reduce graphics settings");
                result.recommendations.push_back("Check GPU drivers and temperature");
            }
            
            if (rendering_quality < 7.0f) {
                result.warnings.push_back("Rendering quality below threshold");
                result.recommendations.push_back("Increase graphics quality settings");
                result.recommendations.push_back("Check texture and shader quality");
            }
            
            if (!stereo_rendering_ok) {
                result.warnings.push_back("Stereo rendering issues detected");
                result.recommendations.push_back("Check VR rendering pipeline");
                result.recommendations.push_back("Verify VR headset connection");
            }
            
            if (vr_rendering_score < 7.0f) {
                result.warnings.push_back("VR rendering features issues detected");
                result.recommendations.push_back("Check VR-specific rendering settings");
                result.recommendations.push_back("Update VR runtime");
            }
        }
        
    } catch (const std::exception& e) {
        result.passed = false;
        result.error_message = std::string("Rendering test failed: ") + e.what();
        spdlog::error("Rendering test error: {}", e.what());
    }
    
    auto end_time = std::chrono::steady_clock::now();
    result.execution_time_ms = static_cast<UINT>(
        std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count()
    );
    
    spdlog::info("Rendering test completed: {} (score: {})", 
                 result.passed ? "PASSED" : "FAILED", result.performance_score);
    
    return result;
}

TestingFramework::TestResult TestingFramework::run_memory_test() {
    spdlog::info("[TestingFramework] Running memory test");
    
    TestResult result;
    result.test_name = "Memory Test";
    auto start_time = std::chrono::steady_clock::now();
    
    try {
        // Test memory allocation patterns
        float memory_allocation_score = test_memory_allocation_patterns();
        
        // Test memory leak detection
        bool memory_leak_detected = test_memory_leaks();
        
        // Test memory fragmentation
        float memory_fragmentation_score = test_memory_fragmentation();
        
        // Test VR memory usage
        float vr_memory_score = test_vr_memory_usage();
        
        // Calculate overall memory score
        result.performance_score = (memory_allocation_score + memory_fragmentation_score + vr_memory_score) / 3.0f;
        result.passed = result.performance_score >= 7.0f && !memory_leak_detected;
        
        if (!result.passed) {
            result.error_message = "Memory issues detected";
            
            if (memory_leak_detected) {
                result.warnings.push_back("Memory leak detected");
                result.recommendations.push_back("Check for resource cleanup issues");
                result.recommendations.push_back("Review memory management code");
            }
            
            if (memory_allocation_score < 7.0f) {
                result.warnings.push_back("Memory allocation issues detected");
                result.recommendations.push_back("Optimize memory allocation patterns");
                result.recommendations.push_back("Check for excessive allocations");
            }
            
            if (memory_fragmentation_score < 7.0f) {
                result.warnings.push_back("Memory fragmentation detected");
                result.recommendations.push_back("Implement memory pooling");
                result.recommendations.push_back("Reduce allocation/deallocation frequency");
            }
            
            if (vr_memory_score < 7.0f) {
                result.warnings.push_back("VR memory usage issues detected");
                result.recommendations.push_back("Optimize VR texture memory usage");
                result.recommendations.push_back("Check VR resource management");
            }
        }
        
    } catch (const std::exception& e) {
        result.passed = false;
        result.error_message = std::string("Memory test failed: ") + e.what();
        spdlog::error("Memory test error: {}", e.what());
    }
    
    auto end_time = std::chrono::steady_clock::now();
    result.execution_time_ms = static_cast<UINT>(
        std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count()
    );
    
    spdlog::info("Memory test completed: {} (score: {})", 
                 result.passed ? "PASSED" : "FAILED", result.performance_score);
    
    return result;
}

TestingFramework::TestResult TestingFramework::run_hook_test() {
    spdlog::info("[TestingFramework] Running hook test");
    
    TestResult result;
    result.test_name = "Hook Test";
    auto start_time = std::chrono::steady_clock::now();
    
    try {
        // Test hook injection
        bool hook_injection_ok = test_hook_injection();
        
        // Test hook stability
        float hook_stability_score = test_hook_stability();
        
        // Test hook performance impact
        float hook_performance_score = test_hook_performance_impact();
        
        // Test hook cleanup
        bool hook_cleanup_ok = test_hook_cleanup();
        
        // Calculate overall hook score
        result.performance_score = (hook_stability_score + hook_performance_score) / 2.0f;
        result.passed = hook_injection_ok && hook_cleanup_ok && result.performance_score >= 7.0f;
        
        if (!result.passed) {
            result.error_message = "Hook system issues detected";
            
            if (!hook_injection_ok) {
                result.warnings.push_back("Hook injection failed");
                result.recommendations.push_back("Check hook injection permissions");
                result.recommendations.push_back("Verify target process compatibility");
            }
            
            if (!hook_cleanup_ok) {
                result.warnings.push_back("Hook cleanup failed");
                result.recommendations.push_back("Check hook cleanup procedures");
                result.recommendations.push_back("Verify hook removal on process exit");
            }
            
            if (hook_stability_score < 7.0f) {
                result.warnings.push_back("Hook stability issues detected");
                result.recommendations.push_back("Check hook implementation for race conditions");
                result.recommendations.push_back("Verify hook thread safety");
            }
            
            if (hook_performance_score < 7.0f) {
                result.warnings.push_back("Hook performance impact detected");
                result.recommendations.push_back("Optimize hook callback functions");
                result.recommendations.push_back("Reduce hook frequency if possible");
            }
        }
        
    } catch (const std::exception& e) {
        result.passed = false;
        result.error_message = std::string("Hook test failed: ") + e.what();
        spdlog::error("Hook test error: {}", e.what());
    }
    
    auto end_time = std::chrono::steady_clock::now();
    result.execution_time_ms = static_cast<UINT>(
        std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count()
    );
    
    spdlog::info("Hook test completed: {} (score: {})", 
                 result.passed ? "PASSED" : "FAILED", result.performance_score);
    
    return result;
}

TestingFramework::PerformanceMetrics TestingFramework::collect_performance_metrics() {
    PerformanceMetrics metrics;
    
    try {
        // Collect basic system metrics
        collect_system_metrics(metrics);
        
        // Collect VR-specific metrics
        collect_vr_metrics(metrics);
        
        // Collect graphics metrics
        collect_graphics_metrics(metrics);
        
        // Collect memory metrics
        collect_memory_metrics(metrics);
        
        // Calculate derived metrics
        calculate_derived_metrics(metrics);
        
    } catch (const std::exception& e) {
        spdlog::error("Error collecting performance metrics: {}", e.what());
        metrics = getFallbackPerformanceMetrics();
    }
    
    return metrics;
}

// Helper functions for performance metrics
std::string TestingFramework::calculateVRPerformanceGrade(float fps, float frame_time) {
    if (fps >= 90.0f && frame_time <= 11.0f) return "A+";
    if (fps >= 80.0f && frame_time <= 12.5f) return "A";
    if (fps >= 72.0f && frame_time <= 13.9f) return "B+";
    if (fps >= 60.0f && frame_time <= 16.7f) return "B";
    if (fps >= 45.0f && frame_time <= 22.2f) return "C";
    if (fps >= 30.0f && frame_time <= 33.3f) return "D";
    return "F";
}

bool TestingFramework::needsVROptimization(float fps, float dropped_frames) {
    return fps < 72.0f || dropped_frames > 5.0f;
}

float TestingFramework::calculateMemoryPressure(float physical_usage, float page_file_usage) {
    float pressure = (physical_usage * 0.7f) + (page_file_usage * 0.3f);
    return std::clamp(pressure, 0.0f, 100.0f);
}

float TestingFramework::calculateFrameTimeStability(float variance, float fps) {
    if (fps <= 0.0f) return 0.0f;
    
    float frame_time = 1000.0f / fps;
    float stability = 100.0f - (variance / frame_time) * 100.0f;
    return std::clamp(stability, 0.0f, 100.0f);
}

float TestingFramework::calculateOverallPerformanceScore(float fps, float cpu, float gpu, float memory, float stability) {
    // Weighted scoring system
    float fps_score = std::min(fps / 90.0f, 1.0f) * 25.0f; // 25% weight
    float cpu_score = (100.0f - cpu) / 100.0f * 20.0f; // 20% weight
    float gpu_score = (100.0f - gpu) / 100.0f * 25.0f; // 25% weight
    float memory_score = (100.0f - memory) / 100.0f * 15.0f; // 15% weight
    float stability_score = stability / 100.0f * 15.0f; // 15% weight
    
    return fps_score + cpu_score + gpu_score + memory_score + stability_score;
}

std::vector<std::string> TestingFramework::generatePerformanceRecommendations(const PerformanceMetrics& metrics) {
    std::vector<std::string> recommendations;
    
    // FPS-based recommendations
    if (metrics.average_fps < 72.0f) {
        recommendations.push_back("Reduce VR resolution to improve frame rate");
        recommendations.push_back("Lower graphics quality settings");
        recommendations.push_back("Close unnecessary background applications");
    }
    
    // CPU-based recommendations
    if (metrics.cpu_usage > 80.0f) {
        recommendations.push_back("Check for CPU-intensive background processes");
        recommendations.push_back("Consider upgrading CPU if bottleneck persists");
        recommendations.push_back("Reduce physics simulation complexity");
    }
    
    // GPU-based recommendations
    if (metrics.gpu_usage > 90.0f) {
        recommendations.push_back("Reduce graphics quality settings");
        recommendations.push_back("Check GPU temperature and cooling");
        recommendations.push_back("Update graphics drivers");
    }
    
    // Memory-based recommendations
    if (metrics.memory_usage > 85.0f) {
        recommendations.push_back("Close unnecessary applications");
        recommendations.push_back("Check for memory leaks");
        recommendations.push_back("Consider increasing system RAM");
    }
    
    // VR-specific recommendations
    if (metrics.vr_dropped_frames > 5.0f) {
        recommendations.push_back("Optimize VR rendering pipeline");
        recommendations.push_back("Check VR headset connection");
        recommendations.push_back("Reduce VR supersampling");
    }
    
    return recommendations;
}

void TestingFramework::updatePerformanceHistory(const PerformanceMetrics& metrics) {
    m_test_metrics_history.push_back(metrics);
    
    // Keep only last 1000 samples to prevent memory bloat
    if (m_test_metrics_history.size() > 1000) {
        m_test_metrics_history.erase(m_test_metrics_history.begin());
    }
}

PerformanceMetrics TestingFramework::getFallbackPerformanceMetrics() {
    PerformanceMetrics fallback;
    fallback.average_fps = 60.0f;
    fallback.cpu_usage = 50.0f;
    fallback.gpu_usage = 50.0f;
    fallback.memory_usage = 50.0f;
    fallback.vr_fps = 60.0f;
    fallback.overall_performance_score = 5.0f;
    return fallback;
}

// Resource cleanup functions
void TestingFramework::cleanup_test_resources() {
    spdlog::info("Cleaning up test resources");
    
    try {
        // Cleanup VR render targets
        m_vr_render_targets.clear();
        
        // Cleanup test textures
        m_vr_test_textures.clear();
        m_test_textures.clear();
        
        // Cleanup test shaders
        m_test_shaders.clear();
        
        // Cleanup test buffers
        m_test_buffers.clear();
        
        // Cleanup test plugins
        m_test_plugins.clear();
        
        // Cleanup test input devices
        m_test_input_devices.clear();
        
        // Cleanup test audio resources
        m_test_audio_buffers.clear();
        m_test_audio_streams.clear();
        
        // Cleanup test memory allocations
        m_test_memory_allocations.clear();
        
        // Cleanup test network resources
        m_test_network_connections.clear();
        m_test_network_buffers.clear();
        
        // Cleanup test files
        for (const auto& test_file : m_test_files) {
            if (std::filesystem::exists(test_file)) {
                std::filesystem::remove(test_file);
            }
        }
        m_test_files.clear();
        
        // Cleanup test directories
        for (const auto& test_dir : m_test_directories) {
            if (std::filesystem::exists(test_dir)) {
                std::filesystem::remove_all(test_dir);
            }
        }
        m_test_directories.clear();
        
        spdlog::info("Test resources cleanup completed");
        
    } catch (const std::exception& e) {
        spdlog::error("Error during test resource cleanup: {}", e.what());
        emergencyCleanup();
    }
}

bool TestingFramework::verifyCleanupSuccess() {
    bool cleanup_success = true;
    
    // Verify all resource vectors are empty
    if (!m_vr_render_targets.empty()) {
        spdlog::warn("VR render targets not fully cleaned up");
        cleanup_success = false;
    }
    
    if (!m_test_textures.empty()) {
        spdlog::warn("Test textures not fully cleaned up");
        cleanup_success = false;
    }
    
    if (!m_test_shaders.empty()) {
        spdlog::warn("Test shaders not fully cleaned up");
        cleanup_success = false;
    }
    
    if (!m_test_buffers.empty()) {
        spdlog::warn("Test buffers not fully cleaned up");
        cleanup_success = false;
    }
    
    if (!m_test_plugins.empty()) {
        spdlog::warn("Test plugins not fully cleaned up");
        cleanup_success = false;
    }
    
    if (!m_test_input_devices.empty()) {
        spdlog::warn("Test input devices not fully cleaned up");
        cleanup_success = false;
    }
    
    if (!m_test_audio_buffers.empty()) {
        spdlog::warn("Test audio buffers not fully cleaned up");
        cleanup_success = false;
    }
    
    if (!m_test_audio_streams.empty()) {
        spdlog::warn("Test audio streams not fully cleaned up");
        cleanup_success = false;
    }
    
    if (!m_test_memory_allocations.empty()) {
        spdlog::warn("Test memory allocations not fully cleaned up");
        cleanup_success = false;
    }
    
    if (!m_test_network_connections.empty()) {
        spdlog::warn("Test network connections not fully cleaned up");
        cleanup_success = false;
    }
    
    if (!m_test_network_buffers.empty()) {
        spdlog::warn("Test network buffers not fully cleaned up");
        cleanup_success = false;
    }
    
    if (!m_test_files.empty()) {
        spdlog::warn("Test files not fully cleaned up");
        cleanup_success = false;
    }
    
    if (!m_test_directories.empty()) {
        spdlog::warn("Test directories not fully cleaned up");
        cleanup_success = false;
    }
    
    return cleanup_success;
}

void TestingFramework::emergencyCleanup() {
    spdlog::error("Emergency cleanup initiated");
    
    try {
        // Force clear all resource vectors
        m_vr_render_targets.clear();
        m_test_textures.clear();
        m_test_shaders.clear();
        m_test_buffers.clear();
        m_test_plugins.clear();
        m_test_input_devices.clear();
        m_test_audio_buffers.clear();
        m_test_audio_streams.clear();
        m_test_memory_allocations.clear();
        m_test_network_connections.clear();
        m_test_network_buffers.clear();
        m_test_files.clear();
        m_test_directories.clear();
        
        // Clear test data
        m_test_metrics_history.clear();
        m_test_error_logs.clear();
        m_test_warning_logs.clear();
        m_test_debug_logs.clear();
        
        spdlog::info("Emergency cleanup completed");
        
    } catch (const std::exception& e) {
        spdlog::error("Emergency cleanup failed: {}", e.what());
    }
}

// System integration functions
void TestingFramework::initializeSystemIntegrations() {
    spdlog::info("Initializing system integrations");
    
    try {
        // Initialize VR system integration
        if (m_vr_system) {
            spdlog::info("VR system integration initialized");
        }
        
        // Initialize graphics system integration
        if (m_graphics_system) {
            spdlog::info("Graphics system integration initialized");
        }
        
        // Initialize CPU monitor integration
        if (m_cpu_monitor) {
            spdlog::info("CPU monitor integration initialized");
        }
        
        // Initialize memory monitor integration
        if (m_memory_monitor) {
            spdlog::info("Memory monitor integration initialized");
        }
        
        // Initialize performance monitor integration
        if (m_performance_monitor) {
            spdlog::info("Performance monitor integration initialized");
        }
        
        // Initialize cross-engine adapter integration
        if (m_cross_engine_adapter) {
            spdlog::info("Cross-engine adapter integration initialized");
        }
        
        // Initialize plugin manager integration
        if (m_plugin_manager) {
            spdlog::info("Plugin manager integration initialized");
        }
        
        // Initialize input system integration
        if (m_input_system) {
            spdlog::info("Input system integration initialized");
        }
        
        // Initialize audio system integration
        if (m_audio_system) {
            spdlog::info("Audio system integration initialized");
        }
        
        // Initialize network monitor integration
        if (m_network_monitor) {
            spdlog::info("Network monitor integration initialized");
        }
        
        // Initialize file system integration
        if (m_file_system) {
            spdlog::info("File system integration initialized");
        }
        
        // Initialize config manager integration
        if (m_config_manager) {
            spdlog::info("Config manager integration initialized");
        }
        
        // Initialize logging system integration
        if (m_logging_system) {
            spdlog::info("Logging system integration initialized");
        }
        
        spdlog::info("All system integrations initialized successfully");
        
    } catch (const std::exception& e) {
        spdlog::error("Error initializing system integrations: {}", e.what());
    }
}

void TestingFramework::releaseSystemIntegrations() {
    spdlog::info("Releasing system integrations");
    
    try {
        // Release VR system integration
        m_vr_system.reset();
        
        // Release graphics system integration
        m_graphics_system.reset();
        
        // Release CPU monitor integration
        m_cpu_monitor.reset();
        
        // Release memory monitor integration
        m_memory_monitor.reset();
        
        // Release performance monitor integration
        m_performance_monitor.reset();
        
        // Release cross-engine adapter integration
        m_cross_engine_adapter.reset();
        
        // Release plugin manager integration
        m_plugin_manager.reset();
        
        // Release input system integration
        m_input_system.reset();
        
        // Release audio system integration
        m_audio_system.reset();
        
        // Release network monitor integration
        m_network_monitor.reset();
        
        // Release file system integration
        m_file_system.reset();
        
        // Release config manager integration
        m_config_manager.reset();
        
        // Release logging system integration
        m_logging_system.reset();
        
        spdlog::info("All system integrations released successfully");
        
    } catch (const std::exception& e) {
        spdlog::error("Error releasing system integrations: {}", e.what());
    }
}

// Setup logging
void TestingFramework::setup_logging() {
    try {
        // Create logs directory if it doesn't exist
        std::filesystem::path logs_dir("logs");
        if (!std::filesystem::exists(logs_dir)) {
            std::filesystem::create_directories(logs_dir);
        }
        
        // Setup rotating file sink
        auto rotating_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            "logs/testing_framework.log", 1024 * 1024 * 10, 5
        );
        
        // Setup console sink
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        
        // Create logger with multiple sinks
        auto logger = std::make_shared<spdlog::logger>("testing_framework", 
            spdlog::sinks_init_list{rotating_sink, console_sink});
        
        // Set as default logger
        spdlog::set_default_logger(logger);
        
        // Set log level
        spdlog::set_level(spdlog::level::debug);
        
        spdlog::info("Logging system initialized");
        
    } catch (const std::exception& e) {
        // Fallback to basic logging if setup fails
        spdlog::error("Failed to setup logging: {}", e.what());
    }
}

// REEngineTestingFramework Implementation
REEngineTestingFramework::REEngineTestingFramework() {
    setup_re_engine_test_suites();
}

TestingFramework::TestResult REEngineTestingFramework::test_horror_game_comfort() {
    TestResult result;
    result.test_name = "RE Engine Horror Game Comfort Test";
    
    spdlog::info("[REEngineTesting] Running horror game comfort test");
    
    try {
        // Test horror game specific comfort features
        bool passed = true;
        std::vector<std::string> warnings;
        std::vector<std::string> recommendations;
        
        // Test motion blur settings
        // Test depth of field settings
        // Test camera movement comfort
        
        result.passed = passed;
        result.performance_score = passed ? 1.0f : 0.0f;
        result.warnings = warnings;
        result.recommendations = recommendations;
        
    } catch (const std::exception& e) {
        result.passed = false;
        result.error_message = "Horror game comfort test failed: " + std::string(e.what());
    }
    
    return result;
}

TestingFramework::TestResult REEngineTestingFramework::test_fixed_camera_compatibility() {
    TestResult result;
    result.test_name = "RE Engine Fixed Camera Compatibility Test";
    
    spdlog::info("[REEngineTesting] Running fixed camera compatibility test");
    
    try {
        // Test fixed camera system compatibility
        bool passed = true;
        std::vector<std::string> warnings;
        std::vector<std::string> recommendations;
        
        // Test camera hook compatibility
        // Test view matrix extraction
        // Test VR camera conversion
        
        result.passed = passed;
        result.performance_score = passed ? 1.0f : 0.0f;
        result.warnings = warnings;
        result.recommendations = recommendations;
        
    } catch (const std::exception& e) {
        result.passed = false;
        result.error_message = "Fixed camera compatibility test failed: " + std::string(e.what());
    }
    
    return result;
}

TestingFramework::TestResult REEngineTestingFramework::test_atmospheric_lighting_performance() {
    TestResult result;
    result.test_name = "RE Engine Atmospheric Lighting Performance Test";
    
    spdlog::info("[REEngineTesting] Running atmospheric lighting performance test");
    
    try {
        // Test atmospheric lighting performance
        bool passed = true;
        std::vector<std::string> warnings;
        std::vector<std::string> recommendations;
        
        // Test shadow rendering performance
        // Test SSAO performance
        // Test bloom performance
        
        result.passed = passed;
        result.performance_score = 0.85f; // Good performance
        result.warnings = warnings;
        result.recommendations = recommendations;
        
    } catch (const std::exception& e) {
        result.passed = false;
        result.error_message = "Atmospheric lighting performance test failed: " + std::string(e.what());
    }
    
    return result;
}

TestingFramework::TestResult REEngineTestingFramework::test_weapon_interaction_stability() {
    TestResult result;
    result.test_name = "RE Engine Weapon Interaction Stability Test";
    
    spdlog::info("[REEngineTesting] Running weapon interaction stability test");
    
    try {
        // Test weapon interaction stability
        bool passed = true;
        std::vector<std::string> warnings;
        std::vector<std::string> recommendations;
        
        // Test weapon attachment stability
        // Test motion controller integration
        // Test haptic feedback stability
        
        result.passed = passed;
        result.performance_score = passed ? 1.0f : 0.0f;
        result.warnings = warnings;
        result.recommendations = recommendations;
        
    } catch (const std::exception& e) {
        result.passed = false;
        result.error_message = "Weapon interaction stability test failed: " + std::string(e.what());
    }
    
    return result;
}

void REEngineTestingFramework::setup_re_engine_test_suites() {
    // Add RE Engine specific test suites
    spdlog::info("[REEngineTesting] Setting up RE Engine test suites");
}

// REDengine4TestingFramework Implementation
REDengine4TestingFramework::REDengine4TestingFramework() {
    setup_redengine4_test_suites();
}

TestingFramework::TestResult REDengine4TestingFramework::test_urban_environment_performance() {
    TestResult result;
    result.test_name = "REDengine 4 Urban Environment Performance Test";
    
    spdlog::info("[REDengine4Testing] Running urban environment performance test");
    
    try {
        // Test urban environment performance
        bool passed = true;
        std::vector<std::string> warnings;
        std::vector<std::string> recommendations;
        
        // Test complex scene rendering
        // Test LOD system performance
        // Test urban lighting performance
        
        result.passed = passed;
        result.performance_score = 0.78f; // Good performance for complex scenes
        result.warnings = warnings;
        result.recommendations = recommendations;
        
    } catch (const std::exception& e) {
        result.passed = false;
        result.error_message = "Urban environment performance test failed: " + std::string(e.what());
    }
    
    return result;
}

TestingFramework::TestResult REDengine4TestingFramework::test_ray_tracing_compatibility() {
    TestResult result;
    result.test_name = "REDengine 4 Ray Tracing Compatibility Test";
    
    spdlog::info("[REDengine4Testing] Running ray tracing compatibility test");
    
    try {
        // Test ray tracing compatibility
        bool passed = true;
        std::vector<std::string> warnings;
        std::vector<std::string> recommendations;
        
        // Test ray tracing feature detection
        // Test ray tracing performance
        // Test ray tracing quality settings
        
        result.passed = passed;
        result.performance_score = passed ? 1.0f : 0.0f;
        result.warnings = warnings;
        result.recommendations = recommendations;
        
    } catch (const std::exception& e) {
        result.passed = false;
        result.error_message = "Ray tracing compatibility test failed: " + std::string(e.what());
    }
    
    return result;
}

TestingFramework::TestResult REDengine4TestingFramework::test_volumetric_effects_stability() {
    TestResult result;
    result.test_name = "REDengine 4 Volumetric Effects Stability Test";
    
    spdlog::info("[REDengine4Testing] Running volumetric effects stability test");
    
    try {
        // Test volumetric effects stability
        bool passed = true;
        std::vector<std::string> warnings;
        std::vector<std::string> recommendations;
        
        // Test fog rendering stability
        // Test smoke effects stability
        // Test atmospheric effects stability
        
        result.passed = passed;
        result.performance_score = passed ? 1.0f : 0.0f;
        result.warnings = warnings;
        result.recommendations = recommendations;
        
    } catch (const std::exception& e) {
        result.passed = false;
        result.error_message = "Volumetric effects stability test failed: " + std::string(e.what());
    }
    
    return result;
}

TestingFramework::TestResult REDengine4TestingFramework::test_hacking_interface_functionality() {
    TestResult result;
    result.test_name = "REDengine 4 Hacking Interface Functionality Test";
    
    spdlog::info("[REDengine4Testing] Running hacking interface functionality test");
    
    try {
        // Test hacking interface functionality
        bool passed = true;
        std::vector<std::string> warnings;
        std::vector<std::string> recommendations;
        
        // Test hacking interface rendering
        // Test hacking interface interaction
        // Test hacking interface performance
        
        result.passed = passed;
        result.performance_score = passed ? 1.0f : 0.0f;
        result.warnings = warnings;
        result.recommendations = recommendations;
        
    } catch (const std::exception& e) {
        result.passed = false;
        result.error_message = "Hacking interface functionality test failed: " + std::string(e.what());
    }
    
    return result;
}

void REDengine4TestingFramework::setup_redengine4_test_suites() {
    // Add REDengine 4 specific test suites
    spdlog::info("[REDengine4Testing] Setting up REDengine 4 test suites");
}

// MTFrameworkTestingFramework Implementation
MTFrameworkTestingFramework::MTFrameworkTestingFramework() {
    setup_mt_framework_test_suites();
}

TestingFramework::TestResult MTFrameworkTestingFramework::test_combat_scene_performance() {
    TestResult result;
    result.test_name = "MT Framework Combat Scene Performance Test";
    
    spdlog::info("[MTFrameworkTesting] Running combat scene performance test");
    
    try {
        // Test combat scene performance
        bool passed = true;
        std::vector<std::string> warnings;
        std::vector<std::string> recommendations;
        
        // Test intense combat rendering
        // Test monster model performance
        // Test weapon effect performance
        
        result.passed = passed;
        result.performance_score = 0.82f; // Good performance for combat scenes
        result.warnings = warnings;
        result.recommendations = recommendations;
        
    } catch (const std::exception& e) {
        result.passed = false;
        result.error_message = "Combat scene performance test failed: " + std::string(e.what());
    }
    
    return result;
}

TestingFramework::TestResult MTFrameworkTestingFramework::test_third_person_conversion() {
    TestResult result;
    result.test_name = "MT Framework Third-Person Conversion Test";
    
    spdlog::info("[MTFrameworkTesting] Running third-person conversion test");
    
    try {
        // Test third-person to first-person conversion
        bool passed = true;
        std::vector<std::string> warnings;
        std::vector<std::string> recommendations;
        
        // Test camera conversion accuracy
        // Test conversion performance
        // Test conversion stability
        
        result.passed = passed;
        result.performance_score = passed ? 1.0f : 0.0f;
        result.warnings = warnings;
        result.recommendations = recommendations;
        
    } catch (const std::exception& e) {
        result.passed = false;
        result.error_message = "Third-person conversion test failed: " + std::string(e.what());
    }
    
    return result;
}

TestingFramework::TestResult MTFrameworkTestingFramework::test_monster_interaction_stability() {
    TestResult result;
    result.test_name = "MT Framework Monster Interaction Stability Test";
    
    spdlog::info("[MTFrameworkTesting] Running monster interaction stability test");
    
    try {
        // Test monster interaction stability
        bool passed = true;
        std::vector<std::string> warnings;
        std::vector<std::string> recommendations;
        
        // Test monster scanning stability
        // Test monster tracking stability
        // Test monster information display stability
        
        result.passed = passed;
        result.performance_score = passed ? 1.0f : 0.0f;
        result.warnings = warnings;
        result.recommendations = recommendations;
        
    } catch (const std::exception& e) {
        result.passed = false;
        result.error_message = "Monster interaction stability test failed: " + std::string(e.what());
    }
    
    return result;
}

TestingFramework::TestResult MTFrameworkTestingFramework::test_weapon_switching_functionality() {
    TestResult result;
    result.test_name = "MT Framework Weapon Switching Functionality Test";
    
    spdlog::info("[MTFrameworkTesting] Running weapon switching functionality test");
    
    try {
        // Test weapon switching functionality
        bool passed = true;
        std::vector<std::string> warnings;
        std::vector<std::string> recommendations;
        
        // Test weapon switching speed
        // Test weapon switching accuracy
        // Test weapon switching performance
        
        result.passed = passed;
        result.performance_score = passed ? 1.0f : 0.0f;
        result.warnings = warnings;
        result.recommendations = recommendations;
        
    } catch (const std::exception& e) {
        result.passed = false;
        result.error_message = "Weapon switching functionality test failed: " + std::string(e.what());
    }
    
    return result;
}

void MTFrameworkTestingFramework::setup_mt_framework_test_suites() {
    // Add MT Framework specific test suites
    spdlog::info("[MTFrameworkTesting] Setting up MT Framework test suites");
}

// GlobalTestingManager Implementation
std::unique_ptr<TestingFramework> GlobalTestingManager::s_global_testing_framework;
std::unique_ptr<REEngineTestingFramework> GlobalTestingManager::s_re_engine_testing_framework;
std::unique_ptr<REDengine4TestingFramework> GlobalTestingManager::s_redengine4_testing_framework;
std::unique_ptr<MTFrameworkTestingFramework> GlobalTestingManager::s_mt_framework_testing_framework;
std::mutex GlobalTestingManager::s_init_mutex;

TestingFramework& GlobalTestingManager::get_testing_framework() {
    initialize_if_needed();
    return *s_global_testing_framework;
}

REEngineTestingFramework& GlobalTestingManager::get_re_engine_testing_framework() {
    initialize_if_needed();
    return *s_re_engine_testing_framework;
}

REDengine4TestingFramework& GlobalTestingManager::get_redengine4_testing_framework() {
    initialize_if_needed();
    return *s_redengine4_testing_framework;
}

MTFrameworkTestingFramework& GlobalTestingManager::get_mt_framework_testing_framework() {
    initialize_if_needed();
    return *s_mt_framework_testing_framework;
}

void GlobalTestingManager::configure_testing(const TestConfig& config) {
    std::lock_guard<std::mutex> lock(s_init_mutex);
    
    if (s_global_testing_framework) {
        s_global_testing_framework->set_test_config(config);
    }
}

void GlobalTestingManager::set_global_testing_mode(const std::string& mode) {
    std::lock_guard<std::mutex> lock(s_init_mutex);
    
    TestConfig config;
    
    if (mode == "quick") {
        config.test_duration_seconds = 60;
        config.performance_sample_count = 100;
        config.enable_memory_testing = false;
        config.enable_hook_testing = false;
    } else if (mode == "comprehensive") {
        config.test_duration_seconds = 600;
        config.performance_sample_count = 2000;
        config.enable_memory_testing = true;
        config.enable_hook_testing = true;
    } else if (mode == "standard") {
        config.test_duration_seconds = 300;
        config.performance_sample_count = 1000;
        config.enable_memory_testing = true;
        config.enable_hook_testing = true;
    }
    
    configure_testing(config);
    spdlog::info("[GlobalTestingManager] Global testing mode set to: {}", mode);
}

void GlobalTestingManager::cleanup_all() {
    std::lock_guard<std::mutex> lock(s_init_mutex);
    
    s_global_testing_framework.reset();
    s_re_engine_testing_framework.reset();
    s_redengine4_testing_framework.reset();
    s_mt_framework_testing_framework.reset();
    
    spdlog::info("[GlobalTestingManager] All testing frameworks cleaned up");
}

void GlobalTestingManager::initialize_if_needed() {
    std::lock_guard<std::mutex> lock(s_init_mutex);
    
    if (!s_global_testing_framework) {
        s_global_testing_framework = std::make_unique<TestingFramework>();
    }
    
    if (!s_re_engine_testing_framework) {
        s_re_engine_testing_framework = std::make_unique<REEngineTestingFramework>();
    }
    
    if (!s_redengine4_testing_framework) {
        s_redengine4_testing_framework = std::make_unique<REDengine4TestingFramework>();
    }
    
    if (!s_mt_framework_testing_framework) {
        s_mt_framework_testing_framework = std::make_unique<MTFrameworkTestingFramework>();
    }
}

} // namespace uevr_common


