#pragma once

#include <memory>
#include <vector>
#include <string>
#include <functional>
#include <unordered_map>
#include <mutex>

namespace uevrLCX {

enum class InputType {
    BUTTON,
    AXIS,
    TRIGGER,
    TOUCHPAD,
    JOYSTICK,
    GRIP,
    MENU,
    SYSTEM
};

enum class InputState {
    RELEASED,
    PRESSED,
    HELD,
    TOUCHED
};

struct InputData {
    InputType type;
    InputState state;
    float value;
    float x, y;
    uint64_t timestamp;
    
    InputData() : type(InputType::BUTTON), state(InputState::RELEASED), 
                  value(0.0f), x(0.0f), y(0.0f), timestamp(0) {}
};

using InputCallback = std::function<void(const InputData&)>;

class VRInput {
public:
    VRInput();
    ~VRInput();

    // Core functionality
    bool initialize();
    void shutdown();
    void update();
    
    // Input registration
    void registerCallback(const std::string& inputName, InputCallback callback);
    void unregisterCallback(const std::string& inputName, size_t callbackId);
    
    // Input querying
    bool isPressed(const std::string& inputName) const;
    bool isHeld(const std::string& inputName) const;
    bool isReleased(const std::string& inputName) const;
    float getValue(const std::string& inputName) const;
    void getPosition(const std::string& inputName, float& x, float& y) const;
    
    // Input simulation
    void simulateInput(const std::string& inputName, const InputData& data);
    void simulateButtonPress(const std::string& inputName);
    void simulateButtonRelease(const std::string& inputName);
    void simulateAxis(const std::string& inputName, float x, float y);
    void simulateTrigger(const std::string& inputName, float value);
    
    // Input mapping
    bool mapInput(const std::string& sourceName, const std::string& targetName);
    bool unmapInput(const std::string& sourceName);
    std::vector<std::string> getMappedInputs(const std::string& sourceName) const;
    
    // Input configuration
    bool setDeadzone(const std::string& inputName, float deadzone);
    bool setSensitivity(const std::string& inputName, float sensitivity);
    bool setInvert(const std::string& inputName, bool invert);
    
    // Statistics
    size_t getInputCount() const;
    size_t getCallbackCount() const;
    std::vector<std::string> getActiveInputs() const;

private:
    struct InputInfo {
        InputData currentData;
        InputData previousData;
        float deadzone;
        float sensitivity;
        bool invert;
        std::vector<InputCallback> callbacks;
    };

    struct InputMapping {
        std::string sourceName;
        std::string targetName;
    };

    // Input storage
    std::unordered_map<std::string, InputInfo> inputs;
    std::vector<InputMapping> inputMappings;
    
    // Thread safety
    mutable std::mutex inputMutex;
    
    // Internal methods
    void processInput(const std::string& inputName, const InputData& data);
    void notifyCallbacks(const std::string& inputName, const InputData& data);
    bool applyDeadzone(float& value, float deadzone) const;
    float applySensitivity(float value, float sensitivity) const;
    float applyInvert(float value, bool invert) const;
};

} // namespace uevrLCX
