#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <thread>
#include <chrono>
#include <filesystem>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>

// Core system includes
#include "vr/UniversalVRConverter.hpp"
#include "vr/RedditCommunityIntegration.hpp"
#include "core/Framework.hpp"

using namespace uevr;

/**
 * @brief Enhanced uevrLCX Main Application
 * 
 * This is the main entry point for the uevrLCX Universal VR Conversion System.
 * It demonstrates the complete system capabilities including:
 * - Universal game-to-VR conversion
 * - Reddit community solution integration
 * - Real collision detection (not fake depth)
 * - Performance optimization
 * - AI-powered game analysis
 * - Community-driven compatibility
 */
class uevrLCXEnhancedMain {
public:
    uevrLCXEnhancedMain() {
        initializeLogging();
        spdlog::info("=== uevrLCX Enhanced Main Application Started ===");
        spdlog::info("Universal VR Conversion System with Community Integration");
    }

    ~uevrLCXEnhancedMain() {
        spdlog::info("=== uevrLCX Enhanced Main Application Shutdown ===");
    }

    bool run() {
        try {
            spdlog::info("Starting uevrLCX Enhanced Main Application...");

            // Initialize the Universal VR Converter
            auto& converter = UniversalVRConverter::getInstance();
            if (!converter.initialize()) {
                spdlog::error("Failed to initialize Universal VR Converter");
                return false;
            }

            // Integrate Reddit community solutions
            if (!converter.integrateRedditCommunitySolutions()) {
                spdlog::warn("Failed to integrate Reddit community solutions, continuing...");
            }

            // Display main menu
            displayMainMenu();

            // Process user input
            processUserInput();

            // Shutdown
            converter.shutdown();

            return true;

        } catch (const std::exception& e) {
            spdlog::error("Exception in main application: {}", e.what());
            return false;
        }
    }

private:
    void initializeLogging() {
        try {
            // Create logs directory
            std::filesystem::create_directories("logs");

            // Set up rotating file logger
            auto logger = spdlog::rotating_logger_mt("uevrLCX_enhanced", 
                "logs/uevrLCX_enhanced.log", 1024*1024*10, 5);
            spdlog::set_default_logger(logger);
            spdlog::set_level(spdlog::level::info);

            spdlog::info("Logging system initialized");
        } catch (const std::exception& e) {
            std::cerr << "Failed to initialize logging: " << e.what() << std::endl;
        }
    }

    void displayMainMenu() {
        std::cout << "\n";
        std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
        std::cout << "║                    uevrLCX Enhanced Main Menu               ║\n";
        std::cout << "╠══════════════════════════════════════════════════════════════╣\n";
        std::cout << "║  Universal VR Conversion System with Community Integration  ║\n";
        std::cout << "╠══════════════════════════════════════════════════════════════╣\n";
        std::cout << "║                                                              ║\n";
        std::cout << "║  1. Convert Game to VR (Auto-detect)                        ║\n";
        std::cout << "║  2. Convert Specific Game                                   ║\n";
        std::cout << "║  3. Inject into Running Game                                ║\n";
        std::cout << "║  4. Browse Community Solutions                              ║\n";
        std::cout << "║  5. Search Community Solutions                              ║\n";
        std::cout << "║  6. Apply Community Solution                                ║\n";
        std::cout << "║  7. Performance Monitoring                                  ║\n";
        std::cout << "║  8. System Status                                           ║\n";
        std::cout << "║  9. Configuration                                           ║\n";
        std::cout << "║  0. Exit                                                    ║\n";
        std::cout << "║                                                              ║\n";
        std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
        std::cout << "\nEnter your choice: ";
    }

