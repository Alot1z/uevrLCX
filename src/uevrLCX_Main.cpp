#include "vr/UniversalVRConverter.hpp"
#include <spdlog/spdlog.h>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>

using namespace uevr;

/**
 * @brief uevrLCX Main Application
 * 
 * This is the main entry point for the uevrLCX Universal VR Conversion System.
 * It demonstrates how to use the complete VR conversion framework to transform
 * any flat PC game into a fully functional VR experience.
 * 
 * Key Features Demonstrated:
 * - Universal game compatibility
 * - Real collision detection
 * - Performance optimization
 * - AI-powered game analysis
 * - Community integration
 */
int main(int argc, char* argv[]) {
    std::cout << "==========================================" << std::endl;
    std::cout << "    uevrLCX Universal VR Conversion System" << std::endl;
    std::cout << "==========================================" << std::endl;
    std::cout << "Version: 2.0.0" << std::endl;
    std::cout << "Universal VR Conversion for Any PC Game" << std::endl;
    std::cout << "==========================================" << std::endl;
    std::cout << std::endl;

    try {
        // Get the Universal VR Converter instance
        auto& converter = UniversalVRConverter::getInstance();
        
        // Initialize the system
        std::cout << "Initializing uevrLCX system..." << std::endl;
        if (!converter.initialize()) {
            std::cerr << "Failed to initialize uevrLCX system!" << std::endl;
            return -1;
        }
        std::cout << "uevrLCX system initialized successfully!" << std::endl;
        std::cout << std::endl;

        // Display supported engines
        std::cout << "Supported Game Engines:" << std::endl;
        auto supportedEngines = converter.getSupportedEngines();
        for (const auto& engine : supportedEngines) {
            std::cout << "  - " << engine << std::endl;
        }
        std::cout << std::endl;

        // Parse command line arguments
        std::string gamePath;
        std::string gameName;
        bool autoDetect = false;
        bool injectRunning = false;
        std::string processName;

        for (int i = 1; i < argc; i++) {
            std::string arg = argv[i];
            
            if (arg == "--game" && i + 1 < argc) {
                gamePath = argv[++i];
            } else if (arg == "--name" && i + 1 < argc) {
                gameName = argv[++i];
            } else if (arg == "--auto-detect") {
                autoDetect = true;
            } else if (arg == "--inject" && i + 1 < argc) {
                injectRunning = true;
                processName = argv[++i];
            } else if (arg == "--help" || arg == "-h") {
                std::cout << "Usage: uevrLCX [options]" << std::endl;
                std::cout << "Options:" << std::endl;
                std::cout << "  --game <path>        Path to game executable" << std::endl;
                std::cout << "  --name <name>        Game name" << std::endl;
                std::cout << "  --auto-detect        Auto-detect and convert active game" << std::endl;
                std::cout << "  --inject <process>   Inject into running process" << std::endl;
                std::cout << "  --help, -h           Show this help message" << std::endl;
                std::cout << std::endl;
                std::cout << "Examples:" << std::endl;
                std::cout << "  uevrLCX --game \"C:\\Games\\MyGame.exe\" --name \"MyGame\"" << std::endl;
                std::cout << "  uevrLCX --auto-detect" << std::endl;
                std::cout << "  uevrLCX --inject \"MyGame.exe\"" << std::endl;
                return 0;
            }
        }

        // Configure conversion settings
        UniversalVRConverter::ConversionSettings settings;
        settings.enableStereoscopicRendering = true;
        settings.enableDepthBufferExtraction = true;
        settings.enableRealCollisionDetection = true;
        settings.enableFoveatedRendering = true;
        settings.enableAsyncTimewarp = true;
        settings.targetFrameRate = 90;
        settings.enablePerformanceOptimization = true;
        settings.enableMotionControllers = true;
        settings.enableRoomScaleTracking = true;
        settings.enableHapticFeedback = true;
        settings.enableAIGameAnalysis = true;
        settings.enableAutoOptimization = true;
        settings.enableCommunityLearning = true;
        settings.enableDebugMode = false;
        settings.logLevel = "info";

        converter.setConversionSettings(settings);
        std::cout << "Conversion settings configured." << std::endl;
        std::cout << std::endl;

        // Perform VR conversion based on arguments
        bool conversionSuccess = false;

        if (autoDetect) {
            std::cout << "Auto-detecting and converting active game..." << std::endl;
            conversionSuccess = converter.detectAndConvertGame();
        } else if (injectRunning) {
            std::cout << "Injecting into running process: " << processName << std::endl;
            conversionSuccess = converter.injectIntoRunningGame(processName);
        } else if (!gamePath.empty()) {
            if (gameName.empty()) {
                // Extract game name from path
                size_t lastSlash = gamePath.find_last_of("/\\");
                size_t lastDot = gamePath.find_last_of('.');
                if (lastSlash != std::string::npos && lastDot != std::string::npos) {
                    gameName = gamePath.substr(lastSlash + 1, lastDot - lastSlash - 1);
                } else {
                    gameName = "Unknown Game";
                }
            }
            
            std::cout << "Converting game: " << gameName << std::endl;
            std::cout << "Game path: " << gamePath << std::endl;
            conversionSuccess = converter.convertGameToVR(gamePath, gameName);
        } else {
            std::cout << "No game specified. Use --help for usage information." << std::endl;
            std::cout << "Example: uevrLCX --auto-detect" << std::endl;
            return 0;
        }

        if (conversionSuccess) {
            std::cout << "VR conversion started successfully!" << std::endl;
            std::cout << std::endl;

            // Monitor conversion status
            std::cout << "Monitoring VR conversion..." << std::endl;
            std::cout << "Press Ctrl+C to stop conversion" << std::endl;
            std::cout << std::endl;

            // Main monitoring loop
            while (converter.isConverting()) {
                // Get performance metrics
                auto metrics = converter.getPerformanceMetrics();
                
                // Display status
                std::cout << "\rStatus: Converting | "
                          << "FPS: " << std::fixed << std::setprecision(1) << metrics.currentFrameRate << " | "
                          << "Latency: " << std::fixed << std::setprecision(1) << metrics.latency << "ms | "
                          << "VR Score: " << std::fixed << std::setprecision(1) << metrics.vrPerformanceScore << "%"
                          << std::flush;

                // Check for errors
                if (converter.hasErrors()) {
                    auto errors = converter.getRecentErrors();
                    if (!errors.empty()) {
                        std::cout << std::endl;
                        std::cout << "Warning: " << errors.back().errorMessage << std::endl;
                    }
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            }

            std::cout << std::endl;
            std::cout << "VR conversion completed." << std::endl;

        } else {
            std::cerr << "Failed to start VR conversion!" << std::endl;
            
            // Display errors
            if (converter.hasErrors()) {
                auto errors = converter.getRecentErrors();
                std::cout << "Errors:" << std::endl;
                for (const auto& error : errors) {
                    std::cout << "  [" << error.errorCode << "] " << error.errorMessage << std::endl;
                    if (!error.suggestedFix.empty()) {
                        std::cout << "    Suggested fix: " << error.suggestedFix << std::endl;
                    }
                }
            }
            
            return -1;
        }

        // Display final statistics
        std::cout << std::endl;
        std::cout << "Final Statistics:" << std::endl;
        auto finalMetrics = converter.getPerformanceMetrics();
        std::cout << "  Average FPS: " << std::fixed << std::setprecision(1) << finalMetrics.averageFrameRate << std::endl;
        std::cout << "  Final VR Score: " << std::fixed << std::setprecision(1) << finalMetrics.vrPerformanceScore << "%" << std::endl;
        std::cout << "  Dropped Frames: " << finalMetrics.droppedFrames << std::endl;
        std::cout << "  Memory Usage: " << std::fixed << std::setprecision(1) << finalMetrics.memoryUsage << " MB" << std::endl;

        // Display community profiles
        auto communityProfiles = converter.getCommunityProfiles();
        if (!communityProfiles.empty()) {
            std::cout << std::endl;
            std::cout << "Community Profiles Available:" << std::endl;
            for (const auto& profile : communityProfiles) {
                std::cout << "  - " << profile << std::endl;
            }
        }

        std::cout << std::endl;
        std::cout << "Thank you for using uevrLCX!" << std::endl;
        std::cout << "Universal VR Conversion System" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}

// Additional utility functions for demonstration

/**
 * @brief Demonstrate advanced VR conversion features
 */
void demonstrateAdvancedFeatures(UniversalVRConverter& converter) {
    std::cout << "Demonstrating advanced features..." << std::endl;

    // Enable neural rendering
    converter.enableNeuralRendering(true);
    std::cout << "  - Neural rendering enabled" << std::endl;

    // Enable AI collision prediction
    converter.enableAICollisionPrediction(true);
    std::cout << "  - AI collision prediction enabled" << std::endl;

    // Enable dynamic optimization
    converter.enableDynamicOptimization(true);
    std::cout << "  - Dynamic optimization enabled" << std::endl;

    // Load community profiles
    auto profiles = converter.getCommunityProfiles();
    if (!profiles.empty()) {
        std::cout << "  - Loaded " << profiles.size() << " community profiles" << std::endl;
    }

    std::cout << "Advanced features demonstration completed." << std::endl;
    std::cout << std::endl;
}

/**
 * @brief Demonstrate performance monitoring
 */
void demonstratePerformanceMonitoring(UniversalVRConverter& converter) {
    std::cout << "Performance monitoring demonstration..." << std::endl;

    // Enable performance monitoring
    converter.enablePerformanceMonitoring(true);

    // Monitor for 5 seconds
    for (int i = 0; i < 5; i++) {
        auto metrics = converter.getPerformanceMetrics();
        std::cout << "  Frame " << (i + 1) << ": "
                  << "FPS=" << std::fixed << std::setprecision(1) << metrics.currentFrameRate
                  << ", Latency=" << std::fixed << std::setprecision(1) << metrics.latency << "ms"
                  << ", VR Score=" << std::fixed << std::setprecision(1) << metrics.vrPerformanceScore << "%"
                  << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    std::cout << "Performance monitoring demonstration completed." << std::endl;
    std::cout << std::endl;
}

/**
 * @brief Demonstrate plugin system
 */
void demonstratePluginSystem(UniversalVRConverter& converter) {
    std::cout << "Plugin system demonstration..." << std::endl;

    // List loaded plugins
    auto plugins = converter.getLoadedPlugins();
    if (!plugins.empty()) {
        std::cout << "  Loaded plugins:" << std::endl;
        for (const auto& plugin : plugins) {
            std::cout << "    - " << plugin << std::endl;
        }
    } else {
        std::cout << "  No plugins currently loaded" << std::endl;
    }

    std::cout << "Plugin system demonstration completed." << std::endl;
    std::cout << std::endl;
}
