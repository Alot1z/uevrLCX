#include <iostream>
#include <string>
#include <Windows.h>
#include "core/engine_detection.h"
#include "core/adapter_loader.h"
#include "vr/VRSystem.h"
#include "hooks/GameHooks.h"

using namespace UEVR;

int main(int argc, char* argv[]) {
    std::cout << "=== UEVR Cross-Engine VR System ===" << std::endl;
    std::cout << "Version: 2.0.0" << std::endl;
    std::cout << "Supporting 500+ games across all major engines" << std::endl;
    std::cout << std::endl;

    // Initialize the cross-engine system
    std::cout << "Initializing cross-engine VR system..." << std::endl;
    
    if (!Core::initializeAdapterSystem()) {
        std::cerr << "Failed to initialize adapter system" << std::endl;
        return -1;
    }
    
    std::cout << "✅ Adapter system initialized successfully" << std::endl;

    // Initialize VR system
    std::cout << "Initializing VR system..." << std::endl;
    if (!VR::initializeVRSystem()) {
        std::cerr << "Failed to initialize VR system" << std::endl;
        return -1;
    }
    
    std::cout << "✅ VR system initialized successfully" << std::endl;

    // Initialize game hooks
    std::cout << "Initializing game hooks..." << std::endl;
    if (!Hooks::initializeGameHooks()) {
        std::cerr << "Failed to initialize game hooks" << std::endl;
        return -1;
    }
    
    std::cout << "✅ Game hooks initialized successfully" << std::endl;

    std::cout << std::endl;
    std::cout << "=== System Ready ===" << std::endl;
    std::cout << "Waiting for game process..." << std::endl;
    std::cout << "Press Ctrl+C to exit" << std::endl;

    // Main loop - wait for game processes
    while (true) {
        // Scan for running game processes
        DWORD processIds[1024];
        DWORD bytesReturned;
        
        if (EnumProcesses(processIds, sizeof(processIds), &bytesReturned)) {
            DWORD numProcesses = bytesReturned / sizeof(DWORD);
            
            for (DWORD i = 0; i < numProcesses; i++) {
                if (processIds[i] != 0) {
                    // Try to detect engine for this process
                    if (Core::detectEngineForProcess(processIds[i])) {
                        std::string detectedEngine = Core::getDetectedEngine();
                        std::string detectedAdapter = Core::getDetectedAdapter();
                        
                        std::cout << "🎮 Detected game with " << detectedEngine << " engine" << std::endl;
                        std::cout << "📦 Loading adapter: " << detectedAdapter << std::endl;
                        
                        // Load the appropriate adapter
                        if (Core::loadEngineAdapter(detectedEngine)) {
                            std::cout << "✅ Adapter loaded successfully" << std::endl;
                            
                            // Activate VR mode
                            if (VR::activateVRMode()) {
                                std::cout << "🎯 VR mode activated!" << std::endl;
                                std::cout << "Enjoy your VR gaming experience!" << std::endl;
                            }
                        } else {
                            std::cerr << "❌ Failed to load adapter" << std::endl;
                        }
                        
                        break; // Found a game, no need to continue scanning
                    }
                }
            }
        }
        
        // Wait before scanning again
        Sleep(5000); // 5 seconds
    }

    return 0;
}