    void processUserInput() {
        auto& converter = UniversalVRConverter::getInstance();
        std::string input;

        while (true) {
            std::getline(std::cin, input);

            if (input == "0" || input == "exit" || input == "quit") {
                spdlog::info("User requested exit");
                break;
            }
            else if (input == "1") {
                handleAutoDetectConversion(converter);
            }
            else if (input == "2") {
                handleSpecificGameConversion(converter);
            }
            else if (input == "3") {
                handleRunningGameInjection(converter);
            }
            else if (input == "4") {
                handleBrowseCommunitySolutions(converter);
            }
            else if (input == "5") {
                handleSearchCommunitySolutions(converter);
            }
            else if (input == "6") {
                handleApplyCommunitySolution(converter);
            }
            else if (input == "7") {
                handlePerformanceMonitoring(converter);
            }
            else if (input == "8") {
                handleSystemStatus(converter);
            }
            else if (input == "9") {
                handleConfiguration(converter);
            }
            else {
                std::cout << "Invalid choice. Please try again.\n";
            }

            displayMainMenu();
        }
    }

    void handleAutoDetectConversion(UniversalVRConverter& converter) {
        std::cout << "\n=== Auto-Detect Game Conversion ===\n";
        std::cout << "Detecting and converting games to VR...\n";

        try {
            if (converter.detectAndConvertGame()) {
                std::cout << "✓ Game successfully converted to VR!\n";
                spdlog::info("Auto-detect conversion completed successfully");
            } else {
                std::cout << "✗ Failed to convert game to VR\n";
                spdlog::error("Auto-detect conversion failed");
            }
        } catch (const std::exception& e) {
            std::cout << "✗ Error during conversion: " << e.what() << "\n";
            spdlog::error("Exception during auto-detect conversion: {}", e.what());
        }

        std::cout << "\nPress Enter to continue...";
        std::cin.get();
    }

    void handleSpecificGameConversion(UniversalVRConverter& converter) {
        std::cout << "\n=== Specific Game Conversion ===\n";
        std::cout << "Enter game executable path: ";
        
        std::string gamePath;
        std::getline(std::cin, gamePath);

        if (gamePath.empty()) {
            std::cout << "No path provided.\n";
            return;
        }

        std::cout << "Converting game: " << gamePath << "\n";

        try {
            if (converter.convertGameToVR(gamePath, "Unknown Game")) {
                std::cout << "✓ Game successfully converted to VR!\n";
                spdlog::info("Specific game conversion completed: {}", gamePath);
            } else {
                std::cout << "✗ Failed to convert game to VR\n";
                spdlog::error("Specific game conversion failed: {}", gamePath);
            }
        } catch (const std::exception& e) {
            std::cout << "✗ Error during conversion: " << e.what() << "\n";
            spdlog::error("Exception during specific game conversion: {}", e.what());
        }

        std::cout << "\nPress Enter to continue...";
        std::cin.get();
    }

    void handleRunningGameInjection(UniversalVRConverter& converter) {
        std::cout << "\n=== Running Game Injection ===\n";
        std::cout << "Enter process name to inject into: ";
        
        std::string processName;
        std::getline(std::cin, processName);

        if (processName.empty()) {
            std::cout << "No process name provided.\n";
            return;
        }

        std::cout << "Injecting into process: " << processName << "\n";

        try {
            if (converter.injectIntoRunningGame(processName)) {
                std::cout << "✓ Successfully injected into running game!\n";
                spdlog::info("Running game injection completed: {}", processName);
            } else {
                std::cout << "✗ Failed to inject into running game\n";
                spdlog::error("Running game injection failed: {}", processName);
            }
        } catch (const std::exception& e) {
            std::cout << "✗ Error during injection: " << e.what() << "\n";
            spdlog::error("Exception during running game injection: {}", e.what());
        }

        std::cout << "\nPress Enter to continue...";
        std::cin.get();
    }

