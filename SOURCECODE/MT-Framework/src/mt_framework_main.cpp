#include "mt_framework_adapter.hpp"
#include "mt_framework_factory.hpp"
#include <uevr/CrossEngineAdapterRegistry.hpp>
#include <iostream>
#include <memory>

using namespace uevr;
using namespace uevr::MTFramework;

/**
 * @brief Main entry point for MT Framework VR Adapter
 * 
 * This is the main entry point that demonstrates the MT Framework adapter
 * capabilities and provides integration with the cross-engine framework.
 * 
 * @version 1.0.0
 * @author uevr Cross-Engine Adapter Project
 */
int main(int argc, char* argv[]) {
    std::cout << "==========================================" << std::endl;
    std::cout << "MT Framework VR Adapter" << std::endl;
    std::cout << "==========================================" << std::endl;
    std::cout << "Version: " << MTFrameworkFactory::getVersion() << std::endl;
    std::cout << "Description: " << MTFrameworkFactory::getDescription() << std::endl;
    std::cout << std::endl;

    try {
        // Check if MT Framework is supported
        if (!MTFrameworkFactory::isSupported()) {
            std::cerr << "ERROR: MT Framework is not supported on this system" << std::endl;
            return 1;
        }

        // Get supported games
        auto supportedGames = MTFrameworkFactory::getSupportedGames();
        std::cout << "Supported Games:" << std::endl;
        for (const auto& game : supportedGames) {
            std::cout << "  - " << game << std::endl;
        }
        std::cout << std::endl;

        // Create adapter
        std::cout << "Creating MT Framework adapter..." << std::endl;
        auto adapter = MTFrameworkFactory::createAdapter();
        
        if (!adapter) {
            std::cerr << "ERROR: Failed to create MT Framework adapter" << std::endl;
            return 1;
        }

        std::cout << "Adapter created successfully:" << std::endl;
        std::cout << "  Name: " << adapter->getName() << std::endl;
        std::cout << "  Engine Type: " << adapter->getEngineType() << std::endl;
        std::cout << "  Game Executable: " << adapter->getGameExecutable() << std::endl;
        std::cout << "  Version: " << adapter->getVersion() << std::endl;
        std::cout << std::endl;

        // Initialize adapter
        std::cout << "Initializing adapter..." << std::endl;
        if (!adapter->initialize()) {
            std::cerr << "ERROR: Failed to initialize adapter: " << adapter->getLastError() << std::endl;
            return 1;
        }

        std::cout << "Adapter initialized successfully" << std::endl;
        std::cout << "Ready: " << (adapter->isReady() ? "Yes" : "No") << std::endl;
        std::cout << std::endl;

        // Test VR functionality
        std::cout << "Testing VR functionality..." << std::endl;
        
        // Enable debug mode
        adapter->setDebugMode(true);
        
        // Test VR enable
        if (adapter->enableVR()) {
            std::cout << "VR mode enabled successfully" << std::endl;
            std::cout << "VR Enabled: " << (adapter->isVREnabled() ? "Yes" : "No") << std::endl;
            std::cout << "Rendering Hooks Active: " << (adapter->areRenderingHooksActive() ? "Yes" : "No") << std::endl;
        } else {
            std::cout << "WARNING: Failed to enable VR mode: " << adapter->getLastError() << std::endl;
        }

        // Test camera conversion
        if (adapter->convertCameraToVR()) {
            std::cout << "Camera converted to VR successfully" << std::endl;
        } else {
            std::cout << "WARNING: Failed to convert camera to VR: " << adapter->getLastError() << std::endl;
        }

        // Test camera matrices
        float viewMatrix[16], projectionMatrix[16];
        if (adapter->updateCameraMatrices(viewMatrix, projectionMatrix)) {
            std::cout << "Camera matrices updated successfully" << std::endl;
        } else {
            std::cout << "WARNING: Failed to update camera matrices: " << adapter->getLastError() << std::endl;
        }

        // Test performance metrics
        std::cout << "Performance Metrics:" << std::endl;
        std::cout << "  Frame Time: " << adapter->getFrameTime() << " ms" << std::endl;
        std::cout << "  Average Frame Time: " << adapter->getAverageFrameTime() << " ms" << std::endl;
        std::cout << "  FPS: " << adapter->getFPS() << std::endl;
        std::cout << std::endl;

        // Test configuration
        std::cout << "Configuration Test:" << std::endl;
        adapter->setConfigValue("test_key", "test_value");
        std::cout << "  Test Key Value: " << adapter->getConfigValue("test_key", "default") << std::endl;
        std::cout << "  Non-existent Key: " << adapter->getConfigValue("non_existent", "default_value") << std::endl;
        std::cout << std::endl;

        // Test MT Framework specific features
        std::cout << "MT Framework Specific Features:" << std::endl;
        std::cout << "  Renderer Handle: " << (adapter->getRenderer() ? "Available" : "Not Available") << std::endl;
        std::cout << "  Camera Handle: " << (adapter->getCamera() ? "Available" : "Not Available") << std::endl;
        std::cout << "  Input Handle: " << (adapter->getInput() ? "Available" : "Not Available") << std::endl;
        std::cout << "  First Person Mode: " << (adapter->isFirstPersonMode() ? "Enabled" : "Disabled") << std::endl;
        
        float cameraOffset[3];
        adapter->getCameraOffset(cameraOffset);
        std::cout << "  Camera Offset: [" << cameraOffset[0] << ", " << cameraOffset[1] << ", " << cameraOffset[2] << "]" << std::endl;
        
        std::cout << "  FOV Multiplier: " << adapter->getFOVMultiplier() << std::endl;
        std::cout << std::endl;

        // Test VR input processing
        std::cout << "Testing VR input processing..." << std::endl;
        if (adapter->processVRInput(nullptr, nullptr)) {
            std::cout << "VR input processing successful" << std::endl;
        } else {
            std::cout << "WARNING: VR input processing failed: " << adapter->getLastError() << std::endl;
        }

        if (adapter->mapVRInputToGameActions()) {
            std::cout << "VR input mapping successful" << std::endl;
        } else {
            std::cout << "WARNING: VR input mapping failed: " << adapter->getLastError() << std::endl;
        }
        std::cout << std::endl;

        // Test error handling
        std::cout << "Error Handling Test:" << std::endl;
        std::cout << "  Has Errors: " << (adapter->hasErrors() ? "Yes" : "No") << std::endl;
        if (adapter->hasErrors()) {
            std::cout << "  Last Error: " << adapter->getLastError() << std::endl;
        }
        
        adapter->clearErrors();
        std::cout << "  After Clear - Has Errors: " << (adapter->hasErrors() ? "Yes" : "No") << std::endl;
        std::cout << std::endl;

        // Integration with cross-engine registry
        std::cout << "Integrating with Cross-Engine Registry..." << std::endl;
        auto& registry = CrossEngineAdapterRegistry::getInstance();
        
        // Register the adapter
        if (registry.registerAdapter(
            adapter->getName(),
            adapter->getEngineType(),
            []() -> std::unique_ptr<ICrossEngineAdapter> {
                return MTFrameworkFactory::createAdapter();
            }
        )) {
            std::cout << "Adapter registered successfully with cross-engine registry" << std::endl;
        } else {
            std::cout << "WARNING: Failed to register adapter with cross-engine registry" << std::endl;
        }

        // Display registry statistics
        std::cout << "\nRegistry Statistics:" << std::endl;
        std::cout << registry.getRegistryStats() << std::endl;

        // Cleanup
        std::cout << "\nShutting down adapter..." << std::endl;
        adapter->shutdown();
        
        std::cout << "Adapter shutdown complete" << std::endl;
        std::cout << "==========================================" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown fatal error occurred" << std::endl;
        return 1;
    }

    return 0;
}
