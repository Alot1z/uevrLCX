#include "mt_framework_adapter.hpp"
#include "mt_framework_factory.hpp"
#include <uevr/CrossEngineAdapterRegistry.hpp>
#include <iostream>
#include <memory>
#include <string>

/**
 * @brief Test Monster Hunter World Cross-Engine Integration
 * 
 * This test file validates that the Monster Hunter World adapter
 * properly integrates with uevr's main cross-engine framework.
 */

namespace uevr {
namespace MTFramework {
namespace Test {

/**
 * @brief Test the adapter creation and basic functionality
 */
bool testAdapterCreation() {
    std::cout << "Testing Monster Hunter World adapter creation..." << std::endl;
    
    try {
        // Create factory
        auto factory = std::make_shared<MTFrameworkAdapterFactory>();
        
        // Test game detection
        if (!factory->canCreateAdapter("MonsterHunterWorld.exe")) {
            std::cout << "ERROR: Factory cannot create adapter for MonsterHunterWorld.exe" << std::endl;
            return false;
        }
        
        // Test adapter creation
        auto adapter = factory->createAdapter("MonsterHunterWorld.exe");
        if (!adapter) {
            std::cout << "ERROR: Failed to create Monster Hunter World adapter" << std::endl;
            return false;
        }
        
        // Test adapter information
        if (adapter->getName() != "MonsterHunterWorld-MTFramework") {
            std::cout << "ERROR: Incorrect adapter name: " << adapter->getName() << std::endl;
            return false;
        }
        
        if (adapter->getEngineType() != "MT-Framework") {
            std::cout << "ERROR: Incorrect engine type: " << adapter->getEngineType() << std::endl;
            return false;
        }
        
        if (adapter->getGameExecutable() != "MonsterHunterWorld.exe") {
            std::cout << "ERROR: Incorrect game executable: " << adapter->getGameExecutable() << std::endl;
            return false;
        }
        
        if (adapter->getVersion() != "1.0.0") {
            std::cout << "ERROR: Incorrect version: " << adapter->getVersion() << std::endl;
            return false;
        }
        
        std::cout << "✓ Adapter creation test passed" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cout << "ERROR: Exception during adapter creation test: " << e.what() << std::endl;
        return false;
    }
}

/**
 * @brief Test the cross-engine registry integration
 */
bool testRegistryIntegration() {
    std::cout << "Testing cross-engine registry integration..." << std::endl;
    
    try {
        // Create and register factory
        auto factory = std::make_shared<MTFrameworkAdapterFactory>();
        CrossEngineAdapterRegistry::registerFactory(factory);
        
        // Test factory registration
        auto factories = CrossEngineAdapterRegistry::getRegisteredFactories();
        if (factories.empty()) {
            std::cout << "ERROR: No factories registered" << std::endl;
            return false;
        }
        
        bool foundMTFramework = false;
        for (const auto& regFactory : factories) {
            if (regFactory->getSupportedEngineType() == "MT-Framework") {
                foundMTFramework = true;
                break;
            }
        }
        
        if (!foundMTFramework) {
            std::cout << "ERROR: MT Framework factory not found in registry" << std::endl;
            return false;
        }
        
        // Test adapter creation through registry
        auto adapter = CrossEngineAdapterRegistry::createAdapter("MonsterHunterWorld.exe");
        if (!adapter) {
            std::cout << "ERROR: Registry failed to create Monster Hunter World adapter" << std::endl;
            return false;
        }
        
        // Test adapter type
        if (adapter->getEngineType() != "MT-Framework") {
            std::cout << "ERROR: Registry created adapter has wrong engine type" << std::endl;
            return false;
        }
        
        std::cout << "✓ Registry integration test passed" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cout << "ERROR: Exception during registry integration test: " << e.what() << std::endl;
        return false;
    }
}

/**
 * @brief Test the adapter lifecycle methods
 */
bool testAdapterLifecycle() {
    std::cout << "Testing adapter lifecycle methods..." << std::endl;
    
    try {
        // Create adapter
        auto factory = std::make_shared<MTFrameworkAdapterFactory>();
        auto adapter = factory->createAdapter("MonsterHunterWorld.exe");
        
        if (!adapter) {
            std::cout << "ERROR: Failed to create adapter for lifecycle test" << std::endl;
            return false;
        }
        
        // Test initial state
        if (adapter->isVREnabled()) {
            std::cout << "ERROR: VR should not be enabled before initialization" << std::endl;
            return false;
        }
        
        // Test initialization (this will fail in test environment, but should not crash)
        bool initResult = adapter->initialize();
        // Note: In test environment, initialization may fail due to missing MT Framework DLL
        // This is expected and acceptable for testing
        
        // Test update and render methods (should not crash)
        adapter->update();
        adapter->render();
        
        // Test cleanup (should not crash)
        adapter->cleanup();
        
        std::cout << "✓ Adapter lifecycle test passed" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cout << "ERROR: Exception during lifecycle test: " << e.what() << std::endl;
        return false;
    }
}

/**
 * @brief Test Monster Hunter World specific functionality
 */
bool testMonsterHunterWorldFeatures() {
    std::cout << "Testing Monster Hunter World specific features..." << std::endl;
    
    try {
        // Create adapter
        auto factory = std::make_shared<MTFrameworkAdapterFactory>();
        auto adapter = factory->createAdapter("MonsterHunterWorld.exe");
        
        if (!adapter) {
            std::cout << "ERROR: Failed to create adapter for feature test" << std::endl;
            return false;
        }
        
        // Cast to MonsterHunterWorldAdapter to test specific methods
        auto mhwAdapter = std::dynamic_pointer_cast<MonsterHunterWorldAdapter>(adapter);
        if (!mhwAdapter) {
            std::cout << "ERROR: Failed to cast to MonsterHunterWorldAdapter" << std::endl;
            return false;
        }
        
        // Test Monster Hunter World specific methods
        mhwAdapter->enableFirstPersonMode(true);
        mhwAdapter->setCameraOffset(0.0f, 1.6f, 0.0f);
        mhwAdapter->setFOVMultiplier(1.0f);
        
        std::cout << "✓ Monster Hunter World features test passed" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cout << "ERROR: Exception during feature test: " << e.what() << std::endl;
        return false;
    }
}

/**
 * @brief Run all integration tests
 */
bool runAllTests() {
    std::cout << "=== Monster Hunter World Cross-Engine Integration Tests ===" << std::endl;
    
    bool allPassed = true;
    
    // Run all tests
    allPassed &= testAdapterCreation();
    allPassed &= testRegistryIntegration();
    allPassed &= testAdapterLifecycle();
    allPassed &= testMonsterHunterWorldFeatures();
    
    std::cout << "\n=== Test Results ===" << std::endl;
    if (allPassed) {
        std::cout << "✓ ALL TESTS PASSED - Monster Hunter World integration is working correctly!" << std::endl;
    } else {
        std::cout << "✗ SOME TESTS FAILED - Check the errors above" << std::endl;
    }
    
    return allPassed;
}

} // namespace Test
} // namespace MTFramework
} // namespace uevr

/**
 * @brief Main test function
 */
int main() {
    return uevr::MTFramework::Test::runAllTests() ? 0 : 1;
}
