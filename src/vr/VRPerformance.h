#pragma once

namespace uevrLCX {

class VRPerformance {
public:
    VRPerformance();
    ~VRPerformance();
    bool initialize();
    void shutdown();
    void update();
};

} // namespace uevrLCX
