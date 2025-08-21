#pragma once

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <atomic>
#include "../core/SystemState.hpp"

namespace UEVRLCX {

// ============================================================================
// Forward Declarations
// ============================================================================

class UnitTester;
class IntegrationTester;
class PerformanceTester;
class SecurityTester;
class CompatibilityTester;
class TestAutomation;

// ============================================================================
// Testing Data Structures
// ============================================================================

enum class TestType {
    UNIT_TEST,
    INTEGRATION_TEST,
    PERFORMANCE_TEST,
    SECURITY_TEST,
    COMPATIBILITY_TEST,
    STRESS_TEST,
    LOAD_TEST,
    CUSTOM_TEST
};

enum class TestStatus {
    PENDING,
    RUNNING,
    PASSED,
    FAILED,
    SKIPPED,
    ERROR,
    TIMEOUT
};

enum class TestPriority {
    LOW,
    MEDIUM,
    HIGH,
    CRITICAL
};

struct TestCase {
    std::string id;
    std::string name;
    std::string description;
    TestType type;
    TestPriority priority;
    std::function<bool()> testFunction;
    std::function<void()> setupFunction;
    std::function<void()> teardownFunction;
    TestStatus status;
    std::string errorMessage;
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point endTime;
    double executionTime;
    bool isAutomated;
    
    TestCase() : type(TestType::CUSTOM), priority(TestPriority::MEDIUM),
                  status(TestStatus::PENDING), executionTime(0.0), isAutomated(false) {
        startTime = std::chrono::steady_clock::now();
        endTime = std::chrono::steady_clock::now();
    }
    
    TestCase(const std::string& i, const std::string& n, const std::string& desc, TestType t)
        : id(i), name(n), description(desc), type(t), priority(TestPriority::MEDIUM),
          status(TestStatus::PENDING), executionTime(0.0), isAutomated(false) {
        startTime = std::chrono::steady_clock::now();
        endTime = std::chrono::steady_clock::now();
    }
    
    void StartExecution() {
        status = TestStatus::RUNNING;
        startTime = std::chrono::steady_clock::now();
    }
    
    void EndExecution(TestStatus result, const std::string& error = "") {
        status = result;
        errorMessage = error;
        endTime = std::chrono::steady_clock::now();
        executionTime = std::chrono::duration<double>(endTime - startTime).count();
    }
};

struct TestSuite {
    std::string name;
    std::string description;
    std::vector<TestCase> testCases;
    bool isActive;
    TestStatus overallStatus;
    std::chrono::steady_clock::time_point lastRun;
    
    TestSuite() : isActive(false), overallStatus(TestStatus::PENDING) {
        lastRun = std::chrono::steady_clock::now();
    }
    
    TestSuite(const std::string& n, const std::string& desc)
        : name(n), description(desc), isActive(false), overallStatus(TestStatus::PENDING) {
        lastRun = std::chrono::steady_clock::now();
    }
    
    void AddTestCase(const TestCase& testCase) {
        testCases.push_back(testCase);
    }
    
    void RemoveTestCase(const std::string& testCaseId) {
        for (auto it = testCases.begin(); it != testCases.end(); ++it) {
            if (it->id == testCaseId) {
                testCases.erase(it);
                break;
            }
        }
    }
    
    void UpdateOverallStatus() {
        if (testCases.empty()) {
            overallStatus = TestStatus::PENDING;
            return;
        }
        
        bool hasFailed = false;
        bool hasRunning = false;
        bool hasPending = false;
        
        for (const auto& testCase : testCases) {
            if (testCase.status == TestStatus::FAILED || testCase.status == TestStatus::ERROR) {
                hasFailed = true;
            } else if (testCase.status == TestStatus::RUNNING) {
                hasRunning = true;
            } else if (testCase.status == TestStatus::PENDING) {
                hasPending = true;
            }
        }
        
        if (hasFailed) {
            overallStatus = TestStatus::FAILED;
        } else if (hasRunning) {
            overallStatus = TestStatus::RUNNING;
        } else if (hasPending) {
            overallStatus = TestStatus::PENDING;
        } else {
            overallStatus = TestStatus::PASSED;
        }
    }
};

struct TestResult {
    std::string testCaseId;
    std::string testCaseName;
    TestStatus status;
    std::string errorMessage;
    double executionTime;
    std::chrono::steady_clock::time_point timestamp;
    std::unordered_map<std::string, std::string> metadata;
    
