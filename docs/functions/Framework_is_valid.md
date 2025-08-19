# Framework is_valid() Function Documentation

## 📋 **FUNCTION OVERVIEW**

**Function**: `Framework::is_valid()`  
**File**: `src/Framework.hpp`  
**Purpose**: Check if the framework was successfully initialized  
**Category**: Framework Status Validation  

---

## 🎯 **FUNCTION SIGNATURE**

```cpp
bool is_valid() const
```

### **Parameters**
- None

### **Return Value**
- `bool`: `true` if framework is valid and ready, `false` otherwise

---

## 🔧 **IMPLEMENTATION DETAILS**

### **Validation Checks**
The function performs comprehensive validation of:

1. **Module Handle**: Verifies framework module is loaded
2. **Hook Status**: Confirms DirectX hooks are installed
3. **VR Runtime**: Checks VR system initialization
4. **Configuration**: Validates configuration loading
5. **Memory State**: Ensures all critical objects are allocated

### **Internal State Verification**
```cpp
bool is_valid() const { 
    return m_valid; 
}
```

The `m_valid` flag is set during initialization and reflects:
- Successful module loading
- DirectX hook installation
- VR runtime initialization
- Configuration system setup
- Memory allocation success

---

## 📚 **USAGE EXAMPLES**

### **Basic Validation**
```cpp
if (g_framework && g_framework->is_valid()) {
    // Framework is ready for use
    g_framework->enable_vr();
} else {
    // Initialization failed
    spdlog::error("Framework initialization failed");
}
```

### **Initialization Check**
```cpp
void startup_thread(HMODULE module) {
    g_framework = std::make_unique<Framework>(module);
    
    if (!g_framework->is_valid()) {
        MessageBox(nullptr, L"UEVR initialization failed", L"Error", MB_OK);
        return;
    }
    
    spdlog::info("UEVR Framework initialized successfully");
}
```

### **Runtime Validation**
```cpp
void on_frame() {
    if (!g_framework || !g_framework->is_valid()) {
        return; // Skip frame processing
    }
    
    // Safe to proceed with frame operations
    g_framework->on_frame_d3d11();
}
```

---

## 🔗 **RELATED FUNCTIONS**
- `Framework::Framework()` - Sets validity during initialization
- `Framework::is_ready()` - Checks if fully ready for operations
- `Framework::is_game_data_intialized()` - Game-specific initialization
- `Framework::initialize()` - Secondary initialization

---

## ⚠️ **IMPORTANT NOTES**
- Always check validity before using framework functions
- Validity can change during runtime (rare)
- Thread-safe function (const member)
- Critical for preventing crashes
- Should be checked after construction

---

## 🚀 **IMPLEMENTATION GUIDE**

To implement similar validation in your project:

1. **Initialization Flag**: Use boolean flag to track state
2. **Comprehensive Checks**: Validate all critical components
3. **Thread Safety**: Make validation function const and thread-safe
4. **Early Returns**: Check validity at function entry points

```cpp
class MyFramework {
private:
    bool m_valid = false;
    
public:
    bool is_valid() const { return m_valid; }
    
    void initialize() {
        // Perform initialization
        bool hooks_ok = install_hooks();
        bool vr_ok = initialize_vr();
        bool config_ok = load_config();
        
        m_valid = hooks_ok && vr_ok && config_ok;
    }
    
    void some_operation() {
        if (!is_valid()) {
            return; // Early exit if not valid
        }
        
        // Safe to proceed
    }
};
```

---

## 🛡️ **BEST PRACTICES**
- Check validity before every major operation
- Use RAII pattern to ensure proper initialization
- Log validation failures for debugging
- Provide fallback behavior when invalid
- Consider retry mechanisms for transient failures
