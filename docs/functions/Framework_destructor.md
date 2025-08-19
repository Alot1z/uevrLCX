# Framework Destructor Documentation

## 📋 **FUNCTION OVERVIEW**

**Function**: `Framework::~Framework()`  
**File**: `src/Framework.cpp`  
**Purpose**: Clean up and destroy the UEVR framework instance  
**Category**: Core Framework Cleanup  

---

## 🎯 **FUNCTION SIGNATURE**

```cpp
Framework::~Framework()
```

### **Parameters**
- None (Destructor)

### **Return Value**
- None (Destructor)

---

## 🔧 **IMPLEMENTATION DETAILS**

### **Cleanup Sequence**
1. **Stop VR Runtime**: Safely shutdown OpenXR/OpenVR sessions
2. **Remove Hooks**: Uninstall all DirectX and Windows API hooks
3. **Save Configuration**: Write current settings to persistent storage
4. **Cleanup Plugins**: Unload all loaded plugins
5. **Release Resources**: Free all allocated memory and handles
6. **Stop Threads**: Terminate monitoring and worker threads

### **Critical Cleanup Operations**
- **DirectX Hook Removal**: Safely removes D3D11/D3D12 hooks
- **VR Session Cleanup**: Properly closes VR runtime sessions
- **Memory Management**: Releases all allocated resources
- **Thread Synchronization**: Ensures all threads are properly terminated
- **Configuration Save**: Preserves user settings and profiles

---

## 📚 **CLEANUP COMPONENTS**

### **VR Runtime Cleanup**
```cpp
// VR runtime shutdown sequence
if (m_vr_runtime) {
    m_vr_runtime->shutdown();
    m_vr_runtime.reset();
}
```

### **Hook Removal**
```cpp
// DirectX hook cleanup
if (m_d3d11_hook) {
    m_d3d11_hook->remove();
    m_d3d11_hook.reset();
}
```

### **Resource Deallocation**
```cpp
// Memory cleanup
m_mods.clear();
m_plugins.clear();
m_config.reset();
```

---

## 🔗 **RELATED FUNCTIONS**
- `Framework::Framework()` - Constructor and initialization
- `Framework::cleanup()` - Manual cleanup function
- `Framework::save_config()` - Configuration saving
- `Framework::remove_hooks()` - Hook removal system

---

## ⚠️ **IMPORTANT NOTES**
- Called automatically when Framework goes out of scope
- Ensures safe cleanup of all VR and graphics resources
- Saves configuration before destruction
- Thread-safe cleanup operations
- Critical for preventing crashes on application exit

---

## 🚀 **IMPLEMENTATION GUIDE**

To implement proper cleanup in your project:

1. **RAII Pattern**: Use smart pointers for automatic cleanup
2. **Exception Safety**: Ensure cleanup happens even with exceptions
3. **Resource Ordering**: Clean up resources in reverse order of creation
4. **Thread Safety**: Synchronize cleanup with active threads

```cpp
// Example implementation
class MyFramework {
    ~MyFramework() {
        // Stop VR first
        if (vr_system) {
            vr_system->shutdown();
        }
        
        // Remove hooks second
        if (graphics_hooks) {
            graphics_hooks->remove_all();
        }
        
        // Save config last
        save_configuration();
    }
};
```

---

## 🛡️ **SAFETY CONSIDERATIONS**
- Never call destructor manually
- Ensure all dependent objects are destroyed first
- Handle exceptions during cleanup gracefully
- Verify all threads are terminated before destruction
