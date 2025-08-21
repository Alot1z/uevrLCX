#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <thread>

#include "core/UEVRLCX_Core.h"
#include "core/SystemManager.h"
#include "core/ConfigurationManager.h"
#include "core/LoggingSystem.h"
#include "vr/VRSystem.h"
#include "context7/Context7AIDatabase.h"
#include "DeepWiki/DeepWikiConnector.h"

using namespace uevrLCX;

class uevrLCXApplication {
private:
    std::unique_ptr<SystemManager> systemManager;
    std::unique_ptr<ConfigurationManager> configManager;
    std::unique_ptr<LoggingSystem> loggingSystem;
    std::unique_ptr<VRSystem> vrSystem;
    std::unique_ptr<Context7AIDatabase> context7DB;
    std::unique_ptr<DeepWikiConnector> deepWikiConnector;
    
    bool isRunning = false;
    bool isInitialized = false;

public:
    uevrLCXApplication() = default;
    ~uevrLCXApplication() = default;

    bool initialize() {
        try {
            std::cout << "=== uevrLCX VR Conversion System ===" << std::endl;
            std::cout << "Initializing system..." << std::endl;

            // Initialize logging first
            loggingSystem = std::make_unique<LoggingSystem>();
            loggingSystem->initialize();
            loggingSystem->log(LogLevel::INFO, "uevrLCX starting up...");

            // Initialize configuration
            configManager = std::make_unique<ConfigurationManager>();
            if (!configManager->loadConfiguration("config/uevrlcx.ini")) {
                loggingSystem->log(LogLevel::WARNING, "Failed to load configuration, using defaults");
                configManager->createDefaultConfiguration();
            }

            // Initialize system manager
            systemManager = std::make_unique<SystemManager>();
            if (!systemManager->initialize()) {
                loggingSystem->log(LogLevel::ERROR, "Failed to initialize system manager");
                return false;
            }

            // Initialize VR system
            vrSystem = std::make_unique<VRSystem>();
            if (!vrSystem->initialize()) {
                loggingSystem->log(LogLevel::ERROR, "Failed to initialize VR system");
                return false;
            }

            // Initialize Context7 AI database
            context7DB = std::make_unique<Context7AIDatabase>();
            if (!context7DB->initialize()) {
                loggingSystem->log(LogLevel::WARNING, "Failed to initialize Context7 database");
            }

            // Initialize DeepWiki connector
            deepWikiConnector = std::make_unique<DeepWikiConnector>();
            if (!deepWikiConnector->initialize()) {
                loggingSystem->log(LogLevel::WARNING, "Failed to initialize DeepWiki connector");
            }

            isInitialized = true;
            loggingSystem->log(LogLevel::INFO, "uevrLCX initialized successfully");
            std::cout << "System initialized successfully!" << std::endl;
            return true;

        } catch (const std::exception& e) {
            std::cerr << "Initialization error: " << e.what() << std::endl;
            return false;
        }
    }

    void run() {
        if (!isInitialized) {
            std::cerr << "System not initialized!" << std::endl;
            return;
        }

        isRunning = true;
        loggingSystem->log(LogLevel::INFO, "Starting uevrLCX main loop");

        std::cout << "uevrLCX is running. Press Ctrl+C to exit." << std::endl;
        std::cout << "Available commands:" << std::endl;
        std::cout << "  help     - Show available commands" << std::endl;
        std::cout << "  status   - Show system status" << std::endl;
        std::cout << "  scan     - Scan for compatible games" << std::endl;
        std::cout << "  convert  - Convert a game to VR" << std::endl;
        std::cout << "  exit     - Exit uevrLCX" << std::endl;

        while (isRunning) {
            try {
                // Process system events
                systemManager->update();

                // Update VR system
                vrSystem->update();

                // Process user input
                processUserInput();

                // Sleep to prevent high CPU usage
                std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS

            } catch (const std::exception& e) {
                loggingSystem->log(LogLevel::ERROR, "Runtime error: " + std::string(e.what()));
                std::cerr << "Runtime error: " << e.what() << std::endl;
            }
        }
    }

