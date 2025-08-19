# Framework initialize_hooks() Function Documentation

## 📋 **FUNCTION OVERVIEW**

**Function**: `Framework::initialize_hooks()`  
**File**: `src/Framework.cpp`  
**Purpose**: Install DirectX and Windows API hooks for graphics interception  
**Category**: Hook System Initialization  

---

## 🎯 **FUNCTION SIGNATURE**

```cpp
void initialize_hooks()
```

### **Parameters**
- None

### **Return Value**
- `void`

---

## 🔧 **IMPLEMENTATION DETAILS**

### **Hook Initialization Process**
1. **DirectX 11 Hook**: Install D3D11 Present and device hooks
2. **DirectX 12 Hook**: Install D3D12 Present and command list hooks
3. **Windows Message Hook**: Install window message interception
4. **DirectInput Hook**: Install input system hooks
5. **Hook Validation**: Verify all hooks are properly installed
6. **Renderer Detection**: Determine which graphics API is active

### **DirectX Hook Installation Sequence**
```cpp
void Framework::initialize_hooks() {
    try {
        // Initialize D3D11 hooks
        if (!m_d3d11_hook) {
            m_d3d11_hook = std::make_unique<D3D11Hook>();
            if (m_d3d11_hook->install()) {
                m_is_d3d11 = true;
                m_renderer_type = RendererType::D3D11;
                spdlog::info("D3D11 hooks installed successfully");
            } else {
                spdlog::warn("D3D11 hook installation failed");
            }
        }
        
        // Initialize D3D12 hooks
        if (!m_d3d12_hook) {
            m_d3d12_hook = std::make_unique<D3D12Hook>();
            if (m_d3d12_hook->install()) {
                m_is_d3d12 = true;
                m_renderer_type = RendererType::D3D12;
                spdlog::info("D3D12 hooks installed successfully");
            } else {
                spdlog::warn("D3D12 hook installation failed");
            }
        }
        
        // Initialize Windows message hooks
        if (!m_windows_message_hook) {
            m_windows_message_hook = std::make_unique<WindowsMessageHook>();
            if (m_windows_message_hook->install()) {
                spdlog::info("Windows message hooks installed successfully");
            } else {
                spdlog::warn("Windows message hook installation failed");
            }
        }
        
        // Initialize DirectInput hooks
        if (!m_dinput_hook) {
            m_dinput_hook = std::make_unique<DInputHook>();
            if (m_dinput_hook->install()) {
                spdlog::info("DirectInput hooks installed successfully");
            } else {
                spdlog::warn("DirectInput hook installation failed");
            }
        }
        
        // Start hook monitoring thread
        start_hook_monitor();
        
    } catch (const std::exception& e) {
        spdlog::error("Hook initialization failed: {}", e.what());
    }
}
```

### **D3D11 Hook Implementation**
```cpp
bool D3D11Hook::install() {
    try {
        // Get D3D11 device creation function
        auto d3d11_module = GetModuleHandle(L"d3d11.dll");
        if (!d3d11_module) {
            spdlog::error("D3D11 module not found");
            return false;
        }
        
        // Hook D3D11CreateDeviceAndSwapChain
        auto create_device_func = GetProcAddress(d3d11_module, "D3D11CreateDeviceAndSwapChain");
        if (!create_device_func) {
            spdlog::error("D3D11CreateDeviceAndSwapChain not found");
            return false;
        }
        
        // Install MinHook
        if (MH_Initialize() != MH_OK) {
            spdlog::error("MinHook initialization failed");
            return false;
        }
        
        // Create hook for device creation
        if (MH_CreateHook(create_device_func, &D3D11CreateDeviceAndSwapChain_Hook, 
                         reinterpret_cast<LPVOID*>(&m_original_create_device)) != MH_OK) {
            spdlog::error("Failed to create D3D11 device hook");
            return false;
        }
        
        // Enable the hook
        if (MH_EnableHook(create_device_func) != MH_OK) {
            spdlog::error("Failed to enable D3D11 device hook");
            return false;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("D3D11 hook installation error: {}", e.what());
        return false;
    }
}
```