    TestResult() : status(TestStatus::PENDING), executionTime(0.0) {
        timestamp = std::chrono::steady_clock::now();
    }
    
    TestResult(const std::string& id, const std::string& name, TestStatus s, double time)
        : testCaseId(id), testCaseName(name), status(s), executionTime(time) {
        timestamp = std::chrono::steady_clock::now();
    }
};

// ============================================================================
// Testing Framework Interface
// ============================================================================

class ITestingFramework {
public:
    virtual ~ITestingFramework() = default;
    
    // ========================================================================
    // Initialization and Management
    // ========================================================================
    
    virtual bool Initialize() = 0;
    virtual void Shutdown() = 0;
    virtual bool IsInitialized() const = 0;
    
    // ========================================================================
    // Test Case Management
    // ========================================================================
    
    virtual bool AddTestCase(const TestCase& testCase) = 0;
    virtual bool RemoveTestCase(const std::string& testCaseId) = 0;
    virtual bool UpdateTestCase(const TestCase& testCase) = 0;
    virtual TestCase* GetTestCase(const std::string& testCaseId) = 0;
    virtual std::vector<TestCase> GetAllTestCases() const = 0;
    
    // ========================================================================
    // Test Suite Management
    // ========================================================================
    
    virtual bool CreateTestSuite(const std::string& name, const std::string& description) = 0;
    virtual bool DeleteTestSuite(const std::string& name) = 0;
    virtual bool AddTestCaseToSuite(const std::string& suiteName, const std::string& testCaseId) = 0;
    virtual bool RemoveTestCaseFromSuite(const std::string& suiteName, const std::string& testCaseId) = 0;
    virtual TestSuite* GetTestSuite(const std::string& name) = 0;
    virtual std::vector<std::string> GetAvailableTestSuites() const = 0;
    
    // ========================================================================
    // Test Execution
    // ========================================================================
    
    virtual bool RunTestCase(const std::string& testCaseId) = 0;
    virtual bool RunTestSuite(const std::string& suiteName) = 0;
    virtual bool RunAllTests() = 0;
    virtual bool StopTestExecution() = 0;
    virtual bool IsTestRunning() const = 0;
    
    // ========================================================================
    // Test Automation
    // ========================================================================
    
    virtual bool EnableTestAutomation(bool enable) = 0;
    virtual bool IsTestAutomationEnabled() const = 0;
    virtual bool ScheduleAutomatedTest(const std::string& testCaseId, int intervalMinutes) = 0;
    virtual bool CancelAutomatedTest(const std::string& testCaseId) = 0;
    virtual std::vector<std::string> GetScheduledTests() const = 0;
    
    // ========================================================================
    // Performance Testing
    // ========================================================================
    
    virtual bool EnablePerformanceTesting(bool enable) = 0;
    virtual bool IsPerformanceTestingEnabled() const = 0;
    virtual bool RunPerformanceTest(const std::string& testCaseId, int iterations) = 0;
    virtual double GetPerformanceTestResult(const std::string& testCaseId) const = 0;
    
    // ========================================================================
    // Security Testing
    // ========================================================================
    
    virtual bool EnableSecurityTesting(bool enable) = 0;
    virtual bool IsSecurityTestingEnabled() const = 0;
    virtual bool RunSecurityTest(const std::string& testCaseId) = 0;
    virtual std::string GetSecurityTestResult(const std::string& testCaseId) const = 0;
    
    // ========================================================================
    // Compatibility Testing
    // ========================================================================
    
    virtual bool EnableCompatibilityTesting(bool enable) = 0;
    virtual bool IsCompatibilityTestingEnabled() const = 0;
    virtual bool RunCompatibilityTest(const std::string& testCaseId, const std::string& platform) = 0;
    virtual std::string GetCompatibilityTestResult(const std::string& testCaseId) const = 0;
    
    // ========================================================================
    // Test Results and Reporting
    // ========================================================================
    
