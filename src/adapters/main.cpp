#include "common/CrossEngineAdapterFactory.hpp"
#include "common/BaseCrossEngineAdapter.hpp"
#include <iostream>
#include <memory>
#include <string>

using namespace uevr;

/**
 * @brief Main entry point for uevr Cross-Engine VR Adapters
 * 
 * Demonstrates the cross-engine adapter system and provides testing capabilities
 */
int main() {
    std::cout << "uevr Cross-Engine VR Adapter Demo" << std::endl;
    std::cout << "=================================" << std::endl;
    
    try {
        // Create factory
        CrossEngineAdapterFactory factory;
        
        // Test adapter creation for different games
        std::vector<std::string> testGames = {
            "re7.exe",
            "Cyberpunk2077.exe", 
            "MonsterHunterWorld.exe",
            "unknown_game.exe"
        };
        
        for (const auto& game : testGames) {
            std::cout << "\nTesting game: " << game << std::endl;
            
            auto adapter = factory.createAdapter(game);
            if (adapter) {
                std::cout << "  ✓ Adapter created successfully" << std::endl;
                std::cout << "  - Name: " << adapter->getName() << std::endl;
                std::cout << "  - Engine: " << adapter->getEngineType() << std::endl;
                std::cout << "  - Version: " << adapter->getVersion() << std::endl;
                
                // Test adapter lifecycle
                std::cout << "  - Testing initialization..." << std::endl;
                if (adapter->initialize()) {
                    std::cout << "    ✓ Initialization successful" << std::endl;
                    
                    // Test update and render
                    std::cout << "  - Testing update/render..." << std::endl;
                    adapter->update();
                    adapter->render();
                    std::cout << "    ✓ Update/render successful" << std::endl;
                    
                    // Test cleanup
                    std::cout << "  - Testing cleanup..." << std::endl;
                    adapter->cleanup();
                    std::cout << "    ✓ Cleanup successful" << std::endl;
                    
                } else {
                    std::cout << "    ✗ Initialization failed" << std::endl;
                }
                
            } else {
                std::cout << "  ✗ Failed to create adapter" << std::endl;
            }
        }
        
        std::cout << "\nDemo completed successfully!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
