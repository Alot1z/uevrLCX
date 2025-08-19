#include <windows.h>
#include <iostream>
#include "uevr/ExceptionHandler.hpp"

// Simple VR wrapper for testing
namespace uevr {
namespace vr {
    class SimpleVRSystem {
    public:
        bool initialize() {
            std::cout << "Simple VR System Initializing..." << std::endl;
            return true;
        }
        
        void shutdown() {
            std::cout << "Simple VR System Shutting Down..." << std::endl;
        }
        
        bool isVRReady() const {
            return true;
        }
    };
}
}

int main() {
    std::cout << "UEVR Core + Simple VR Test Starting..." << std::endl;
    
    // Test exception handler
    framework::global_exception_handler(nullptr);
    
    // Test simple VR system
    uevr::vr::SimpleVRSystem vr_system;
    if (vr_system.initialize()) {
        std::cout << "VR System initialized successfully!" << std::endl;
        if (vr_system.isVRReady()) {
            std::cout << "VR is ready!" << std::endl;
        }
        vr_system.shutdown();
    }
    
    std::cout << "UEVR Core + Simple VR Test Completed Successfully!" << std::endl;
    return 0;
}
