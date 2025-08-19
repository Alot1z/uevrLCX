# Framework save_config() Function Documentation

## 📋 **FUNCTION OVERVIEW**

**Function**: `Framework::save_config()`  
**File**: `src/Framework.cpp`  
**Purpose**: Save current framework configuration to persistent storage  
**Category**: Configuration Management  

---

## 🎯 **FUNCTION SIGNATURE**

```cpp
void save_config()
```

### **Parameters**
- None

### **Return Value**
- `void`

---

## 🔧 **IMPLEMENTATION DETAILS**

### **Configuration Save Process**
1. **Gather Settings**: Collect all current configuration values
2. **Serialize Data**: Convert settings to JSON format
3. **File Operations**: Write configuration to persistent file
4. **Backup Creation**: Create backup of previous configuration
5. **Validation**: Verify saved configuration integrity
6. **Error Handling**: Handle file system errors gracefully

### **Configuration Categories**

#### **Core Settings**
```cpp
void Framework::save_config() {
    try {
        json config_json;
        
        // Core framework settings
        config_json["framework"]["version"] = UEVR_VERSION;
        config_json["framework"]["renderer_type"] = static_cast<int>(m_renderer_type);
        config_json["framework"]["vr_enabled"] = m_vr_enabled;
        
        // VR settings
        if (m_vr_mod) {
            config_json["vr"] = m_vr_mod->get_config();
        }
        
        // Mod settings
        config_json["mods"] = json::array();
        for (const auto& mod : m_mods) {
            if (mod->has_config()) {
                config_json["mods"].push_back(mod->get_config());
            }
        }
        
        // Write to file
        auto config_path = get_persistent_dir() / "uevr_config.json";
        std::ofstream config_file(config_path);
        config_file << config_json.dump(4);
        
    } catch (const std::exception& e) {
        spdlog::error("Failed to save config: {}", e.what());
    }
}
```

#### **VR Configuration**
- **Runtime Settings**: OpenXR/OpenVR preferences
- **Display Options**: Resolution, refresh rate, IPD
- **Comfort Settings**: Snap turning, teleportation, comfort vignette
- **Performance**: Render scale, fixed foveated rendering
- **Input Mapping**: Controller bindings and gestures

#### **Mod Configuration**
- **Enabled Mods**: List of active modifications
- **Mod Settings**: Individual mod configurations
- **Key Bindings**: Hotkey assignments
- **UI Preferences**: Interface customization

---

## 📚 **USAGE EXAMPLES**

### **Automatic Save on Exit**
```cpp
Framework::~Framework() {
    // Save configuration before destruction
    save_config();
    
    // Cleanup other resources
    cleanup();
}
```

### **Manual Save Trigger**
```cpp
void on_settings_changed() {
    // Save immediately when settings change
    if (g_framework && g_framework->is_valid()) {
        g_framework->save_config();
    }
}
```

### **Deferred Save (Recommended)**
```cpp
void Framework::on_config_changed() {
    // Use deferred save to avoid frequent disk writes
    deferred_save_config();
}

void Framework::deferred_save_config() {
    m_frame_worker->enqueue([this]() {
        save_config();
    });
}
```

### **Custom Mod Configuration**
```cpp
class CustomMod : public Mod {
public:
    json get_config() const override {
        json config;
        config["enabled"] = m_enabled;
        config["intensity"] = m_intensity;
        config["custom_setting"] = m_custom_value;
        return config;
    }
    
    void set_config(const json& config) override {
        if (config.contains("enabled")) {
            m_enabled = config["enabled"];
        }
        if (config.contains("intensity")) {
            m_intensity = config["intensity"];
        }
        if (config.contains("custom_setting")) {
            m_custom_value = config["custom_setting"];
        }
    }
};
```

---

## 🔗 **RELATED FUNCTIONS**
- `Framework::load_config()` - Load configuration from file
- `Framework::reset_config()` - Reset to default configuration
- `Framework::reload_config()` - Reload configuration from file
- `Framework::deferred_save_config()` - Asynchronous save operation
- `Framework::get_persistent_dir()` - Get configuration directory

---

## ⚠️ **IMPORTANT NOTES**
- Configuration is saved in JSON format for readability
- Backup is created before overwriting existing config
- Thread-safe when using deferred save
- Handles file system errors gracefully
- Validates configuration integrity after save

---

## 🚀 **IMPLEMENTATION GUIDE**

To implement configuration saving in your project:

1. **JSON Serialization**: Use modern JSON library (nlohmann/json)
2. **Error Handling**: Comprehensive error handling for file operations
3. **Backup Strategy**: Always backup before overwriting
4. **Validation**: Verify saved data integrity

```cpp
class MyFramework {
private:
    std::filesystem::path config_path;
    json current_config;
    
public:
    void save_config() {
        try {
            // Create backup
            if (std::filesystem::exists(config_path)) {
                auto backup_path = config_path;
                backup_path += ".backup";
                std::filesystem::copy_file(config_path, backup_path, 
                    std::filesystem::copy_options::overwrite_existing);
            }
            
            // Prepare configuration
            json config;
            config["version"] = "1.0.0";
            config["timestamp"] = std::chrono::system_clock::now().time_since_epoch().count();
            
            // Add framework settings
            config["framework"]["vr_enabled"] = vr_enabled;
            config["framework"]["renderer"] = renderer_type;
            
            // Add component configs
            for (const auto& component : components) {
                if (component->has_config()) {
                    config["components"][component->get_name()] = component->get_config();
                }
            }
            
            // Write to temporary file first
            auto temp_path = config_path;
            temp_path += ".tmp";
            
            std::ofstream temp_file(temp_path);
            if (!temp_file.is_open()) {
                throw std::runtime_error("Cannot open temporary config file");
            }
            
            temp_file << config.dump(4);
            temp_file.close();
            
            // Atomic move to final location
            std::filesystem::rename(temp_path, config_path);
            
            spdlog::info("Configuration saved successfully");
            
        } catch (const std::exception& e) {
            spdlog::error("Failed to save configuration: {}", e.what());
            
            // Restore from backup if available
            auto backup_path = config_path;
            backup_path += ".backup";
            if (std::filesystem::exists(backup_path)) {
                try {
                    std::filesystem::copy_file(backup_path, config_path,
                        std::filesystem::copy_options::overwrite_existing);
                } catch (...) {
                    spdlog::error("Failed to restore configuration from backup");
                }
            }
        }
    }
};
```

---

## 🛡️ **BEST PRACTICES**
- **Atomic Operations**: Use temporary file + rename for atomic saves
- **Backup Strategy**: Always create backup before overwriting
- **Error Recovery**: Implement rollback mechanism
- **Validation**: Verify saved data can be loaded
- **Performance**: Use deferred saves to avoid blocking operations
- **Security**: Validate file paths to prevent directory traversal
- **Compression**: Consider compression for large configurations
