#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>

namespace uevr {
namespace vr {

// High-level input sources
enum class InputSource {
    HMD = 0,
    LEFT_CONTROLLER,
    RIGHT_CONTROLLER,
    GAMEPAD,
    KEYBOARD_MOUSE,
    CUSTOM
};

// Simple pose structure
struct Pose {
    float position[3]{0,0,0};
    float rotation[4]{0,0,0,1}; // quaternion
    bool valid{false};
};

// Action types
enum class ActionType { BUTTON, AXIS1D, AXIS2D, POSE };

struct ActionBinding {
    std::string actionName;   // e.g., "trigger", "grab", "teleport"
    ActionType type{ActionType::BUTTON};
    InputSource source{InputSource::RIGHT_CONTROLLER};
    std::string hardwarePath; // runtime-specific path (e.g., OpenXR /user/hand/right/input/trigger)
};

class IInputMapper {
public:
    virtual ~IInputMapper() = default;

    // Lifecycle
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    virtual bool isInitialized() const = 0;

    // Bindings
    virtual bool addBinding(const ActionBinding& binding) = 0;
    virtual bool removeBinding(const std::string& actionName) = 0;
    virtual void clearBindings() = 0;

    // Poll/update
    virtual void update(double deltaSeconds) = 0;

    // Queries
    virtual bool getButton(const std::string& actionName) const = 0;
    virtual float getAxis1D(const std::string& actionName) const = 0;
    virtual std::pair<float,float> getAxis2D(const std::string& actionName) const = 0;
    virtual Pose getPose(InputSource source) const = 0;

    // Haptics
    virtual bool triggerHaptic(InputSource source, float durationSeconds, float frequency, float amplitude) = 0;
};

std::unique_ptr<IInputMapper> CreateInputMapper();

} // namespace vr
} // namespace uevr