### **D3D12 Hook Implementation**
```cpp
bool D3D12Hook::install() {
    try {
        // Get D3D12 device creation function
        auto d3d12_module = GetModuleHandle(L"d3d12.dll");
        if (!d3d12_module) {
            spdlog::error("D3D12 module not found");
            return false;
        }
        
        // Hook D3D12CreateDevice
        auto create_device_func = GetProcAddress(d3d12_module, "D3D12CreateDevice");
        if (!create_device_func) {
            spdlog::error("D3D12CreateDevice not found");
            return false;
        }
        
        // Install MinHook
        if (MH_Initialize() != MH_OK) {
            spdlog::error("MinHook initialization failed");
            return false;
        }
        
        // Create hook for device creation
        if (MH_CreateHook(create_device_func, &D3D12CreateDevice_Hook, 
                         reinterpret_cast<LPVOID*>(&m_original_create_device)) != MH_OK) {
            spdlog::error("Failed to create D3D12 device hook");
            return false;
        }
        
        // Enable the hook
        if (MH_EnableHook(create_device_func) != MH_OK) {
            spdlog::error("Failed to enable D3D12 device hook");
            return false;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("D3D12 hook installation error: {}", e.what());
        return false;
    }
}
```

---

## 📚 **USAGE EXAMPLES**

### **Automatic Hook Initialization**
```cpp
Framework::Framework(HMODULE framework_module) 
    : m_framework_module(framework_module)
{
    // Initialize hooks before VR system
    initialize_logging();
    initialize_hooks();  // Install all necessary hooks
    
    // Check which renderer was detected
    if (m_is_d3d11) {
        spdlog::info("D3D11 renderer detected");
    } else if (m_is_d3d12) {
        spdlog::info("D3D12 renderer detected");
    } else {
        spdlog::warn("No DirectX renderer detected");
    }
    
    // Continue with VR initialization
    initialize_vr();
}
```

### **Manual Hook Installation**
```cpp
void install_specific_hooks() {
    // Install only D3D11 hooks
    if (!m_d3d11_hook) {
        m_d3d11_hook = std::make_unique<D3D11Hook>();
        if (m_d3d11_hook->install()) {
            m_is_d3d11 = true;
            m_renderer_type = RendererType::D3D11;
        }
    }
    
    // Install only Windows message hooks
    if (!m_windows_message_hook) {
        m_windows_message_hook = std::make_unique<WindowsMessageHook>();
        m_windows_message_hook->install();
    }
}
```

### **Hook Status Monitoring**
```cpp
void Framework::check_hook_status() {
    spdlog::info("Hook Status Report:");
    spdlog::info("  D3D11: {}", m_is_d3d11 ? "Active" : "Inactive");
    spdlog::info("  D3D12: {}", m_is_d3d12 ? "Active" : "Inactive");
    spdlog::info("  Windows Messages: {}", m_windows_message_hook ? "Active" : "Inactive");
    spdlog::info("  DirectInput: {}", m_dinput_hook ? "Active" : "Inactive");
    
    if (m_renderer_type != RendererType::Unknown) {
        spdlog::info("  Renderer: {}", static_cast<int>(m_renderer_type));
    }
}
```

---

## 🔗 **RELATED FUNCTIONS**
- `Framework::remove_hooks()` - Remove all installed hooks
- `Framework::hook_monitor()` - Monitor hook status and health
- `Framework::on_frame_d3d11()` - D3D11 frame processing
- `Framework::on_frame_d3d12()` - D3D12 frame processing
- `Framework::on_message()` - Handle intercepted Windows messages
- `D3D11Hook::install()` - Install D3D11 specific hooks
- `D3D12Hook::install()` - Install D3D12 specific hooks

---

## ⚠️ **IMPORTANT NOTES**
- Called during framework initialization
- Installs hooks for multiple graphics APIs
- Uses MinHook library for API interception
- Thread-safe hook installation
- Gracefully handles hook installation failures
- Starts monitoring thread for hook health

---

## 🚀 **IMPLEMENTATION GUIDE**

To implement hook initialization in your project:

1. **Hook Library**: Use MinHook or similar hooking library
2. **API Detection**: Detect which graphics APIs are available
3. **Error Handling**: Handle hook installation failures gracefully
4. **Thread Safety**: Ensure thread-safe hook installation
5. **Monitoring**: Implement hook health monitoring

