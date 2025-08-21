#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <thread>
#include <atomic>
#include <mutex>
#include <chrono>
#include <gtest/gtest.h>

// Core system includes
#include "../src/core/Framework.hpp"
#include "../src/vr/UniversalVRConverter.hpp"
#include "../src/vr/RedditCommunityIntegration.hpp"
#include "../src/vr/VRInjectionSystem.hpp"
#include "../src/vr/StereoscopicRenderer.hpp"

namespace uevr {
namespace test {

/**
 * @brief Comprehensive Test Framework for uevrLCX
 * 
 * Provides comprehensive testing capabilities:
 * - Unit tests for all components
 * - Integration tests for system interactions
 * - Performance tests for VR optimization
 * - Compatibility tests for game engines
 * - Automated test execution and reporting
 */
class TestFramework {
public:
    // Singleton pattern
    static TestFramework& getInstance();
    
    // Disable copy and assignment
    TestFramework(const TestFramework&) = delete;
    TestFramework& operator=(const TestFramework&) = delete;

    // Core test framework lifecycle
    bool initialize();
    void shutdown();
    bool isInitialized() const { return m_initialized.load(); }

    // Test configuration
    struct TestConfig {
        bool runUnitTests = true;
        bool runIntegrationTests = true;
        bool runPerformanceTests = true;
        bool runCompatibilityTests = true;
        bool runStressTests = false;
        bool runMemoryTests = true;
        bool runSecurityTests = true;
        
        int maxTestDuration = 300; // seconds
        int maxMemoryUsage = 1024; // MB
        bool stopOnFirstFailure = false;
        bool generateDetailedReports = true;
        std::string reportFormat = "html"; // html, json, xml
        std::string outputDirectory = "test_reports";
    };

    void setTestConfig(const TestConfig& config);
    TestConfig getTestConfig() const;

    // Test execution
    struct TestResult {
        std::string testName;
        std::string testCategory;
        bool passed;
        double executionTime;
        std::string errorMessage;
        std::string stackTrace;
        std::unordered_map<std::string, std::string> metrics;
        
        TestResult() : passed(false), executionTime(0.0) {}
    };

    std::vector<TestResult> runAllTests();
    std::vector<TestResult> runUnitTests();
    std::vector<TestResult> runIntegrationTests();
    std::vector<TestResult> runPerformanceTests();
    std::vector<TestResult> runCompatibilityTests();
    std::vector<TestResult> runStressTests();
    std::vector<TestResult> runMemoryTests();
    std::vector<TestResult> runSecurityTests();

    // Test categories
    bool runVRConverterTests();
    bool runRedditIntegrationTests();
    bool runInjectionSystemTests();
    bool runStereoscopicRendererTests();
    bool runEngineAdapterTests();
    bool runPerformanceOptimizationTests();
    bool runCommunitySolutionTests();

    // Performance testing
    struct PerformanceTest {
        std::string testName;
        double baselineTime;
        double currentTime;
        double improvement;
        bool meetsTarget;
        std::string target;
        
        PerformanceTest() : baselineTime(0.0), currentTime(0.0), improvement(0.0), meetsTarget(false) {}
    };

    std::vector<PerformanceTest> runPerformanceBenchmarks();
    bool validatePerformanceTargets();
    bool generatePerformanceReport();

    // Compatibility testing
    struct CompatibilityTest {
        std::string gameName;
        std::string engineType;
        bool isCompatible;
        std::string compatibilityLevel; // "full", "partial", "experimental", "unsupported"
        std::vector<std::string> issues;
        std::vector<std::string> solutions;
        
        CompatibilityTest() : isCompatible(false) {}
    };

    std::vector<CompatibilityTest> testGameCompatibility();
    bool validateEngineSupport();
    bool generateCompatibilityReport();

    // Memory testing
    struct MemoryTest {
        std::string testName;
        size_t initialMemory;
        size_t peakMemory;
        size_t finalMemory;
        size_t memoryLeak;
        bool hasLeak;
        
        MemoryTest() : initialMemory(0), peakMemory(0), finalMemory(0), memoryLeak(0), hasLeak(false) {}
    };

    std::vector<MemoryTest> runMemoryTests();
    bool detectMemoryLeaks();
    bool validateMemoryUsage();

    // Security testing
    struct SecurityTest {
        std::string testName;
        std::string vulnerability;
        bool isVulnerable;
        std::string severity; // "critical", "high", "medium", "low"
        std::string description;
        std::string mitigation;
        
