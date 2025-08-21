#pragma once

#include <string>
#include <vector>
#include <memory>

namespace uevrLCX {
namespace core {

struct VRConfiguration {
    struct Resolution {
        int width;
        int height;
    };
    
    Resolution resolution;
    int refreshRate;
    float fov;
    bool enablePhysics;
    bool enableHaptics;
};

class EngineAdapter {
public:
    EngineAdapter();
    ~EngineAdapter();

    // Core functionality
    bool initialize();
    bool injectVR(const std::string& gamePath, const VRConfiguration& config);
    bool configureVR(const std::string& gamePath, const VRConfiguration& config);
    bool removeVR(const std::string& gamePath);
    
    // Engine support
    std::vector<std::string> getSupportedEngines();
    bool isEngineSupported(const std::string& engineType);
    VRConfiguration getDefaultConfig(const std::string& engineType);
    
    // Cleanup
    bool shutdown();

private:
    class EngineAdapterImpl;
    std::unique_ptr<EngineAdapterImpl> impl;
};

} // namespace core
} // namespace uevrLCX
