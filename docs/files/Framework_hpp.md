# Framework.hpp File Documentation

## 📋 **FILE OVERVIEW**

**File**: `src/Framework.hpp`  
**Size**: 454 lines  
**Language**: C++23  
**Purpose**: Core UEVR framework interface definition  
**Category**: Core System Header  

---

## 🎯 **FILE STRUCTURE**

### **Header Guards and Includes**
```cpp
#pragma once

#include <windows.h>
#include <memory>
#include <vector>
#include <mutex>
#include <thread>
#include <chrono>
#include <filesystem>
#include <array>

#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

#include "utility/Address.hpp"
#include "utility/Thread.hpp"
```

### **Forward Declarations**
```cpp
class Mod;
class VRMod;
class D3D11Hook;
class D3D12Hook;
class WindowsMessageHook;
class DInputHook;

namespace UEVRSharedMemory {
    enum class Command : uint32_t;
}
```

---

## 🔧 **CLASS DEFINITIONS**

### **RendererType Enumeration**
```cpp
enum class RendererType : uint8_t {
    D3D11,
    D3D12,
    OpenGL,
    Vulkan,
    Unknown
};
```

### **Framework Class Declaration**
```cpp
class Framework {
private:
    // Core state members
    HMODULE m_framework_module{nullptr};
    Address m_game_module{0};
    bool m_valid{false};
    bool m_initialized{false};
    bool m_game_data_initialized{false};
    
    // Renderer state
    bool m_is_d3d11{false};
    bool m_is_d3d12{false};
    RendererType m_renderer_type{RendererType::Unknown};
    
    // Hook management
    std::unique_ptr<D3D11Hook> m_d3d11_hook;
    std::unique_ptr<D3D12Hook> m_d3d12_hook;
    std::unique_ptr<WindowsMessageHook> m_windows_message_hook;
    std::unique_ptr<DInputHook> m_dinput_hook;
    
    // Mod system
    std::vector<std::unique_ptr<Mod>> m_mods;
    VRMod* m_vr_mod{nullptr};
    
    // Threading
    std::unique_ptr<FrameWorker> m_frame_worker;
    std::mutex m_hook_monitor_mutex;
    std::thread m_hook_monitor_thread;
    
    // Input state
    std::array<uint8_t, 256> m_last_keys{};
    Vector2f m_mouse_delta{};
    
public:
    // Constructor and destructor
    Framework(HMODULE framework_module);
    virtual ~Framework();
    
    // Core functionality
    bool is_valid() const { return m_valid; }
    bool is_ready() const { return m_initialized && m_game_data_initialized; }
    bool is_game_data_intialized() const { return m_game_data_initialized; }
    
    // Renderer queries
    bool is_dx11() const { return m_is_d3d11; }
    bool is_dx12() const { return m_is_d3d12; }
    RendererType get_renderer_type() const { return m_renderer_type; }
    
    // Module access
    auto get_framework_module() const { return m_framework_module; }
    Address get_module() const { return m_game_module; }
    
    // Mod system access
    const auto& get_mods() const { return m_mods; }
    
    // Input access
    const auto& get_mouse_delta() const { return m_mouse_delta; }
    const auto& get_keyboard_state() const { return m_last_keys; }
    
    // Frame processing
    void run_imgui_frame(bool from_present);
    void on_frame_d3d11();
    void on_post_present_d3d11();
    void on_frame_d3d12();
    void on_post_present_d3d12();
    void on_reset(uint32_t w, uint32_t h);
    
    // Window management
    void activate_window();
    void set_mouse_to_center();
    void patch_set_cursor_pos();
    void remove_set_cursor_pos_patch();
    void post_message(UINT message, WPARAM w_param, LPARAM l_param);
    bool on_message(HWND wnd, UINT message, WPARAM w_param, LPARAM l_param);
    
    // Command processing
    void on_frontend_command(UEVRSharedMemory::Command command);
    void on_direct_input_keys(const std::array<uint8_t, 256>& keys);
    
    // Configuration management
    static std::filesystem::path get_persistent_dir();
    static std::filesystem::path get_persistent_dir(const std::string& dir);
    
    void save_config();
    void reset_config();
    void reload_config();
    
    void deferred_save_config();
    void deferred_reload_config();
    
private:
    // Internal methods
    void hook_monitor();
    void command_thread();
    void initialize_logging();
    void initialize_hooks();
    void initialize_vr();
    void cleanup();
};
```

---

## 📚 **DETAILED MEMBER ANALYSIS**

