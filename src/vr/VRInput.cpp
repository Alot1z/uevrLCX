#include "VRInput.h"
#include <chrono>
#include <algorithm>

namespace uevrLCX {

VRInput::VRInput() {
}

VRInput::~VRInput() {
    shutdown();
}

bool VRInput::initialize() {
    try {
        inputs.clear();
        inputMappings.clear();
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

void VRInput::shutdown() {
    std::lock_guard<std::mutex> lock(inputMutex);
    inputs.clear();
    inputMappings.clear();
}

void VRInput::update() {
    std::lock_guard<std::mutex> lock(inputMutex);
    
    // Update input states
    for (auto& pair : inputs) {
        auto& inputInfo = pair.second;
        inputInfo.previousData = inputInfo.currentData;
    }
}

void VRInput::registerCallback(const std::string& inputName, InputCallback callback) {
    std::lock_guard<std::mutex> lock(inputMutex);
    
    if (inputs.find(inputName) == inputs.end()) {
        inputs[inputName] = InputInfo{};
    }
    
    inputs[inputName].callbacks.push_back(std::move(callback));
}

void VRInput::unregisterCallback(const std::string& inputName, size_t callbackId) {
    std::lock_guard<std::mutex> lock(inputMutex);
    
    auto it = inputs.find(inputName);
    if (it != inputs.end() && callbackId < it->second.callbacks.size()) {
        it->second.callbacks.erase(it->second.callbacks.begin() + callbackId);
    }
}

bool VRInput::isPressed(const std::string& inputName) const {
    std::lock_guard<std::mutex> lock(inputMutex);
    
    auto it = inputs.find(inputName);
    if (it != inputs.end()) {
        return it->second.currentData.state == InputState::PRESSED;
    }
    return false;
}

bool VRInput::isHeld(const std::string& inputName) const {
    std::lock_guard<std::mutex> lock(inputMutex);
    
    auto it = inputs.find(inputName);
    if (it != inputs.end()) {
        return it->second.currentData.state == InputState::HELD;
    }
    return false;
}

bool VRInput::isReleased(const std::string& inputName) const {
    std::lock_guard<std::mutex> lock(inputMutex);
    
    auto it = inputs.find(inputName);
    if (it != inputs.end()) {
        return it->second.currentData.state == InputState::RELEASED;
    }
    return false;
}

float VRInput::getValue(const std::string& inputName) const {
    std::lock_guard<std::mutex> lock(inputMutex);
    
    auto it = inputs.find(inputName);
    if (it != inputs.end()) {
        return it->second.currentData.value;
    }
    return 0.0f;
}

void VRInput::getPosition(const std::string& inputName, float& x, float& y) const {
    std::lock_guard<std::mutex> lock(inputMutex);
    
    auto it = inputs.find(inputName);
    if (it != inputs.end()) {
        x = it->second.currentData.x;
        y = it->second.currentData.y;
    } else {
        x = y = 0.0f;
    }
}

void VRInput::simulateInput(const std::string& inputName, const InputData& data) {
    std::lock_guard<std::mutex> lock(inputMutex);
    
    if (inputs.find(inputName) == inputs.end()) {
        inputs[inputName] = InputInfo{};
    }
    
    InputData processedData = data;
    processedData.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    
    processInput(inputName, processedData);
}

void VRInput::simulateButtonPress(const std::string& inputName) {
    InputData data;
    data.type = InputType::BUTTON;
    data.state = InputState::PRESSED;
    data.value = 1.0f;
    
    simulateInput(inputName, data);
}

void VRInput::simulateButtonRelease(const std::string& inputName) {
    InputData data;
    data.type = InputType::BUTTON;
    data.state = InputState::RELEASED;
    data.value = 0.0f;
    
    simulateInput(inputName, data);
}

void VRInput::simulateAxis(const std::string& inputName, float x, float y) {
    InputData data;
    data.type = InputType::AXIS;
    data.state = InputState::HELD;
    data.x = x;
    data.y = y;
    data.value = std::sqrt(x * x + y * y);
    
    simulateInput(inputName, data);
}

void VRInput::simulateTrigger(const std::string& inputName, float value) {
    InputData data;
    data.type = InputType::TRIGGER;
    data.state = InputState::HELD;
    data.value = std::max(0.0f, std::min(1.0f, value));
    
    simulateInput(inputName, data);
}

bool VRInput::mapInput(const std::string& sourceName, const std::string& targetName) {
    std::lock_guard<std::mutex> lock(inputMutex);
    
    InputMapping mapping;
    mapping.sourceName = sourceName;
    mapping.targetName = targetName;
    
    inputMappings.push_back(mapping);
    return true;
}

bool VRInput::unmapInput(const std::string& sourceName) {
    std::lock_guard<std::mutex> lock(inputMutex);
    
    auto it = std::remove_if(inputMappings.begin(), inputMappings.end(),
        [&sourceName](const InputMapping& mapping) {
            return mapping.sourceName == sourceName;
        });
    
    inputMappings.erase(it, inputMappings.end());
    return true;
}

std::vector<std::string> VRInput::getMappedInputs(const std::string& sourceName) const {
    std::lock_guard<std::mutex> lock(inputMutex);
    
    std::vector<std::string> result;
    for (const auto& mapping : inputMappings) {
        if (mapping.sourceName == sourceName) {
            result.push_back(mapping.targetName);
        }
    }
    
    return result;
}

bool VRInput::setDeadzone(const std::string& inputName, float deadzone) {
    std::lock_guard<std::mutex> lock(inputMutex);
    
    if (inputs.find(inputName) == inputs.end()) {
        inputs[inputName] = InputInfo{};
    }
    
    inputs[inputName].deadzone = std::max(0.0f, std::min(1.0f, deadzone));
    return true;
}

bool VRInput::setSensitivity(const std::string& inputName, float sensitivity) {
    std::lock_guard<std::mutex> lock(inputMutex);
    
    if (inputs.find(inputName) == inputs.end()) {
        inputs[inputName] = InputInfo{};
    }
    
    inputs[inputName].sensitivity = std::max(0.1f, sensitivity);
    return true;
}

bool VRInput::setInvert(const std::string& inputName, bool invert) {
    std::lock_guard<std::mutex> lock(inputMutex);
    
    if (inputs.find(inputName) == inputs.end()) {
        inputs[inputName] = InputInfo{};
    }
    
    inputs[inputName].invert = invert;
    return true;
}

size_t VRInput::getInputCount() const {
    std::lock_guard<std::mutex> lock(inputMutex);
    return inputs.size();
}

size_t VRInput::getCallbackCount() const {
    std::lock_guard<std::mutex> lock(inputMutex);
    
    size_t count = 0;
    for (const auto& pair : inputs) {
        count += pair.second.callbacks.size();
    }
    return count;
}

std::vector<std::string> VRInput::getActiveInputs() const {
    std::lock_guard<std::mutex> lock(inputMutex);
    
    std::vector<std::string> result;
    for (const auto& pair : inputs) {
        if (pair.second.currentData.state != InputState::RELEASED) {
            result.push_back(pair.first);
        }
    }
    
    return result;
}

void VRInput::processInput(const std::string& inputName, const InputData& data) {
    auto& inputInfo = inputs[inputName];
    
    // Apply processing
    InputData processedData = data;
    
    // Apply deadzone
    if (applyDeadzone(processedData.value, inputInfo.deadzone)) {
        processedData.value = 0.0f;
    }
    
    // Apply sensitivity
    processedData.value = applySensitivity(processedData.value, inputInfo.sensitivity);
    processedData.x = applySensitivity(processedData.x, inputInfo.sensitivity);
    processedData.y = applySensitivity(processedData.y, inputInfo.sensitivity);
    
    // Apply invert
    processedData.value = applyInvert(processedData.value, inputInfo.invert);
    processedData.x = applyInvert(processedData.x, inputInfo.invert);
    processedData.y = applyInvert(processedData.y, inputInfo.invert);
    
    // Update state
    inputInfo.previousData = inputInfo.currentData;
    inputInfo.currentData = processedData;
    
    // Handle state transitions
    if (inputInfo.previousData.state == InputState::RELEASED && 
        processedData.state == InputState::PRESSED) {
        processedData.state = InputState::PRESSED;
    } else if (inputInfo.previousData.state == InputState::PRESSED && 
               processedData.state == InputState::PRESSED) {
        processedData.state = InputState::HELD;
    }
    
    // Notify callbacks
    notifyCallbacks(inputName, processedData);
    
    // Process mappings
    for (const auto& mapping : inputMappings) {
        if (mapping.sourceName == inputName) {
            processInput(mapping.targetName, processedData);
        }
    }
}

void VRInput::notifyCallbacks(const std::string& inputName, const InputData& data) {
    auto it = inputs.find(inputName);
    if (it != inputs.end()) {
        for (const auto& callback : it->second.callbacks) {
            try {
                callback(data);
            } catch (...) {
                // Ignore callback errors
            }
        }
    }
}

bool VRInput::applyDeadzone(float& value, float deadzone) const {
    if (std::abs(value) < deadzone) {
        return true; // Value is in deadzone
    }
    return false;
}

float VRInput::applySensitivity(float value, float sensitivity) const {
    return value * sensitivity;
}

float VRInput::applyInvert(float value, bool invert) const {
    return invert ? -value : value;
}

} // namespace uevrLCX