    virtual std::vector<TestResult> GetTestResults() const = 0;
    virtual TestResult* GetTestResult(const std::string& testCaseId) = 0;
    virtual bool ExportTestResults(const std::string& filePath) = 0;
    virtual std::string GenerateTestReport() const = 0;
    virtual double GetTestSuccessRate() const = 0;
    
    // ========================================================================
    // Configuration and Settings
    // ========================================================================
    
    virtual bool SetTestTimeout(int milliseconds) = 0;
    virtual int GetTestTimeout() const = 0;
    virtual bool EnableParallelTesting(bool enable) = 0;
    virtual bool IsParallelTestingEnabled() const = 0;
    virtual bool SetMaxConcurrentTests(int maxTests) = 0;
    virtual int GetMaxConcurrentTests() const = 0;
    
    // ========================================================================
    // Monitoring and Statistics
    // ========================================================================
    
    virtual int GetTotalTests() const = 0;
    virtual int GetPassedTests() const = 0;
    virtual int GetFailedTests() const = 0;
    virtual int GetRunningTests() const = 0;
    virtual std::string GetTestingStatus() const = 0;
    virtual std::chrono::steady_clock::time_point GetLastTestRun() const = 0;
};

// ============================================================================
// Testing Framework Implementation
// ============================================================================

class TestingFramework : public ITestingFramework {
private:
    // Core Components
    std::unique_ptr<UnitTester> m_unitTester;
    std::unique_ptr<IntegrationTester> m_integrationTester;
    std::unique_ptr<PerformanceTester> m_performanceTester;
    std::unique_ptr<SecurityTester> m_securityTester;
    std::unique_ptr<CompatibilityTester> m_compatibilityTester;
    std::unique_ptr<TestAutomation> m_testAutomation;
    
    // Test Management
    std::unordered_map<std::string, TestCase> m_testCases;
    std::unordered_map<std::string, TestSuite> m_testSuites;
    std::vector<TestResult> m_testResults;
    
    // Configuration
    int m_testTimeout;
    bool m_parallelTestingEnabled;
    int m_maxConcurrentTests;
    bool m_testAutomationEnabled;
    bool m_performanceTestingEnabled;
    bool m_securityTestingEnabled;
    bool m_compatibilityTestingEnabled;
    bool m_isInitialized;
    
    // Execution State
    std::atomic<bool> m_testRunning;
    std::atomic<int> m_runningTests;
    std::chrono::steady_clock::time_point m_lastTestRun;
    
    // Statistics
    int m_totalTests;
    int m_passedTests;
    int m_failedTests;
    double m_testSuccessRate;
    
public:
    TestingFramework();
    ~TestingFramework();
    
    // ========================================================================
    // ITestingFramework Implementation
    // ========================================================================
    
    bool Initialize() override;
    void Shutdown() override;
    bool IsInitialized() const override { return m_isInitialized; }
    
    bool AddTestCase(const TestCase& testCase) override;
    bool RemoveTestCase(const std::string& testCaseId) override;
    bool UpdateTestCase(const TestCase& testCase) override;
    TestCase* GetTestCase(const std::string& testCaseId) override;
    std::vector<TestCase> GetAllTestCases() const override;
    
    bool CreateTestSuite(const std::string& name, const std::string& description) override;
    bool DeleteTestSuite(const std::string& name) override;
    bool AddTestCaseToSuite(const std::string& suiteName, const std::string& testCaseId) override;
    bool RemoveTestCaseFromSuite(const std::string& suiteName, const std::string& testCaseId) override;
    TestSuite* GetTestSuite(const std::string& name) override;
    std::vector<std::string> GetAvailableTestSuites() const override;
    
    bool RunTestCase(const std::string& testCaseId) override;
    bool RunTestSuite(const std::string& suiteName) override;
    bool RunAllTests() override;
    bool StopTestExecution() override;
    bool IsTestRunning() const override { return m_testRunning.load(); }
    
    bool EnableTestAutomation(bool enable) override;
    bool IsTestAutomationEnabled() const override { return m_testAutomationEnabled; }
    bool ScheduleAutomatedTest(const std::string& testCaseId, int intervalMinutes) override;
    bool CancelAutomatedTest(const std::string& testCaseId) override;
    std::vector<std::string> GetScheduledTests() const override;
    
