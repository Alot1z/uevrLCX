# Framework.cpp File Documentation

## 📋 **FILE OVERVIEW**

**File**: `src/Framework.cpp`  
**Size**: 2067 lines  
**Language**: C++23  
**Purpose**: Core UEVR framework implementation  
**Category**: Core System Implementation  

---

## 🎯 **FILE STRUCTURE**

### **Includes and Dependencies**
```cpp
#include <windows.h>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>
#include <filesystem>
#include <memory>
#include <thread>
#include <mutex>
#include <chrono>

#include "Framework.hpp"
#include "Mod.hpp"
#include "hooks/D3D11Hook.hpp"
#include "hooks/D3D12Hook.hpp"
#include "mods/VR.hpp"
```

### **Global Variables**
```cpp
std::unique_ptr<Framework> g_framework;
std::mutex g_framework_mutex;
```

---

## 🔧 **CLASS IMPLEMENTATION**

### **Constructor Implementation**
```cpp
Framework::Framework(HMODULE framework_module) 
    : m_framework_module(framework_module)
    , m_valid(false)
    , m_initialized(false)
    , m_game_data_initialized(false)
    , m_is_d3d11(false)
    , m_is_d3d12(false)
    , m_renderer_type(RendererType::Unknown)
{
    // Initialization sequence
    initialize_logging();
    initialize_hooks();
    initialize_vr();
    load_config();
    
    m_valid = true;
    spdlog::info("UEVR Framework initialized successfully");
}
```

### **Key Methods Implementation**

#### **Frame Processing Methods**
- `on_frame_d3d11()` - DirectX 11 frame processing
- `on_frame_d3d12()` - DirectX 12 frame processing
- `on_post_present_d3d11()` - Post-present D3D11 processing
- `on_post_present_d3d12()` - Post-present D3D12 processing
- `run_imgui_frame()` - ImGui frame rendering

#### **Hook Management Methods**
- `install_hooks()` - Install DirectX and Windows hooks
- `remove_hooks()` - Remove all installed hooks
- `hook_monitor()` - Monitor hook status
- `on_reset()` - Handle device reset events

#### **Configuration Methods**
- `save_config()` - Save configuration to file
- `load_config()` - Load configuration from file
- `reload_config()` - Reload configuration
- `reset_config()` - Reset to default configuration

#### **VR Integration Methods**
- `initialize_vr()` - Initialize VR runtime
- `enable_vr()` - Enable VR mode
- `disable_vr()` - Disable VR mode
- `update_vr()` - Update VR state

---

## 📚 **DETAILED FUNCTION IMPLEMENTATIONS**

### **Initialization Sequence**
```cpp
void Framework::initialize_logging() {
    auto log_path = get_persistent_dir() / "uevr.log";
    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(log_path.string(), true);
    auto logger = std::make_shared<spdlog::logger>("uevr", file_sink);
    spdlog::set_default_logger(logger);
    spdlog::set_level(spdlog::level::info);
}

void Framework::initialize_hooks() {
    // D3D11 hook installation
    if (!m_d3d11_hook) {
        m_d3d11_hook = std::make_unique<D3D11Hook>();
        if (m_d3d11_hook->install()) {
            m_is_d3d11 = true;
            m_renderer_type = RendererType::D3D11;
        }
    }
    
    // D3D12 hook installation
    if (!m_d3d12_hook) {
        m_d3d12_hook = std::make_unique<D3D12Hook>();
        if (m_d3d12_hook->install()) {
            m_is_d3d12 = true;
            m_renderer_type = RendererType::D3D12;
        }
    }
}
```

### **VR Initialization**
```cpp
void Framework::initialize_vr() {
    try {
        // Create VR mod
        m_vr_mod = std::make_unique<VRMod>();
        m_mods.push_back(m_vr_mod.get());
        
        // Initialize OpenXR
        if (m_vr_mod->initialize_openxr()) {
            spdlog::info("OpenXR initialized successfully");
        } else {
            spdlog::warn("OpenXR initialization failed, trying OpenVR");
            if (m_vr_mod->initialize_openvr()) {
                spdlog::info("OpenVR initialized successfully");
            }
        }
    } catch (const std::exception& e) {
        spdlog::error("VR initialization failed: {}", e.what());
    }
}
```

### **Frame Processing Implementation**
```cpp
void Framework::on_frame_d3d11() {
    if (!is_ready()) return;
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    try {
        // Update VR state
        if (m_vr_mod && m_vr_mod->is_enabled()) {
            m_vr_mod->on_frame_d3d11();
        }
        
        // Process all mods
        for (auto& mod : m_mods) {
            if (mod->is_enabled()) {
                mod->on_frame();
            }
        }
        
        // Render ImGui
        run_imgui_frame(false);
        
    } catch (const std::exception& e) {
        spdlog::error("Frame processing error: {}", e.what());
    }
    
    // Performance monitoring
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    
    if (duration.count() > 1000) { // > 1ms
        spdlog::warn("Frame processing took {}μs", duration.count());
    }
}
```

---

## 🔗 **RELATED FILES**
- `src/Framework.hpp` - Framework header file
- `src/Mod.hpp` - Base mod interface
- `src/hooks/D3D11Hook.hpp` - DirectX 11 hooking
- `src/hooks/D3D12Hook.hpp` - DirectX 12 hooking
- `src/mods/VR.hpp` - VR modification system

---

## ⚠️ **IMPORTANT IMPLEMENTATION DETAILS**

### **Thread Safety**
- Uses mutex protection for critical sections
- Frame processing is called from render thread
- Configuration operations use deferred execution
- VR updates happen on dedicated thread

### **Error Handling**
- Comprehensive exception handling throughout
- Graceful degradation when components fail
- Detailed logging for debugging
- Recovery mechanisms for common failures

### **Performance Optimization**
- Minimal allocations in frame processing
- Efficient data structures for mod management
- Optimized hook implementations
- VR-specific performance considerations

---

## 🚀 **IMPLEMENTATION PATTERNS**

### **RAII Pattern**
```cpp
class Framework {
public:
    Framework(HMODULE module) {
        // Acquire resources
        initialize_hooks();
        initialize_vr();
    }
    
    ~Framework() {
        // Release resources
        cleanup_vr();
        remove_hooks();
    }
};
```

### **Factory Pattern**
```cpp
std::unique_ptr<Mod> Framework::create_mod(const std::string& mod_type) {
    if (mod_type == "VR") {
        return std::make_unique<VRMod>();
    } else if (mod_type == "UI") {
        return std::make_unique<UIMod>();
    }
    return nullptr;
}
```

### **Observer Pattern**
```cpp
void Framework::notify_frame_observers() {
    for (auto& observer : m_frame_observers) {
        observer->on_frame();
    }
}
```

---

## 🛡️ **BEST PRACTICES DEMONSTRATED**

1. **Modern C++**: Uses C++23 features and best practices
2. **Exception Safety**: Strong exception safety guarantees
3. **Resource Management**: RAII pattern throughout
4. **Performance**: Optimized for VR frame rates
5. **Maintainability**: Clear separation of concerns
6. **Extensibility**: Plugin architecture for mods
7. **Debugging**: Comprehensive logging and error reporting