### **Core State Members**
```cpp
// Framework module and validation
HMODULE m_framework_module{nullptr};    // DLL module handle
bool m_valid{false};                     // Initialization success flag
bool m_initialized{false};               // Framework initialized
bool m_game_data_initialized{false};     // Game-specific data ready

// Game module information
Address m_game_module{0};                // Main game module address
```

### **Renderer State Management**
```cpp
// Renderer detection and state
bool m_is_d3d11{false};                  // DirectX 11 detected
bool m_is_d3d12{false};                  // DirectX 12 detected
RendererType m_renderer_type{RendererType::Unknown}; // Current renderer
```

### **Hook System Architecture**
```cpp
// Graphics API hooks
std::unique_ptr<D3D11Hook> m_d3d11_hook;           // DirectX 11 hooking
std::unique_ptr<D3D12Hook> m_d3d12_hook;           // DirectX 12 hooking

// System hooks
std::unique_ptr<WindowsMessageHook> m_windows_message_hook; // Window messages
std::unique_ptr<DInputHook> m_dinput_hook;                  // DirectInput
```

### **Mod System Architecture**
```cpp
// Mod management
std::vector<std::unique_ptr<Mod>> m_mods;  // All loaded mods
VRMod* m_vr_mod{nullptr};                  // VR mod reference
```

### **Threading Architecture**
```cpp
// Thread management
std::unique_ptr<FrameWorker> m_frame_worker;    // Frame processing worker
std::mutex m_hook_monitor_mutex;                // Hook monitor synchronization
std::thread m_hook_monitor_thread;              // Hook monitoring thread
```

---

## 🔗 **RELATED FILES AND DEPENDENCIES**

### **Direct Dependencies**
- `utility/Address.hpp` - Memory address utilities
- `utility/Thread.hpp` - Threading utilities
- `Mod.hpp` - Base mod interface
- `hooks/D3D11Hook.hpp` - DirectX 11 hooking
- `hooks/D3D12Hook.hpp` - DirectX 12 hooking

### **System Dependencies**
- `windows.h` - Windows API
- `spdlog/spdlog.h` - Logging system
- `nlohmann/json.hpp` - JSON processing

---

## ⚠️ **IMPORTANT DESIGN PATTERNS**

### **RAII Pattern**
```cpp
class Framework {
public:
    Framework(HMODULE module) {
        // Acquire resources in constructor
        initialize_hooks();
        initialize_vr();
    }
    
    ~Framework() {
        // Release resources in destructor
        cleanup();
    }
};
```

### **Singleton Pattern (Global Instance)**
```cpp
// Global framework instance
extern std::unique_ptr<Framework> g_framework;
```

### **Factory Pattern (Mod Creation)**
```cpp
// Mods are created and managed by the framework
std::vector<std::unique_ptr<Mod>> m_mods;
```

### **Observer Pattern (Event Handling)**
```cpp
// Framework observes and handles various events
bool on_message(HWND wnd, UINT message, WPARAM w_param, LPARAM l_param);
void on_frontend_command(UEVRSharedMemory::Command command);
```

---

## 🚀 **IMPLEMENTATION GUIDELINES**

### **Thread Safety Considerations**
```cpp
// Thread-safe access patterns
const auto& get_mods() const { return m_mods; }     // Read-only access
std::mutex m_hook_monitor_mutex;                    // Explicit synchronization
```

### **Error Handling Strategy**
```cpp
// Validation methods for safe operation
bool is_valid() const { return m_valid; }
bool is_ready() const { return m_initialized && m_game_data_initialized; }
```

### **Resource Management**
```cpp
// Smart pointers for automatic resource management
std::unique_ptr<D3D11Hook> m_d3d11_hook;
std::unique_ptr<D3D12Hook> m_d3d12_hook;
std::vector<std::unique_ptr<Mod>> m_mods;
```

### **Configuration Management**
```cpp
// Deferred operations for performance
void deferred_save_config() {
    m_frame_worker->enqueue([this]() {
        save_config();
    });
}
```

---

## 🛡️ **BEST PRACTICES DEMONSTRATED**

1. **Modern C++ Features**: Uses C++23 features and smart pointers
2. **Exception Safety**: RAII pattern ensures proper cleanup
3. **Thread Safety**: Explicit synchronization where needed
4. **Performance**: Deferred operations for non-critical tasks
5. **Maintainability**: Clear separation of concerns
6. **Extensibility**: Plugin architecture for mods
7. **Debugging**: Comprehensive state validation methods

### **Usage Example**
```cpp
// Proper framework usage
if (g_framework && g_framework->is_valid()) {
    if (g_framework->is_ready()) {
        // Safe to use framework functionality
        g_framework->on_frame_d3d11();
    }
}
```
