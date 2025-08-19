#include "uevr/Plugin.hpp"
#include <iostream>

/**
 * @brief Basic uevr Plugin Example
 * 
 * This example demonstrates how to create a basic uevr plugin
 * that can be loaded by the framework.
 */

int main() {
    std::cout << "=== uevr Basic Plugin Example ===" << std::endl;
    
    try {
        // Create plugin instance
        auto plugin = std::make_unique<uevr::Plugin>();
        
        // Initialize the plugin
        if (!plugin->initialize()) {
            std::cerr << "Failed to initialize plugin" << std::endl;
            return 1;
        }
        
        std::cout << "Plugin initialized successfully!" << std::endl;
        
        // Get framework reference
        auto framework = plugin->getFramework();
        if (framework) {
            std::cout << "Framework reference obtained" << std::endl;
        }
        
        // Cleanup
        plugin->cleanup();
        std::cout << "Plugin cleanup completed" << std::endl;
        
        std::cout << "Example completed successfully!" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
}
