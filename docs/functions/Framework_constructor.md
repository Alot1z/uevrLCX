# Framework Constructor Documentation

## 📋 **FUNCTION OVERVIEW**

**Function**: `Framework::Framework(HMODULE framework_module)`  
**File**: `src/Framework.cpp`  
**Purpose**: Initialize the main UEVR framework with DLL module handle  
**Category**: Core Framework Initialization  

---

## 🎯 **FUNCTION SIGNATURE**

```cpp
Framework::Framework(HMODULE framework_module)
```

### **Parameters**
- `framework_module` (HMODULE): Handle to the UEVR DLL module

### **Return Value**
- None (Constructor)

---

## 🔧 **IMPLEMENTATION DETAILS**

### **Core Initialization Steps**
1. **Store Module Handle**: Saves the framework module handle
2. **Initialize Logging**: Sets up spdlog logging system
3. **Setup Hooks**: Installs DirectX and Windows API hooks
4. **Initialize VR**: Prepares VR runtime integration
5. **Load Configuration**: Reads configuration from persistent storage
6. **Start Monitoring**: Begins frame monitoring and input handling

### **Key Components Initialized**
- **DirectX Hooks**: D3D11/D3D12 rendering hooks
- **Input System**: Keyboard and mouse input handling
- **VR Runtime**: OpenXR/OpenVR integration
- **Plugin System**: Plugin loading and management
- **Configuration**: Settings and profile management

---

## 📚 **USAGE EXAMPLE**

```cpp
// Called from DllMain startup thread
void startup_thread(HMODULE poc_module) {
    g_framework = std::make_unique<Framework>(poc_module);
}
```

---

## 🔗 **RELATED FUNCTIONS**
- `Framework::~Framework()` - Destructor and cleanup
- `Framework::is_valid()` - Check initialization status
- `Framework::initialize()` - Secondary initialization
- `Framework::hook_monitor()` - Frame monitoring system

---

## ⚠️ **IMPORTANT NOTES**
- Must be called from a separate thread (not DllMain)
- Requires valid HMODULE handle
- Initializes global framework instance
- Critical for all subsequent UEVR functionality

---

## 🚀 **IMPLEMENTATION GUIDE**

To implement this in your own project:

1. **Get Module Handle**: Obtain HMODULE from DllMain
2. **Create Framework**: Instantiate Framework with module handle
3. **Check Validity**: Verify initialization with is_valid()
4. **Start Operations**: Begin VR operations and game hooks

```cpp
// Example implementation
HMODULE my_module = GetModuleHandle(L"MyVRMod.dll");
auto framework = std::make_unique<Framework>(my_module);
if (framework->is_valid()) {
    // Framework ready for use
}
```