    void shutdown() {
        if (!isInitialized) return;

        loggingSystem->log(LogLevel::INFO, "Shutting down uevrLCX...");
        std::cout << "Shutting down..." << std::endl;

        isRunning = false;

        // Shutdown components in reverse order
        if (deepWikiConnector) {
            deepWikiConnector->shutdown();
        }

        if (context7DB) {
            context7DB->shutdown();
        }

        if (vrSystem) {
            vrSystem->shutdown();
        }

        if (systemManager) {
            systemManager->shutdown();
        }

        if (loggingSystem) {
            loggingSystem->log(LogLevel::INFO, "uevrLCX shutdown complete");
            loggingSystem->shutdown();
        }

        std::cout << "Shutdown complete." << std::endl;
    }

private:
    void processUserInput() {
        // This is a simple console input handler
        // In a real application, this would be more sophisticated
        if (std::cin.rdbuf()->in_avail()) {
            std::string command;
            std::getline(std::cin, command);
            
            if (command == "help") {
                showHelp();
            } else if (command == "status") {
                showStatus();
            } else if (command == "scan") {
                scanForGames();
            } else if (command == "convert") {
                convertGame();
            } else if (command == "exit") {
                isRunning = false;
            } else if (!command.empty()) {
                std::cout << "Unknown command: " << command << std::endl;
                std::cout << "Type 'help' for available commands." << std::endl;
            }
        }
    }

    void showHelp() {
        std::cout << "\n=== uevrLCX Help ===" << std::endl;
        std::cout << "help     - Show this help message" << std::endl;
        std::cout << "status   - Show system status and performance" << std::endl;
        std::cout << "scan     - Scan for compatible games on the system" << std::endl;
        std::cout << "convert  - Start VR conversion process for a game" << std::endl;
        std::cout << "exit     - Exit uevrLCX" << std::endl;
        std::cout << "==================\n" << std::endl;
    }

    void showStatus() {
        std::cout << "\n=== System Status ===" << std::endl;
        std::cout << "System Manager: " << (systemManager ? "Running" : "Stopped") << std::endl;
        std::cout << "VR System: " << (vrSystem ? "Running" : "Stopped") << std::endl;
        std::cout << "Context7 DB: " << (context7DB ? "Connected" : "Disconnected") << std::endl;
        std::cout << "DeepWiki: " << (deepWikiConnector ? "Connected" : "Disconnected") << std::endl;
        std::cout << "==================\n" << std::endl;
    }

    void scanForGames() {
        std::cout << "\nScanning for compatible games..." << std::endl;
        // This would integrate with the game detection system
        std::cout << "Game scanning not yet implemented." << std::endl;
        std::cout << "This feature will detect and list compatible games.\n" << std::endl;
    }

    void convertGame() {
        std::cout << "\nVR Conversion Process" << std::endl;
        std::cout << "Please select a game to convert:" << std::endl;
        // This would show a list of detected games
        std::cout << "Game conversion not yet implemented." << std::endl;
        std::cout << "This feature will convert selected games to VR.\n" << std::endl;
    }
};

int main(int argc, char* argv[]) {
    // Parse command line arguments
    std::vector<std::string> args(argv + 1, argv + argc);
    
    bool showHelp = false;
    bool runTests = false;
    bool verbose = false;

    for (const auto& arg : args) {
        if (arg == "--help" || arg == "-h") {
            showHelp = true;
        } else if (arg == "--test" || arg == "-t") {
            runTests = true;
        } else if (arg == "--verbose" || arg == "-v") {
            verbose = true;
        }
    }

    if (showHelp) {
        std::cout << "uevrLCX - Universal VR Conversion System" << std::endl;
        std::cout << "Usage: uevrLCX [options]" << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  --help, -h     Show this help message" << std::endl;
        std::cout << "  --test, -t     Run tests" << std::endl;
        std::cout << "  --verbose, -v  Enable verbose output" << std::endl;
        return 0;
    }

    if (runTests) {
        std::cout << "Running tests..." << std::endl;
        // This would run the test suite
        std::cout << "Test system not yet implemented." << std::endl;
        return 0;
    }

    // Create and run the application
    uevrLCXApplication app;

    if (!app.initialize()) {
        std::cerr << "Failed to initialize uevrLCX!" << std::endl;
        return 1;
    }

    // Set up signal handlers for graceful shutdown
    signal(SIGINT, [](int) {
        // This would trigger shutdown
        std::cout << "\nReceived interrupt signal. Shutting down..." << std::endl;
    });

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }

    app.shutdown();
    return 0;
}