```cpp
class MyHookSystem {
private:
    std::unique_ptr<D3D11Hook> m_d3d11_hook;
    std::unique_ptr<D3D12Hook> m_d3d12_hook;
    bool m_d3d11_active = false;
    bool m_d3d12_active = false;
    
public:
    void initialize_hooks() {
        try {
            // Try D3D11 first
            if (try_install_d3d11_hooks()) {
                m_d3d11_active = true;
                spdlog::info("D3D11 hooks installed");
            }
            
            // Try D3D12
            if (try_install_d3d12_hooks()) {
                m_d3d12_active = true;
                spdlog::info("D3D12 hooks installed");
            }
            
            // Check if any hooks were installed
            if (!m_d3d11_active && !m_d3d12_active) {
                spdlog::error("No graphics API hooks could be installed");
            }
            
        } catch (const std::exception& e) {
            spdlog::error("Hook initialization failed: {}", e.what());
        }
    }
    
private:
    bool try_install_d3d11_hooks() {
        try {
            m_d3d11_hook = std::make_unique<D3D11Hook>();
            return m_d3d11_hook->install();
        } catch (...) {
            return false;
        }
    }
    
    bool try_install_d3d12_hooks() {
        try {
            m_d3d12_hook = std::make_unique<D3D12Hook>();
            return m_d3d12_hook->install();
        } catch (...) {
            return false;
        }
    }
};

// Hook implementation example
class D3D11Hook {
public:
    bool install() {
        // Initialize MinHook
        if (MH_Initialize() != MH_OK) {
            return false;
        }
        
        // Get target function
        auto d3d11_module = GetModuleHandle(L"d3d11.dll");
        if (!d3d11_module) {
            return false;
        }
        
        auto target_func = GetProcAddress(d3d11_module, "D3D11CreateDeviceAndSwapChain");
        if (!target_func) {
            return false;
        }
        
        // Create hook
        if (MH_CreateHook(target_func, &hook_function, &original_function) != MH_OK) {
            return false;
        }
        
        // Enable hook
        return MH_EnableHook(target_func) == MH_OK;
    }
    
private:
    static HRESULT WINAPI hook_function(IDXGIAdapter* pAdapter, D3D_DRIVER_TYPE DriverType,
                                      HMODULE Software, UINT Flags, const D3D_FEATURE_LEVEL* pFeatureLevels,
                                      UINT FeatureLevels, UINT SDKVersion, const DXGI_SWAP_CHAIN_DESC* pSwapChainDesc,
                                      IDXGISwapChain** ppSwapChain, ID3D11Device** ppDevice,
                                      D3D_FEATURE_LEVEL* pFeatureLevel, ID3D11DeviceContext** ppImmediateContext) {
        // Call original function
        HRESULT result = original_function(pAdapter, DriverType, Software, Flags, pFeatureLevels,
                                         FeatureLevels, SDKVersion, pSwapChainDesc, ppSwapChain,
                                         ppDevice, pFeatureLevel, ppImmediateContext);
        
        if (SUCCEEDED(result)) {
            // Hook successful, store device and swap chain
            if (ppDevice && *ppDevice) {
                g_framework->on_d3d11_device_created(*ppDevice);
            }
            if (ppSwapChain && *ppSwapChain) {
                g_framework->on_d3d11_swapchain_created(*ppSwapChain);
            }
        }
        
        return result;
    }
    
    static decltype(&D3D11CreateDeviceAndSwapChain) original_function;
};

decltype(&D3D11CreateDeviceAndSwapChain) D3D11Hook::original_function = nullptr;
```

---

## 🛡️ **BEST PRACTICES**
- **Hook Library**: Use established hooking libraries (MinHook, Detours)
- **API Detection**: Detect available graphics APIs before hooking
- **Error Recovery**: Continue operation even if some hooks fail
- **Thread Safety**: Ensure thread-safe hook installation
- **Resource Management**: Properly manage hook resources
- **Monitoring**: Implement hook health monitoring
- **Cleanup**: Properly remove hooks on shutdown

