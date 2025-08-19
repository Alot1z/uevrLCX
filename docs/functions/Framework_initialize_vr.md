# Framework initialize_vr() Function Documentation

## 📋 **FUNCTION OVERVIEW**

**Function**: `Framework::initialize_vr()`  
**File**: `src/Framework.cpp`  
**Purpose**: Initialize VR runtime and prepare VR system for operation  
**Category**: VR System Initialization  

---

## 🎯 **FUNCTION SIGNATURE**

```cpp
void initialize_vr()
```

### **Parameters**
- None

### **Return Value**
- `void`

---

## 🔧 **IMPLEMENTATION DETAILS**

### **VR Initialization Process**
1. **Runtime Detection**: Detect available VR runtimes (OpenXR, OpenVR)
2. **Device Enumeration**: Enumerate VR headsets and controllers
3. **Session Creation**: Create VR session with selected runtime
4. **Render Target Setup**: Configure VR-specific render targets
5. **Input System**: Initialize VR input handling system
6. **Performance Monitoring**: Set up VR performance tracking

### **VR Runtime Initialization Sequence**
```cpp
void Framework::initialize_vr() {
    try {
        // Create VR mod instance
        m_vr_mod = std::make_unique<VRMod>();
        m_mods.push_back(m_vr_mod.get());
        
        // Try OpenXR first (preferred)
        if (m_vr_mod->initialize_openxr()) {
            spdlog::info("OpenXR initialized successfully");
            m_vr_runtime = VRRuntime::OpenXR;
            return;
        }
        
        // Fallback to OpenVR
        if (m_vr_mod->initialize_openvr()) {
            spdlog::info("OpenVR initialized successfully");
            m_vr_runtime = VRRuntime::OpenVR;
            return;
        }
        
        // No VR runtime available
        spdlog::warn("No VR runtime available, VR features disabled");
        m_vr_enabled = false;
        
    } catch (const std::exception& e) {
        spdlog::error("VR initialization failed: {}", e.what());
        m_vr_enabled = false;
    }
}
```

### **OpenXR Initialization**
```cpp
bool VRMod::initialize_openxr() {
    try {
        // Load OpenXR loader
        if (!load_openxr_loader()) {
            spdlog::error("Failed to load OpenXR loader");
            return false;
        }
        
        // Enumerate available extensions
        auto extensions = enumerate_openxr_extensions();
        if (extensions.empty()) {
            spdlog::error("No OpenXR extensions available");
            return false;
        }
        
        // Create OpenXR instance
        XrInstanceCreateInfo create_info = {};
        create_info.type = XR_TYPE_INSTANCE_CREATE_INFO;
        create_info.applicationInfo.apiVersion = XR_CURRENT_API_VERSION;
        create_info.enabledExtensionCount = extensions.size();
        create_info.enabledExtensionNames = extensions.data();
        
        XrInstance instance;
        if (xrCreateInstance(&create_info, &instance) != XR_SUCCESS) {
            spdlog::error("Failed to create OpenXR instance");
            return false;
        }
        
        m_openxr_instance = instance;
        
        // Create VR session
        if (!create_openxr_session()) {
            spdlog::error("Failed to create OpenXR session");
            return false;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("OpenXR initialization error: {}", e.what());
        return false;
    }
}
```

### **OpenVR Initialization**
```cpp
bool VRMod::initialize_openvr() {
    try {
        // Initialize OpenVR system
        vr::EVRInitError init_error;
        m_openvr_system = vr::VR_Init(&init_error, vr::VRApplication_Scene);
        
        if (init_error != vr::VRInitError_None) {
            spdlog::error("OpenVR initialization failed: {}", 
                         vr::VR_GetVRInitErrorAsEnglishDescription(init_error));
            return false;
        }
        
        // Get OpenVR runtime version
        std::string runtime_version = m_openvr_system->GetRuntimeVersion();
        spdlog::info("OpenVR runtime version: {}", runtime_version);
        
        // Initialize compositor
        if (!vr::VRCompositor()) {
            spdlog::error("OpenVR compositor not available");
            return false;
        }
        
        // Get display properties
        vr::VRDisplay_t display_props;
        m_openvr_system->GetDisplayBounds(&display_props);
        
        spdlog::info("VR display: {}x{}", 
                     display_props.nWidth, display_props.nHeight);
        
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("OpenVR initialization error: {}", e.what());
        return false;
    }
}
```

---

## 📚 **USAGE EXAMPLES**

### **Automatic VR Initialization**
```cpp
Framework::Framework(HMODULE framework_module) 
    : m_framework_module(framework_module)
{
    // Initialize VR after basic setup
    initialize_logging();
    initialize_hooks();
    initialize_vr();  // Initialize VR runtime
    
    // Check VR status
    if (m_vr_enabled) {
        spdlog::info("VR system ready");
    } else {
        spdlog::warn("VR system not available");
    }
}
```

