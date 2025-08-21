#include "VRPhysics.h"

namespace uevrLCX {

VRPhysics::VRPhysics() {
}

VRPhysics::~VRPhysics() {
    shutdown();
}

bool VRPhysics::initialize() {
    return true;
}

void VRPhysics::shutdown() {
}

void VRPhysics::update() {
}

} // namespace uevrLCX