    bool EnablePerformanceTesting(bool enable) override;
    bool IsPerformanceTestingEnabled() const override { return m_performanceTestingEnabled; }
    bool RunPerformanceTest(const std::string& testCaseId, int iterations) override;
    double GetPerformanceTestResult(const std::string& testCaseId) const override;
    
    bool EnableSecurityTesting(bool enable) override;
    bool IsSecurityTestingEnabled() const override { return m_securityTestingEnabled; }
    bool RunSecurityTest(const std::string& testCaseId) override;
    std::string GetSecurityTestResult(const std::string& testCaseId) const override;
    
    bool EnableCompatibilityTesting(bool enable) override;
    bool IsCompatibilityTestingEnabled() const override { return m_compatibilityTestingEnabled; }
    bool RunCompatibilityTest(const std::string& testCaseId, const std::string& platform) override;
    std::string GetCompatibilityTestResult(const std::string& testCaseId) const override;
    
    std::vector<TestResult> GetTestResults() const override { return m_testResults; }
    TestResult* GetTestResult(const std::string& testCaseId) override;
    bool ExportTestResults(const std::string& filePath) override;
    std::string GenerateTestReport() const override;
    double GetTestSuccessRate() const override { return m_testSuccessRate; }
    
    bool SetTestTimeout(int milliseconds) override;
    int GetTestTimeout() const override { return m_testTimeout; }
    bool EnableParallelTesting(bool enable) override;
    bool IsParallelTestingEnabled() const override { return m_parallelTestingEnabled; }
    bool SetMaxConcurrentTests(int maxTests) override;
    int GetMaxConcurrentTests() const override { return m_maxConcurrentTests; }
    
    int GetTotalTests() const override { return m_totalTests; }
    int GetPassedTests() const override { return m_passedTests; }
    int GetFailedTests() const override { return m_failedTests; }
    int GetRunningTests() const override { return m_runningTests.load(); }
    std::string GetTestingStatus() const override;
    std::chrono::steady_clock::time_point GetLastTestRun() const override { return m_lastTestRun; }
    
private:
    // ========================================================================
    // Private Helper Methods
    // ========================================================================
    
    bool InitializeUnitTester();
    bool InitializeIntegrationTester();
    bool InitializePerformanceTester();
    bool InitializeSecurityTester();
    bool InitializeCompatibilityTester();
    bool InitializeTestAutomation();
    
    bool ValidateTestCase(const TestCase& testCase);
    bool ValidateTestSuite(const std::string& suiteName);
    bool ExecuteTestCaseInternal(TestCase& testCase);
    bool ExecuteTestSuiteInternal(TestSuite& testSuite);
    
    void UpdateTestStatistics();
    void CalculateTestSuccessRate();
    void LogTestExecution(const TestCase& testCase, TestStatus status);
    void LogTestResult(const TestResult& result);
    
    bool IsTestCaseValid(const std::string& testCaseId);
    bool IsTestSuiteValid(const std::string& suiteName);
    void HandleTestError(const std::string& testCaseId, const std::string& error);
    
    void CleanupTestResults();
    void CleanupOldTestCases();
    void CleanupOldTestSuites();
    
    bool ProcessPerformanceTest(const std::string& testCaseId, int iterations);
    bool ProcessSecurityTest(const std::string& testCaseId);
    bool ProcessCompatibilityTest(const std::string& testCaseId, const std::string& platform);
    
    std::string GenerateTestCaseReport(const TestCase& testCase) const;
    std::string GenerateTestSuiteReport(const TestSuite& testSuite) const;
    std::string GenerateOverallTestReport() const;
    
    void UpdateTestExecutionState(bool isRunning);
    void IncrementRunningTests();
    void DecrementRunningTests();
};

// ============================================================================
// Factory Functions
// ============================================================================

std::unique_ptr<ITestingFramework> CreateTestingFramework();
std::unique_ptr<ITestingFramework> CreateTestingFrameworkWithConfig(const std::string& configPath);

} // namespace UEVRLCX