    void handleBrowseCommunitySolutions(UniversalVRConverter& converter) {
        std::cout << "\n=== Browse Community Solutions ===\n";
        std::cout << "Enter game name to browse solutions for: ";
        
        std::string gameName;
        std::getline(std::cin, gameName);

        if (gameName.empty()) {
            std::cout << "No game name provided.\n";
            return;
        }

        try {
            auto solutions = converter.getCommunitySolutionsForGame(gameName);
            
            if (solutions.empty()) {
                std::cout << "No community solutions found for: " << gameName << "\n";
            } else {
                std::cout << "\nFound " << solutions.size() << " community solutions:\n";
                std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
                
                for (size_t i = 0; i < solutions.size() && i < 10; ++i) {
                    const auto& solution = solutions[i];
                    std::cout << "║ " << (i + 1) << ". " << solution.title << "\n";
                    std::cout << "║    Author: " << solution.author << " | Upvotes: " << solution.upvotes << "\n";
                    std::cout << "║    Engine: " << solution.engineType << " | Verified: " << (solution.isVerified ? "Yes" : "No") << "\n";
                    std::cout << "║    " << solution.description.substr(0, 60) << "...\n";
                    std::cout << "║\n";
                }
                
                std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
            }
        } catch (const std::exception& e) {
            std::cout << "✗ Error browsing community solutions: " << e.what() << "\n";
            spdlog::error("Exception while browsing community solutions: {}", e.what());
        }

        std::cout << "\nPress Enter to continue...";
        std::cin.get();
    }

    void handleSearchCommunitySolutions(UniversalVRConverter& converter) {
        std::cout << "\n=== Search Community Solutions ===\n";
        std::cout << "Enter search query: ";
        
        std::string query;
        std::getline(std::cin, query);

        if (query.empty()) {
            std::cout << "No search query provided.\n";
            return;
        }

        std::cout << "Searching for: " << query << "\n";

        try {
            // This would implement actual search functionality
            std::cout << "Search functionality to be implemented...\n";
            spdlog::info("Community solution search requested: {}", query);
        } catch (const std::exception& e) {
            std::cout << "✗ Error during search: " << e.what() << "\n";
            spdlog::error("Exception during community solution search: {}", e.what());
        }

        std::cout << "\nPress Enter to continue...";
        std::cin.get();
    }

    void handleApplyCommunitySolution(UniversalVRConverter& converter) {
        std::cout << "\n=== Apply Community Solution ===\n";
        std::cout << "Enter solution ID: ";
        
        std::string solutionId;
        std::getline(std::cin, solutionId);

        if (solutionId.empty()) {
            std::cout << "No solution ID provided.\n";
            return;
        }

        std::cout << "Enter game path: ";
        std::string gamePath;
        std::getline(std::cin, gamePath);

        if (gamePath.empty()) {
            std::cout << "No game path provided.\n";
            return;
        }

        std::cout << "Applying solution " << solutionId << " to " << gamePath << "\n";

        try {
            if (converter.applyRedditSolution(solutionId, gamePath)) {
                std::cout << "✓ Community solution applied successfully!\n";
                spdlog::info("Community solution applied: {} to {}", solutionId, gamePath);
            } else {
                std::cout << "✗ Failed to apply community solution\n";
                spdlog::error("Failed to apply community solution: {} to {}", solutionId, gamePath);
            }
        } catch (const std::exception& e) {
            std::cout << "✗ Error applying community solution: " << e.what() << "\n";
            spdlog::error("Exception while applying community solution: {}", e.what());
        }

        std::cout << "\nPress Enter to continue...";
        std::cin.get();
    }

    void handlePerformanceMonitoring(UniversalVRConverter& converter) {
        std::cout << "\n=== Performance Monitoring ===\n";
        
        try {
            // Get performance metrics
            auto metrics = converter.getPerformanceMetrics();
            
            std::cout << "Current Performance Metrics:\n";
            std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
            std::cout << "║ Frame Rate: " << std::fixed << std::setprecision(2) << metrics.frameRate << " FPS\n";
            std::cout << "║ Latency: " << metrics.latency << " ms\n";
            std::cout << "║ CPU Usage: " << metrics.cpuUsage << "%\n";
            std::cout << "║ GPU Usage: " << metrics.gpuUsage << "%\n";
            std::cout << "║ Memory Usage: " << metrics.memoryUsage << " MB\n";
            std::cout << "║ Improvement Factor: " << metrics.improvementFactor << "x\n";
            std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
            
            spdlog::info("Performance monitoring displayed");
        } catch (const std::exception& e) {
            std::cout << "✗ Error getting performance metrics: " << e.what() << "\n";
            spdlog::error("Exception while getting performance metrics: {}", e.what());
        }

        std::cout << "\nPress Enter to continue...";
        std::cin.get();
    }

