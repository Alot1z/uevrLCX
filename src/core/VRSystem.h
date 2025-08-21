#pragma once

#include <string>
#include <memory>

namespace uevrLCX {
namespace core {

enum class VRSystemState {
    STOPPED,
    INITIALIZED,
    RUNNING,
    PAUSED,
    ERROR
};

class VRSystem {
public:
    VRSystem();
    ~VRSystem();

    // Core VR system functionality
    bool initialize();
    bool start();
    bool stop();
    bool pause();
    bool resume();
    
    // State management
    VRSystemState getState() const;
    std::string getStateString() const;
    bool isActive() const;
    
    // Cleanup
    bool shutdown();

private:
    class VRSystemImpl;
    std::unique_ptr<VRSystemImpl> impl;
};

} // namespace core
} // namespace uevrLCX
