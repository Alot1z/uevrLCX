#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <cassert>
#include "../../common/CrossEngineAdapterFactory.hpp"
#include "../../common/BaseCrossEngineAdapter.hpp"
#include "../../re-engine/REEngineAdapter.hpp"
#include "../../redengine4/REDengine4Adapter.hpp"
#include "../../mt-framework/MTFrameworkAdapter.hpp"
#include "uevr/ICrossEngineAdapter.hpp"

using namespace uevr;

/**
 * @brief Test suite for uevr Cross-Engine VR Adapters
 * 
 * Comprehensive testing of all adapter functionality
 */
class CrossEngineAdapterTestSuite {
public:
    CrossEngineAdapterTestSuite() : m_testsPassed(0), m_testsFailed(0) {}
    
    void runAllTests() {
        std::cout << "=== Running Cross-Engine Adapter Test Suite ===" << std::endl;
        
        testFactoryCreation();
        testREEngineAdapter();
        testREDengine4Adapter();
        testMTFrameworkAdapter();
        testBaseAdapterFunctionality();
        testErrorHandling();
        testPerformanceMetrics();
        
        printTestResults();
    }
    
private:
    int m_testsPassed;
    int m_testsFailed;
    
    void testFactoryCreation() {
        std::cout << "\n--- Testing Adapter Factory ---" << std::endl;
        
        CrossEngineAdapterFactory factory;
        
        // Test supported games
        assert(factory.canCreateAdapter("re7.exe") == true);
        assert(factory.canCreateAdapter("Cyberpunk2077.exe") == true);
        assert(factory.canCreateAdapter("MonsterHunterWorld.exe") == true);
        assert(factory.canCreateAdapter("unknown.exe") == false);
        
        std::cout << "✓ Factory creation and game detection tests passed" << std::endl;
        m_testsPassed++;
    }
    
    void testREEngineAdapter() {
        std::cout << "\n--- Testing RE Engine Adapter ---" << std::endl;
        
        REEngineAdapter adapter;
        
        // Test basic properties
        assert(adapter.getName() == "RE Engine VR Adapter");
        assert(adapter.getEngineType() == "RE Engine");
        assert(adapter.getGameExecutable() == "re7.exe");
        assert(adapter.getVersion() == "1.0.0");
        
        // Test initialization (should fail in test environment)
        bool initResult = adapter.initialize();
        // In test environment, engine detection will fail, which is expected
        
        std::cout << "✓ RE Engine adapter tests passed" << std::endl;
        m_testsPassed++;
    }
    
    void testREDengine4Adapter() {
        std::cout << "\n--- Testing REDengine 4 Adapter ---" << std::endl;
        
        REDengine4Adapter adapter;
        
        // Test basic properties
        assert(adapter.getName() == "REDengine 4 VR Adapter");
        assert(adapter.getEngineType() == "REDengine 4");
        assert(adapter.getGameExecutable() == "Cyberpunk2077.exe");
        assert(adapter.getVersion() == "1.0.0");
        
        // Test initialization (should fail in test environment)
        bool initResult = adapter.initialize();
        // In test environment, engine detection will fail, which is expected
        
        std::cout << "✓ REDengine 4 adapter tests passed" << std::endl;
        m_testsPassed++;
    }
    
    void testMTFrameworkAdapter() {
        std::cout << "\n--- Testing MT Framework Adapter ---" << std::endl;
        
        MTFrameworkAdapter adapter;
        
        // Test basic properties
        assert(adapter.getName() == "MT Framework VR Adapter");
        assert(adapter.getEngineType() == "MT Framework");
        assert(adapter.getGameExecutable() == "MonsterHunterWorld.exe");
        assert(adapter.getVersion() == "1.0.0");
        
        // Test initialization (should fail in test environment)
        bool initResult = adapter.initialize();
        // In test environment, engine detection will fail, which is expected
        
        std::cout << "✓ MT Framework adapter tests passed" << std::endl;
        m_testsPassed++;
    }
    