    void handleSystemStatus(UniversalVRConverter& converter) {
        std::cout << "\n=== System Status ===\n";
        
        try {
            std::cout << "System Status:\n";
            std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
            std::cout << "║ Initialized: " << (converter.isInitialized() ? "Yes" : "No") << "\n";
            std::cout << "║ Converting: " << (converter.isConverting() ? "Yes" : "No") << "\n";
            std::cout << "║ Supported Engines: " << converter.getSupportedEngines().size() << "\n";
            std::cout << "║ Loaded Plugins: " << converter.getLoadedPlugins().size() << "\n";
            std::cout << "║ Recent Errors: " << converter.getRecentErrors().size() << "\n";
            std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
            
            spdlog::info("System status displayed");
        } catch (const std::exception& e) {
            std::cout << "✗ Error getting system status: " << e.what() << "\n";
            spdlog::error("Exception while getting system status: {}", e.what());
        }

        std::cout << "\nPress Enter to continue...";
        std::cin.get();
    }

    void handleConfiguration(UniversalVRConverter& converter) {
        std::cout << "\n=== Configuration ===\n";
        std::cout << "Configuration options:\n";
        std::cout << "1. View current settings\n";
        std::cout << "2. Modify settings\n";
        std::cout << "3. Reset to defaults\n";
        std::cout << "Enter choice: ";
        
        std::string choice;
        std::getline(std::cin, choice);

        if (choice == "1") {
            try {
                auto settings = converter.getConversionSettings();
                std::cout << "\nCurrent Settings:\n";
                std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
                std::cout << "║ Stereoscopic Rendering: " << (settings.enableStereoscopicRendering ? "Enabled" : "Disabled") << "\n";
                std::cout << "║ Depth Buffer Extraction: " << (settings.enableDepthBufferExtraction ? "Enabled" : "Disabled") << "\n";
                std::cout << "║ Real Collision Detection: " << (settings.enableRealCollisionDetection ? "Enabled" : "Disabled") << "\n";
                std::cout << "║ Foveated Rendering: " << (settings.enableFoveatedRendering ? "Enabled" : "Disabled") << "\n";
                std::cout << "║ Async Timewarp: " << (settings.enableAsyncTimewarp ? "Enabled" : "Disabled") << "\n";
                std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
            } catch (const std::exception& e) {
                std::cout << "✗ Error getting settings: " << e.what() << "\n";
            }
        }
        else if (choice == "2") {
            std::cout << "Settings modification to be implemented...\n";
        }
        else if (choice == "3") {
            std::cout << "Settings reset to be implemented...\n";
        }
        else {
            std::cout << "Invalid choice.\n";
        }

        std::cout << "\nPress Enter to continue...";
        std::cin.get();
    }
};

/**
 * @brief Main entry point for uevrLCX Enhanced
 */
int main(int argc, char* argv[]) {
    try {
        std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
        std::cout << "║                    uevrLCX Enhanced                         ║\n";
        std::cout << "║              Universal VR Conversion System                 ║\n";
        std::cout << "║                                                              ║\n";
        std::cout << "║  Features:                                                   ║\n";
        std::cout << "║  • Universal game-to-VR conversion                          ║\n";
        std::cout << "║  • Real collision detection (not fake depth)                ║\n";
        std::cout << "║  • Reddit community solution integration                    ║\n";
        std::cout << "║  • Performance optimization (foveated rendering)            ║\n";
        std::cout << "║  • AI-powered game analysis                                 ║\n";
        std::cout << "║  • Multi-engine support (UE, Unity, custom, legacy)         ║\n";
        std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
        std::cout << "\n";

        uevrLCXEnhancedMain app;
        return app.run() ? 0 : 1;

    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
}