        SecurityTest() : isVulnerable(false) {}
    };

    std::vector<SecurityTest> runSecurityTests();
    bool validateSecurityMeasures();
    bool generateSecurityReport();

    // Test utilities
    struct TestUtility {
        static bool createTestGame(const std::string& gameName, const std::string& engineType);
        static bool simulateVRHeadset();
        static bool simulateGameProcess();
        static bool generateTestData();
        static bool cleanupTestData();
    };

    // Mock objects
    struct MockVRSystem {
        bool initialize();
        void shutdown();
        bool isConnected() const;
        std::string getHeadsetType() const;
        bool simulateTracking();
    };

    struct MockGameProcess {
        bool start(const std::string& executable);
        void stop();
        bool isRunning() const;
        DWORD getProcessId() const;
        bool injectDLL(const std::string& dllPath);
    };

    // Test reporting
    struct TestReport {
        std::string reportId;
        std::chrono::system_clock::time_point timestamp;
        std::vector<TestResult> results;
        std::vector<PerformanceTest> performanceTests;
        std::vector<CompatibilityTest> compatibilityTests;
        std::vector<MemoryTest> memoryTests;
        std::vector<SecurityTest> securityTests;
        
        // Summary statistics
        int totalTests;
        int passedTests;
        int failedTests;
        int skippedTests;
        double overallSuccessRate;
        double averageExecutionTime;
        
        TestReport() : totalTests(0), passedTests(0), failedTests(0), skippedTests(0),
                      overallSuccessRate(0.0), averageExecutionTime(0.0) {}
    };

    TestReport generateTestReport();
    bool saveTestReport(const TestReport& report, const std::string& filePath);
    bool loadTestReport(const std::string& filePath, TestReport& report);
    std::string generateHTMLReport(const TestReport& report);
    std::string generateJSONReport(const TestReport& report);
    std::string generateXMLReport(const TestReport& report);

    // Continuous integration
    bool setupCITesting();
    bool runCITests();
    bool validateCIResults();
    bool sendCINotifications();

    // Test automation
    struct TestAutomation {
        bool scheduleTests(const std::string& schedule);
        bool runAutomatedTests();
        bool monitorTestExecution();
        bool handleTestFailures();
    };

    // Error handling
    struct TestError {
        std::string errorCode;
        std::string errorMessage;
        std::string testName;
        std::string timestamp;
        std::string stackTrace;
        
        TestError() {}
    };

    std::vector<TestError> getTestErrors() const;
    void clearTestErrors();
    bool hasTestErrors() const;

private:
    TestFramework();
    ~TestFramework();

    // Internal data structures
    std::atomic<bool> m_initialized{false};
    std::mutex m_mutex;
    std::condition_variable m_cv;
    
    // Configuration
    TestConfig m_testConfig;
    
    // Test results
    std::vector<TestResult> m_testResults;
    std::vector<PerformanceTest> m_performanceTests;
    std::vector<CompatibilityTest> m_compatibilityTests;
    std::vector<MemoryTest> m_memoryTests;
    std::vector<SecurityTest> m_securityTests;
    
    // Error handling
    std::vector<TestError> m_testErrors;
    mutable std::mutex m_errorsMutex;
    
    // Mock objects
    std::unique_ptr<MockVRSystem> m_mockVRSystem;
    std::unique_ptr<MockGameProcess> m_mockGameProcess;
    
    // Internal methods
    bool initializeTestEnvironment();
    bool initializeMockObjects();
    bool initializeReporting();
    
    // Test implementations
    bool runVRConverterUnitTests();
    bool runRedditIntegrationUnitTests();
    bool runInjectionSystemUnitTests();
    bool runStereoscopicRendererUnitTests();
    
    bool runVRConverterIntegrationTests();
    bool runRedditIntegrationIntegrationTests();
    bool runInjectionSystemIntegrationTests();
    bool runStereoscopicRendererIntegrationTests();
    
    bool runVRPerformanceTests();
    bool runRedditPerformanceTests();
    bool runInjectionPerformanceTests();
    bool runStereoscopicPerformanceTests();
    
    // Utility methods
    void logTestError(const std::string& errorCode, const std::string& errorMessage, 
                     const std::string& testName = "");
    double measureExecutionTime(std::function<void()> testFunction);
    size_t measureMemoryUsage();
    void cleanupTestEnvironment();
};

} // namespace test
} // namespace uevr