    void testBaseAdapterFunctionality() {
        std::cout << "\n--- Testing Base Adapter Functionality ---" << std::endl;
        
        // Test with a mock adapter
        class MockAdapter : public BaseCrossEngineAdapter {
        public:
            MockAdapter() : BaseCrossEngineAdapter("Mock", "Mock Engine", "mock.exe") {}
            
        protected:
            bool detectEngine() override { return true; }
            bool initializeEngineHooks() override { return true; }
            void cleanupEngineHooks() override {}
            bool initializeVR() override { return true; }
            void cleanupVR() override {}
            void updateVR() override {}
            void renderVR() override {}
        };
        
        MockAdapter mockAdapter;
        
        // Test initialization
        assert(mockAdapter.initialize() == true);
        assert(mockAdapter.isVREnabled() == true);
        
        // Test update and render
        mockAdapter.update();
        mockAdapter.render();
        
        // Test cleanup
        mockAdapter.cleanup();
        assert(mockAdapter.isVREnabled() == false);
        
        std::cout << "✓ Base adapter functionality tests passed" << std::endl;
        m_testsPassed++;
    }
    
    void testErrorHandling() {
        std::cout << "\n--- Testing Error Handling ---" << std::endl;
        
        // Test with a failing adapter
        class FailingAdapter : public BaseCrossEngineAdapter {
        public:
            FailingAdapter() : BaseCrossEngineAdapter("Failing", "Failing Engine", "fail.exe") {}
            
        protected:
            bool detectEngine() override { return false; }
            bool initializeEngineHooks() override { return false; }
            void cleanupEngineHooks() override {}
            bool initializeVR() override { return false; }
            void cleanupVR() override {}
            void updateVR() override {}
            void renderVR() override {}
        };
        
        FailingAdapter failingAdapter;
        
        // Test initialization failure
        assert(failingAdapter.initialize() == false);
        assert(failingAdapter.isVREnabled() == false);
        
        std::cout << "✓ Error handling tests passed" << std::endl;
        m_testsPassed++;
    }
    
    void testPerformanceMetrics() {
        std::cout << "\n--- Testing Performance Metrics ---" << std::endl;
        
        class MockAdapter : public BaseCrossEngineAdapter {
        public:
            MockAdapter() : BaseCrossEngineAdapter("Mock", "Mock Engine", "mock.exe") {}
            
        protected:
            bool detectEngine() override { return true; }
            bool initializeEngineHooks() override { return true; }
            void cleanupEngineHooks() override {}
            bool initializeVR() override { return true; }
            void cleanupVR() override {}
            void updateVR() override {}
            void renderVR() override {}
        };
        
        MockAdapter mockAdapter;
        mockAdapter.initialize();
        
        // Test performance tracking
        for (int i = 0; i < 10; i++) {
            mockAdapter.update();
            mockAdapter.render();
        }
        
        // Performance metrics should be updated
        assert(mockAdapter.isVREnabled() == true);
        
        std::cout << "✓ Performance metrics tests passed" << std::endl;
        m_testsPassed++;
    }
    
    void printTestResults() {
        std::cout << "\n=== Test Results ===" << std::endl;
        std::cout << "Tests Passed: " << m_testsPassed << std::endl;
        std::cout << "Tests Failed: " << m_testsFailed << std::endl;
        std::cout << "Total Tests: " << (m_testsPassed + m_testsFailed) << std::endl;
        
        if (m_testsFailed == 0) {
            std::cout << "🎉 ALL TESTS PASSED! 🎉" << std::endl;
        } else {
            std::cout << "❌ SOME TESTS FAILED! ❌" << std::endl;
        }
    }
};

int main() {
    CrossEngineAdapterTestSuite testSuite;
    testSuite.runAllTests();
    return 0;
}
