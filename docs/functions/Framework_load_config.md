# Framework load_config() Function Documentation

## 📋 **FUNCTION OVERVIEW**

**Function**: `Framework::load_config()`  
**File**: `src/Framework.cpp`  
**Purpose**: Load framework configuration from persistent storage  
**Category**: Configuration Management  

---

## 🎯 **FUNCTION SIGNATURE**

```cpp
void load_config()
```

### **Parameters**
- None

### **Return Value**
- `void`

---

## 🔧 **IMPLEMENTATION DETAILS**

### **Configuration Loading Process**
1. **File Discovery**: Locate configuration file in persistent directory
2. **JSON Parsing**: Parse configuration from JSON format
3. **Validation**: Verify configuration integrity and version compatibility
4. **Application**: Apply loaded settings to framework components
5. **Fallback**: Use default values if configuration is missing or invalid
6. **Error Handling**: Log errors and continue with defaults

### **Configuration Loading Sequence**
```cpp
void Framework::load_config() {
    try {
        auto config_path = get_persistent_dir() / "uevr_config.json";
        
        if (!std::filesystem::exists(config_path)) {
            spdlog::info("No configuration file found, using defaults");
            apply_default_config();
            return;
        }
        
        // Read and parse JSON
        std::ifstream config_file(config_path);
        if (!config_file.is_open()) {
            throw std::runtime_error("Cannot open configuration file");
        }
        
        json config_json;
        config_file >> config_json;
        
        // Validate configuration
        if (!validate_config(config_json)) {
            spdlog::warn("Configuration validation failed, using defaults");
            apply_default_config();
            return;
        }
        
        // Apply configuration
        apply_config(config_json);
        spdlog::info("Configuration loaded successfully");
        
    } catch (const std::exception& e) {
        spdlog::error("Failed to load configuration: {}", e.what());
        apply_default_config();
    }
}
```

### **Configuration Validation**
```cpp
bool Framework::validate_config(const json& config) {
    // Check required fields
    if (!config.contains("framework") || !config.contains("version")) {
        return false;
    }
    
    // Version compatibility check
    std::string version = config["framework"]["version"];
    if (version != UEVR_VERSION) {
        spdlog::warn("Configuration version mismatch: expected {}, got {}", 
                     UEVR_VERSION, version);
        // Continue with version migration if possible
    }
    
    return true;
}
```

---

## 📚 **USAGE EXAMPLES**

### **Automatic Loading on Startup**
```cpp
Framework::Framework(HMODULE framework_module) 
    : m_framework_module(framework_module)
{
    // Load configuration after basic initialization
    initialize_logging();
    load_config();  // Load user preferences
    
    // Continue with other initialization
    initialize_hooks();
    initialize_vr();
}
```

### **Manual Configuration Reload**
```cpp
void on_config_file_changed() {
    // Reload configuration when file changes
    if (g_framework && g_framework->is_valid()) {
        g_framework->load_config();
        spdlog::info("Configuration reloaded from file");
    }
}
```

### **Configuration Migration**
```cpp
void Framework::migrate_config(const json& old_config) {
    json new_config;
    
    // Migrate old format to new format
    if (old_config.contains("vr_settings")) {
        new_config["vr"] = old_config["vr_settings"];
    }
    
    if (old_config.contains("mod_settings")) {
        new_config["mods"] = old_config["mod_settings"];
    }
    
    // Save migrated configuration
    save_config();
}
```

---

## 🔗 **RELATED FUNCTIONS**
- `Framework::save_config()` - Save configuration to file
- `Framework::reset_config()` - Reset to default configuration
- `Framework::reload_config()` - Reload configuration from file
- `Framework::get_persistent_dir()` - Get configuration directory
- `Framework::apply_default_config()` - Apply default settings

---

## ⚠️ **IMPORTANT NOTES**
- Called automatically during framework initialization
- Gracefully handles missing or corrupted configuration files
- Applies default values when configuration is invalid
- Thread-safe when called from initialization
- Logs all configuration loading operations

---

## 🚀 **IMPLEMENTATION GUIDE**

To implement configuration loading in your project:

1. **File Discovery**: Locate configuration file in appropriate directory
2. **JSON Parsing**: Use robust JSON parsing with error handling
3. **Validation**: Implement comprehensive configuration validation
4. **Fallback Strategy**: Provide sensible default values
5. **Error Recovery**: Continue operation even with configuration errors

```cpp
class MyFramework {
private:
    json m_config;
    std::filesystem::path m_config_path;
    
public:
    void load_config() {
        try {
            // Try to load from file
            if (std::filesystem::exists(m_config_path)) {
                std::ifstream file(m_config_path);
                file >> m_config;
                
                if (validate_config(m_config)) {
                    apply_config(m_config);
                    return;
                }
            }
            
            // Fallback to defaults
            apply_default_config();
            
        } catch (const std::exception& e) {
            spdlog::error("Configuration loading failed: {}", e.what());
            apply_default_config();
        }
    }
    
private:
    bool validate_config(const json& config) {
        // Implement validation logic
        return config.contains("version") && config.contains("settings");
    }
    
    void apply_default_config() {
        m_config = {
            {"version", "1.0.0"},
            {"settings", {
                {"vr_enabled", true},
                {"render_scale", 1.0},
                {"comfort_mode", false}
            }}
        };
        
        apply_config(m_config);
    }
    
    void apply_config(const json& config) {
        // Apply configuration to framework components
        if (config.contains("settings")) {
            auto& settings = config["settings"];
            
            if (settings.contains("vr_enabled")) {
                vr_enabled = settings["vr_enabled"];
            }
            
            if (settings.contains("render_scale")) {
                render_scale = settings["render_scale"];
            }
        }
    }
};
```

---

## 🛡️ **BEST PRACTICES**
- **Robust Error Handling**: Never fail completely due to configuration errors
- **Default Values**: Always provide sensible defaults for all settings
- **Version Migration**: Support upgrading old configuration formats
- **Validation**: Verify configuration integrity before application
- **Logging**: Log all configuration operations for debugging
- **Atomic Loading**: Load configuration atomically to prevent partial states
- **Backup Strategy**: Keep backup of working configuration

