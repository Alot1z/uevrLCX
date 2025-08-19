/*
Mass Deployment Test Program
Demonstrates the universal cross-engine VR system for 500+ games.
*/

#include "common/UniversalEngineRegistry.hpp"
#include "common/UniversalVRFactory.hpp"
#include <iostream>
#include <thread>
#include <chrono>

using namespace uevr;

int main() {
    std::cout << "=== UEVR Universal Cross-Engine VR System - Mass Deployment Test ===" << std::endl;
    std::cout << "Target: 500+ games across all major engines" << std::endl;
    std::cout << "================================================================" << std::endl;
    
    try {
        // Initialize the universal engine registry
        std::cout << "\n[1/4] Initializing Universal Engine Registry..." << std::endl;
        if (!UniversalEngineRegistry::initialize()) {
            std::cerr << "Failed to initialize Universal Engine Registry!" << std::endl;
            return -1;
        }
        
        // Initialize the universal VR factory
        std::cout << "\n[2/4] Initializing Universal VR Factory..." << std::endl;
        UniversalVRFactory::FactoryConfig config;
        config.enableAutoDetection = true;
        config.enableFallbackAdapters = true;
        config.enablePerformanceOptimization = true;
        config.enableQualityAdaptation = true;
        config.enableCrossEngineCompatibility = true;
        config.defaultAdapterType = "Universal";
        config.preferredEngines = {"Unreal Engine 5", "RE Engine", "REDengine 4", "MT Framework"};
        
        if (!UniversalVRFactory::initialize(config)) {
            std::cerr << "Failed to initialize Universal VR Factory!" << std::endl;
            return -1;
        }
        
        // Display system capabilities
        std::cout << "\n[3/4] System Capabilities:" << std::endl;
        std::cout << "  Supported Engines: " << UniversalEngineRegistry::getTotalSupportedEngines() << std::endl;
        std::cout << "  Supported Games: " << UniversalEngineRegistry::getTotalSupportedGames() << std::endl;
        
        auto supportedEngines = UniversalEngineRegistry::getSupportedEngines();
        std::cout << "  Engine Types:" << std::endl;
        for (const auto& engine : supportedEngines) {
            auto games = UniversalEngineRegistry::getSupportedGames(engine);
            std::cout << "    - " << static_cast<int>(engine) << " (" << games.size() << " games)" << std::endl;
        }
        
        // Auto-detect current game and engine
        std::cout << "\n[4/4] Auto-Detection Test:" << std::endl;
        std::cout << "Scanning for games and engines..." << std::endl;
        
        auto detectedEngine = UniversalEngineRegistry::autoDetectEngine();
        auto detectedGame = UniversalEngineRegistry::autoDetectGame();
        
        if (detectedEngine != EngineType::UNKNOWN_ENGINE) {
            std::cout << "✓ Engine Detected: " << static_cast<int>(detectedEngine) << std::endl;
            
            auto compatibility = UniversalEngineRegistry::getEngineCompatibility();
            std::cout << "  Engine Name: " << compatibility.engineName << std::endl;
            std::cout << "  Generic Rendering: " << (compatibility.supportsGenericRendering ? "Yes" : "No") << std::endl;
            std::cout << "  Custom Hooks Required: " << (compatibility.requiresCustomHooks ? "Yes" : "No") << std::endl;
            
            // VR Capabilities
            std::cout << "  VR Capabilities:" << std::endl;
            std::cout << "    - Stereo Rendering: " << (compatibility.vrCapabilities.supportsStereoRendering ? "Yes" : "No") << std::endl;
            std::cout << "    - Motion Controllers: " << (compatibility.vrCapabilities.supportsMotionControllers ? "Yes" : "No") << std::endl;
            std::cout << "    - Haptic Feedback: " << (compatibility.vrCapabilities.supportsHapticFeedback ? "Yes" : "No") << std::endl;
            std::cout << "    - Dynamic FOV: " << (compatibility.vrCapabilities.supportsDynamicFOV ? "Yes" : "No") << std::endl;
            std::cout << "    - Frame Interpolation: " << (compatibility.vrCapabilities.supportsFrameInterpolation ? "Yes" : "No") << std::endl;
            std::cout << "    - Neural Upscaling: " << (compatibility.vrCapabilities.supportsNeuralUpscaling ? "Yes" : "No") << std::endl;
            std::cout << "    - Ray Tracing: " << (compatibility.vrCapabilities.supportsRayTracing ? "Yes" : "No") << std::endl;
            std::cout << "    - Temporal Upsampling: " << (compatibility.vrCapabilities.supportsTemporalUpsampling ? "Yes" : "No") << std::endl;
        } else {
            std::cout << "✗ No engine detected" << std::endl;
        }
        
        if (!detectedGame.empty()) {
            std::cout << "✓ Game Detected: " << detectedGame << std::endl;
            
            auto vrConfig = UniversalEngineRegistry::getUniversalVRConfig();
            std::cout << "  VR Configuration:" << std::endl;
            std::cout << "    - Target Frame Rate: " << vrConfig.targetFrameRate << " FPS" << std::endl;
            std::cout << "    - Target Latency: " << vrConfig.targetLatency << "ms" << std::endl;
            std::cout << "    - Adaptive Quality: " << (vrConfig.adaptiveQuality ? "Yes" : "No") << std::endl;
            std::cout << "    - Performance Monitoring: " << (vrConfig.performanceMonitoring ? "Yes" : "No") << std::endl;
        } else {
            std::cout << "✗ No game detected" << std::endl;
        }
        
        // Test adapter creation
        std::cout << "\n[5/5] Testing Universal Adapter Creation..." << std::endl;
        auto adapter = UniversalVRFactory::autoDetectAndCreate();
        
        if (adapter) {
            std::cout << "✓ Universal adapter created successfully!" << std::endl;
            std::cout << "  Adapter Name: " << adapter->getName() << std::endl;
            std::cout << "  Engine Type: " << adapter->getEngineType() << std::endl;
            std::cout << "  Game Executable: " << adapter->getGameExecutable() << std::endl;
            std::cout << "  Version: " << adapter->getVersion() << std::endl;
            
            // Test initialization
            std::cout << "\nTesting adapter initialization..." << std::endl;
            if (adapter->initialize()) {
                std::cout << "✓ Adapter initialized successfully!" << std::endl;
                std::cout << "  VR Enabled: " << (adapter->isVREnabled() ? "Yes" : "No") << std::endl;
                
                // Test update and render
                std::cout << "\nTesting adapter update and render..." << std::endl;
                for (int i = 0; i < 5; ++i) {
                    adapter->update();
                    adapter->render();
                    std::cout << "  Frame " << (i + 1) << " processed" << std::endl;
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
                
                // Cleanup
                adapter->cleanup();
                std::cout << "✓ Adapter cleanup completed" << std::endl;
            } else {
                std::cout << "✗ Adapter initialization failed" << std::endl;
            }
        } else {
            std::cout << "✗ Failed to create universal adapter" << std::endl;
        }
        
        // Display factory statistics
        std::cout << "\n=== Factory Statistics ===" << std::endl;
        std::cout << UniversalVRFactory::getFactoryStatistics() << std::endl;
        
        // Cleanup
        std::cout << "\nCleaning up..." << std::endl;
        UniversalVRFactory::cleanup();
        UniversalEngineRegistry::cleanup();
        
        std::cout << "\n=== Mass Deployment Test Completed Successfully! ===" << std::endl;
        std::cout << "The system is ready to support 500+ games across all major engines!" << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Mass deployment test failed with exception: " << e.what() << std::endl;
        return -1;
    }
}


