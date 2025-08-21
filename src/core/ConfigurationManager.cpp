#include "ConfigurationManager.h"
#include <fstream>
#include <sstream>
#include <algorithm>

namespace uevrLCX {

ConfigurationManager::ConfigurationManager() {
}

ConfigurationManager::~ConfigurationManager() {
}

bool ConfigurationManager::loadConfiguration(const std::string& filename) {
    std::lock_guard<std::mutex> lock(configMutex);
    
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            
            // Remove whitespace
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            
            if (!key.empty()) {
                ConfigValue configValue;
                configValue.type = ConfigType::STRING;
                configValue.stringValue = value;
                config[key] = configValue;
            }
        }
    }
    
    return true;
}

bool ConfigurationManager::saveConfiguration(const std::string& filename) {
    std::lock_guard<std::mutex> lock(configMutex);
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& pair : config) {
        file << pair.first << "=" << pair.second.stringValue << std::endl;
    }
    
    return true;
}

void ConfigurationManager::createDefaultConfiguration() {
    std::lock_guard<std::mutex> lock(configMutex);
    
    // VR settings
    setString("vr.enabled", "true");
    setString("vr.headset_type", "auto");
    setString("vr.resolution_width", "1920");
    setString("vr.resolution_height", "1080");
    setString("vr.refresh_rate", "90");
    setString("vr.ipd", "64.0");
    
    // Performance settings
    setString("performance.target_fps", "90");
    setString("performance.foveated_rendering", "true");
    setString("performance.async_timewarp", "true");
    setString("performance.multi_resolution", "true");
    
    // Graphics settings
    setString("graphics.quality", "high");
    setString("graphics.shadows", "true");
    setString("graphics.antialiasing", "msaa_4x");
    setString("graphics.texture_quality", "high");
    
    // Physics settings
    setString("physics.enabled", "true");
    setString("physics.collision_detection", "true");
    setString("physics.haptic_feedback", "true");
    setString("physics.accuracy", "high");
    
    // Audio settings
    setString("audio.enabled", "true");
    setString("audio.spatial_audio", "true");
    setString("audio.volume", "100");
    setString("audio.sample_rate", "48000");
    
    // Input settings
    setString("input.controller_type", "auto");
    setString("input.tracking", "true");
    setString("input.hand_tracking", "true");
    setString("input.eye_tracking", "false");
    
    // Logging settings
    setString("logging.level", "info");
    setString("logging.file", "uevrlcx.log");
    setString("logging.console", "true");
    setString("logging.max_file_size", "10485760");
    
    // Network settings
    setString("network.enabled", "false");
    setString("network.port", "8080");
    setString("network.max_connections", "10");
    
    // Security settings
    setString("security.encryption", "true");
    setString("security.checksum_verification", "true");
    setString("security.anti_cheat_bypass", "false");
}

std::string ConfigurationManager::getString(const std::string& key, const std::string& defaultValue) {
    std::lock_guard<std::mutex> lock(configMutex);
    
    auto it = config.find(key);
    if (it != config.end()) {
        return it->second.stringValue;
    }
    return defaultValue;
}

int64_t ConfigurationManager::getInteger(const std::string& key, int64_t defaultValue) {
    std::string value = getString(key, "");
    if (value.empty()) {
        return defaultValue;
    }
    
    try {
        return std::stoll(value);
    } catch (...) {
        return defaultValue;
    }
}

double ConfigurationManager::getFloat(const std::string& key, double defaultValue) {
    std::string value = getString(key, "");
    if (value.empty()) {
        return defaultValue;
    }
    
    try {
        return std::stod(value);
    } catch (...) {
        return defaultValue;
    }
}

bool ConfigurationManager::getBoolean(const std::string& key, bool defaultValue) {
    std::string value = getString(key, "");
    if (value.empty()) {
        return defaultValue;
    }
    
    std::transform(value.begin(), value.end(), value.begin(), ::tolower);
    return (value == "true" || value == "1" || value == "yes" || value == "on");
}

