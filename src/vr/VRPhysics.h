#pragma once

#include <memory>
#include <vector>
#include <string>

namespace uevrLCX {

class VRPhysics {
public:
    VRPhysics();
    ~VRPhysics();

    bool initialize();
    void shutdown();
    void update();
};

} // namespace uevrLCX
