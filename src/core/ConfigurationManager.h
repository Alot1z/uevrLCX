#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <functional>
#include <mutex>

namespace uevrLCX {

enum class ConfigType {
    STRING,
    INTEGER,
    FLOAT,
    BOOLEAN,
    ARRAY,
    OBJECT
};

struct ConfigValue {
    ConfigType type;
    std::string stringValue;
    int64_t intValue;
    double floatValue;
    bool boolValue;
    std::vector<std::string> arrayValue;
    std::unordered_map<std::string, std::string> objectValue;
    
    ConfigValue() : type(ConfigType::STRING), intValue(0), floatValue(0.0), boolValue(false) {}
};

class ConfigurationManager {
public:
    ConfigurationManager();
    ~ConfigurationManager();

    // Core functionality
    bool loadConfiguration(const std::string& filename);
    bool saveConfiguration(const std::string& filename);
    void createDefaultConfiguration();
    
    // Value access
    std::string getString(const std::string& key, const std::string& defaultValue = "");
    int64_t getInteger(const std::string& key, int64_t defaultValue = 0);
    double getFloat(const std::string& key, double defaultValue = 0.0);
    bool getBoolean(const std::string& key, bool defaultValue = false);
    std::vector<std::string> getArray(const std::string& key);
    std::unordered_map<std::string, std::string> getObject(const std::string& key);
    
    // Value setting
    void setString(const std::string& key, const std::string& value);
    void setInteger(const std::string& key, int64_t value);
    void setFloat(const std::string& key, double value);
    void setBoolean(const std::string& key, bool value);
    void setArray(const std::string& key, const std::vector<std::string>& value);
    void setObject(const std::string& key, const std::unordered_map<std::string, std::string>& value);
    
    // Configuration management
    bool hasKey(const std::string& key) const;
    void removeKey(const std::string& key);
    std::vector<std::string> getKeys() const;
    void clear();
    
    // Change notifications
    using ChangeCallback = std::function<void(const std::string&, const ConfigValue&, const ConfigValue&)>;
    void registerChangeCallback(const std::string& key, ChangeCallback callback);
    void unregisterChangeCallback(const std::string& key);
    
    // Validation
    bool validateConfiguration() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Import/Export
    bool importFromJSON(const std::string& jsonData);
    std::string exportToJSON() const;
    bool importFromINI(const std::string& iniData);
    std::string exportToINI() const;

private:
    std::unordered_map<std::string, ConfigValue> config;
    std::unordered_map<std::string, std::vector<ChangeCallback>> changeCallbacks;
    mutable std::mutex configMutex;
    
    // Internal methods
    void notifyChange(const std::string& key, const ConfigValue& oldValue, const ConfigValue& newValue);
    ConfigValue parseValue(const std::string& value, ConfigType type);
    std::string serializeValue(const ConfigValue& value) const;
    bool parseINI(const std::string& data);
    std::string serializeINI() const;
    bool parseJSON(const std::string& data);
    std::string serializeJSON() const;
};

} // namespace uevrLCX