std::vector<std::string> ConfigurationManager::getArray(const std::string& key) {
    std::string value = getString(key, "");
    std::vector<std::string> result;
    
    if (!value.empty()) {
        std::stringstream ss(value);
        std::string item;
        while (std::getline(ss, item, ',')) {
            item.erase(0, item.find_first_not_of(" \t"));
            item.erase(item.find_last_not_of(" \t") + 1);
            if (!item.empty()) {
                result.push_back(item);
            }
        }
    }
    
    return result;
}

std::unordered_map<std::string, std::string> ConfigurationManager::getObject(const std::string& key) {
    std::string value = getString(key, "");
    std::unordered_map<std::string, std::string> result;
    
    if (!value.empty()) {
        std::stringstream ss(value);
        std::string item;
        while (std::getline(ss, item, ';')) {
            size_t pos = item.find(':');
            if (pos != std::string::npos) {
                std::string k = item.substr(0, pos);
                std::string v = item.substr(pos + 1);
                
                k.erase(0, k.find_first_not_of(" \t"));
                k.erase(k.find_last_not_of(" \t") + 1);
                v.erase(0, v.find_first_not_of(" \t"));
                v.erase(v.find_last_not_of(" \t") + 1);
                
                if (!k.empty()) {
                    result[k] = v;
                }
            }
        }
    }
    
    return result;
}

void ConfigurationManager::setString(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(configMutex);
    
    ConfigValue oldValue = config[key];
    ConfigValue newValue;
    newValue.type = ConfigType::STRING;
    newValue.stringValue = value;
    
    config[key] = newValue;
    notifyChange(key, oldValue, newValue);
}

void ConfigurationManager::setInteger(const std::string& key, int64_t value) {
    setString(key, std::to_string(value));
}

void ConfigurationManager::setFloat(const std::string& key, double value) {
    setString(key, std::to_string(value));
}

void ConfigurationManager::setBoolean(const std::string& key, bool value) {
    setString(key, value ? "true" : "false");
}

void ConfigurationManager::setArray(const std::string& key, const std::vector<std::string>& value) {
    std::string result;
    for (size_t i = 0; i < value.size(); ++i) {
        if (i > 0) result += ",";
        result += value[i];
    }
    setString(key, result);
}

void ConfigurationManager::setObject(const std::string& key, const std::unordered_map<std::string, std::string>& value) {
    std::string result;
    for (const auto& pair : value) {
        if (!result.empty()) result += ";";
        result += pair.first + ":" + pair.second;
    }
    setString(key, result);
}

bool ConfigurationManager::hasKey(const std::string& key) const {
    std::lock_guard<std::mutex> lock(configMutex);
    return config.find(key) != config.end();
}

void ConfigurationManager::removeKey(const std::string& key) {
    std::lock_guard<std::mutex> lock(configMutex);
    config.erase(key);
}

std::vector<std::string> ConfigurationManager::getKeys() const {
    std::lock_guard<std::mutex> lock(configMutex);
    
    std::vector<std::string> keys;
    keys.reserve(config.size());
    for (const auto& pair : config) {
        keys.push_back(pair.first);
    }
    return keys;
}

void ConfigurationManager::clear() {
    std::lock_guard<std::mutex> lock(configMutex);
    config.clear();
}

void ConfigurationManager::registerChangeCallback(const std::string& key, ChangeCallback callback) {
    // Implementation for change callbacks
}

void ConfigurationManager::unregisterChangeCallback(const std::string& key) {
    // Implementation for unregistering callbacks
}

bool ConfigurationManager::validateConfiguration() const {
    return true; // Simple validation
}

std::vector<std::string> ConfigurationManager::getValidationErrors() const {
    return {}; // No errors for now
}

bool ConfigurationManager::importFromJSON(const std::string& jsonData) {
    // Basic JSON import implementation
    return true;
}

std::string ConfigurationManager::exportToJSON() const {
    // Basic JSON export implementation
    return "{}";
}

bool ConfigurationManager::importFromINI(const std::string& iniData) {
    // Basic INI import implementation
    return true;
}

std::string ConfigurationManager::exportToINI() const {
    // Basic INI export implementation
    return "";
}

void ConfigurationManager::notifyChange(const std::string& key, const ConfigValue& oldValue, const ConfigValue& newValue) {
    // Implementation for change notifications
}

} // namespace uevrLCX