### **Manual VR Initialization**
```cpp
void enable_vr_mode() {
    if (!m_vr_mod) {
        initialize_vr();
    }
    
    if (m_vr_enabled) {
        m_vr_mod->enable();
        spdlog::info("VR mode enabled");
    }
}
```

### **VR Runtime Selection**
```cpp
void Framework::select_vr_runtime(VRRuntime preferred_runtime) {
    if (preferred_runtime == VRRuntime::OpenXR) {
        if (m_vr_mod->initialize_openxr()) {
            m_vr_runtime = VRRuntime::OpenXR;
            return;
        }
    }
    
    // Fallback to OpenVR
    if (m_vr_mod->initialize_openvr()) {
        m_vr_runtime = VRRuntime::OpenVR;
        return;
    }
    
    spdlog::error("No VR runtime available");
}
```

---

## 🔗 **RELATED FUNCTIONS**
- `Framework::cleanup_vr()` - Clean up VR resources
- `Framework::enable_vr()` - Enable VR mode
- `Framework::disable_vr()` - Disable VR mode
- `Framework::update_vr()` - Update VR state
- `VRMod::initialize_openxr()` - OpenXR initialization
- `VRMod::initialize_openvr()` - OpenVR initialization

---

## ⚠️ **IMPORTANT NOTES**
- Called during framework initialization
- Gracefully handles missing VR hardware
- Supports multiple VR runtimes with fallback
- Initializes VR mod system
- Sets up VR-specific render targets
- Thread-safe initialization

---

## 🚀 **IMPLEMENTATION GUIDE**

To implement VR initialization in your project:

1. **Runtime Detection**: Detect available VR runtimes
2. **Extension Support**: Check for required VR extensions
3. **Session Management**: Create and manage VR sessions
4. **Error Handling**: Graceful fallback when VR is unavailable
5. **Performance Setup**: Configure VR-specific performance settings

```cpp
class MyVRSystem {
private:
    enum class VRRuntime { None, OpenXR, OpenVR };
    VRRuntime m_runtime = VRRuntime::None;
    bool m_vr_enabled = false;
    
public:
    void initialize_vr() {
        // Try OpenXR first
        if (try_initialize_openxr()) {
            m_runtime = VRRuntime::OpenXR;
            m_vr_enabled = true;
            return;
        }
        
        // Try OpenVR as fallback
        if (try_initialize_openvr()) {
            m_runtime = VRRuntime::OpenVR;
            m_vr_enabled = true;
            return;
        }
        
        // No VR available
        m_runtime = VRRuntime::None;
        m_vr_enabled = false;
        spdlog::warn("No VR runtime available");
    }
    
private:
    bool try_initialize_openxr() {
        try {
            // Load OpenXR loader
            if (!load_openxr_library()) {
                return false;
            }
            
            // Check for required extensions
            if (!check_openxr_extensions()) {
                return false;
            }
            
            // Create instance and session
            if (!create_openxr_instance()) {
                return false;
            }
            
            if (!create_openxr_session()) {
                return false;
            }
            
            spdlog::info("OpenXR initialized successfully");
            return true;
            
        } catch (const std::exception& e) {
            spdlog::error("OpenXR initialization failed: {}", e.what());
            return false;
        }
    }
    
    bool try_initialize_openvr() {
        try {
            // Initialize OpenVR
            vr::EVRInitError init_error;
            auto system = vr::VR_Init(&init_error, vr::VRApplication_Scene);
            
            if (init_error != vr::VRInitError_None) {
                return false;
            }
            
            // Check compositor
            if (!vr::VRCompositor()) {
                return false;
            }
            
            spdlog::info("OpenVR initialized successfully");
            return true;
            
        } catch (const std::exception& e) {
            spdlog::error("OpenVR initialization failed: {}", e.what());
            return false;
        }
    }
    
    bool check_openxr_extensions() {
        // Check for required extensions like:
        // - XR_KHR_vulkan_enable
        // - XR_KHR_D3D11_enable
        // - XR_KHR_D3D12_enable
        return true; // Implementation specific
    }
};
```

---

## 🛡️ **BEST PRACTICES**
- **Runtime Fallback**: Always provide fallback VR runtime support
- **Extension Checking**: Verify required VR extensions are available
- **Error Recovery**: Continue operation even when VR initialization fails
- **Performance Monitoring**: Set up VR-specific performance tracking
- **Resource Management**: Properly manage VR resources and sessions
- **Thread Safety**: Ensure VR initialization is thread-safe
- **Logging**: Comprehensive logging for VR initialization debugging

