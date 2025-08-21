#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <thread>
#include <atomic>
#include <mutex>
#include <windows.h>
#include <d3d11.h>
#include <d3d12.h>
#include <dxgi.h>

// Core system includes
#include "../core/Framework.hpp"
#include "../core/PluginManager.hpp"

namespace uevr {

/**
 * @brief VR Injection System
 * 
 * Handles the core VR injection functionality:
 * - Process injection and DLL loading
 * - DirectX/OpenGL hooking
 * - Shader injection and modification
 * - Input redirection
 * - Memory management and safety
 */
class VRInjectionSystem {
public:
    // Singleton pattern
    static VRInjectionSystem& getInstance();
    
    // Disable copy and assignment
    VRInjectionSystem(const VRInjectionSystem&) = delete;
    VRInjectionSystem& operator=(const VRInjectionSystem&) = delete;

    // Core system lifecycle
    bool initialize();
    void shutdown();
    bool isInitialized() const { return m_initialized.load(); }

    // Process injection
    struct InjectionTarget {
        DWORD processId;
        std::string processName;
        std::string executablePath;
        std::string engineType;
        bool is64Bit;
        bool isElevated;
        std::unordered_map<std::string, std::string> metadata;
        
        InjectionTarget() : processId(0), is64Bit(false), isElevated(false) {}
    };

    bool injectIntoProcess(const InjectionTarget& target);
    bool injectIntoProcessById(DWORD processId);
    bool injectIntoProcessByName(const std::string& processName);
    bool ejectFromProcess(DWORD processId);
    std::vector<InjectionTarget> findInjectionTargets();
    bool isProcessInjected(DWORD processId) const;

    // Memory hooking
    struct HookInfo {
        std::string hookName;
        void* originalFunction;
        void* hookFunction;
        void* trampoline;
        bool isActive;
        std::string description;
        
        HookInfo() : originalFunction(nullptr), hookFunction(nullptr), 
                     trampoline(nullptr), isActive(false) {}
    };

    bool installHook(const std::string& hookName, void* originalFunction, void* hookFunction);
    bool removeHook(const std::string& hookName);
    bool enableHook(const std::string& hookName);
    bool disableHook(const std::string& hookName);
    std::vector<HookInfo> getActiveHooks() const;

    // DirectX hooking
    bool hookDirectX11();
    bool hookDirectX12();
    bool hookDXGI();
    bool hookOpenGL();
    bool hookVulkan();

    // Shader injection
    struct ShaderInfo {
        std::string shaderName;
        std::string shaderType; // "vertex", "pixel", "compute", etc.
        std::string originalShader;
        std::string modifiedShader;
        std::string injectionPoint;
        bool isActive;
        
        ShaderInfo() : isActive(false) {}
    };

    bool injectShader(const ShaderInfo& shaderInfo);
    bool removeShader(const std::string& shaderName);
    bool modifyShader(const std::string& shaderName, const std::string& newShader);
    std::vector<ShaderInfo> getInjectedShaders() const;

    // VR-specific shaders
    bool injectStereoscopicShader();
    bool injectDepthExtractionShader();
    bool injectVRPostProcessShader();
    bool injectFoveatedRenderingShader();

    // Input redirection
    struct InputMapping {
        std::string gameInput;
        std::string vrInput;
        std::string mappingType; // "keyboard", "mouse", "controller", "motion"
        float sensitivity;
        bool isInverted;
        std::string description;
        
        InputMapping() : sensitivity(1.0f), isInverted(false) {}
    };

    bool setupInputMapping(const std::vector<InputMapping>& mappings);
    bool redirectInput(const std::string& gameInput, const std::string& vrInput);
    bool clearInputMappings();
    std::vector<InputMapping> getInputMappings() const;

    // Memory management
    struct MemoryRegion {
        void* address;
        size_t size;
        std::string description;
        bool isProtected;
        bool isReadable;
        bool isWritable;
        bool isExecutable;
        
        MemoryRegion() : address(nullptr), size(0), isProtected(false),
                        isReadable(false), isWritable(false), isExecutable(false) {}
    };

    bool allocateMemory(size_t size, void** address);
    bool freeMemory(void* address);
    bool protectMemory(void* address, size_t size, DWORD protection);
    bool readMemory(void* address, void* buffer, size_t size);
    bool writeMemory(void* address, const void* buffer, size_t size);
    std::vector<MemoryRegion> getMemoryRegions() const;

    // Safety and validation
    struct SafetyCheck {
        std::string checkName;
        bool passed;
        std::string description;
        std::string recommendation;
        
        SafetyCheck() : passed(false) {}
    };

    std::vector<SafetyCheck> performSafetyChecks();
    bool validateInjectionTarget(const InjectionTarget& target);
    bool validateHook(const HookInfo& hook);
    bool validateShader(const ShaderInfo& shader);
    bool isSystemSafe() const;

    // Error handling
    struct InjectionError {
        std::string errorCode;
        std::string errorMessage;
        std::string processName;
        DWORD processId;
        std::string timestamp;
        std::string suggestedFix;
        
        InjectionError() : processId(0) {}
    };

    std::vector<InjectionError> getRecentErrors() const;
    void clearErrors();
    bool hasErrors() const;

    // Performance monitoring
    struct InjectionMetrics {
        int totalInjections;
        int successfulInjections;
        int failedInjections;
        double averageInjectionTime;
        double averageHookTime;
        double averageShaderInjectionTime;
        int activeHooks;
        int activeShaders;
        double memoryUsage;
        
        InjectionMetrics() : totalInjections(0), successfulInjections(0), 
                           failedInjections(0), averageInjectionTime(0.0),
                           averageHookTime(0.0), averageShaderInjectionTime(0.0),
                           activeHooks(0), activeShaders(0), memoryUsage(0.0) {}
    };

    InjectionMetrics getMetrics() const;
    void resetMetrics();

private:
    VRInjectionSystem();
    ~VRInjectionSystem();

    // Internal data structures
    std::atomic<bool> m_initialized{false};
    std::mutex m_mutex;
    std::condition_variable m_cv;
    
    // Core components
    std::unique_ptr<Framework> m_framework;
    std::unique_ptr<PluginManager> m_pluginManager;
    
    // Injection data
    std::unordered_map<DWORD, InjectionTarget> m_injectedProcesses;
    std::unordered_map<std::string, HookInfo> m_activeHooks;
    std::unordered_map<std::string, ShaderInfo> m_injectedShaders;
    std::vector<InputMapping> m_inputMappings;
    std::vector<MemoryRegion> m_allocatedMemory;
    std::vector<InjectionError> m_recentErrors;
    
    // Performance tracking
    InjectionMetrics m_metrics;
    mutable std::mutex m_metricsMutex;
    
    // Safety and validation
    std::atomic<bool> m_systemSafe{false};
    std::vector<SafetyCheck> m_lastSafetyChecks;
    
    // Internal methods
    bool initializeHooking();
    bool initializeShaderInjection();
    bool initializeInputRedirection();
    bool initializeMemoryManagement();
    
    // Hook implementations
    bool installDirectX11Hooks();
    bool installDirectX12Hooks();
    bool installDXGIHooks();
    bool installOpenGLHooks();
    bool installVulkanHooks();
    
    // Shader implementations
    bool compileShader(const std::string& shaderSource, const std::string& shaderType, void** shader);
    bool injectShaderIntoProcess(DWORD processId, const ShaderInfo& shader);
    bool modifyShaderInMemory(void* shaderAddress, const std::string& newShader);
    
    // Input implementations
    bool setupKeyboardMapping();
    bool setupMouseMapping();
    bool setupControllerMapping();
    bool setupMotionMapping();
    
    // Memory implementations
    bool allocateProcessMemory(DWORD processId, size_t size, void** address);
    bool freeProcessMemory(DWORD processId, void* address);
    bool readProcessMemory(DWORD processId, void* address, void* buffer, size_t size);
    bool writeProcessMemory(DWORD processId, void* address, const void* buffer, size_t size);
    
    // Safety implementations
    bool checkProcessCompatibility(const InjectionTarget& target);
    bool checkMemorySafety(void* address, size_t size);
    bool checkHookSafety(const HookInfo& hook);
    bool checkShaderSafety(const ShaderInfo& shader);
    
    // Error handling
    void logError(const std::string& errorCode, const std::string& errorMessage, 
                  const std::string& processName = "", DWORD processId = 0);
    void updateMetrics(const std::string& operation, double time, bool success);
    
    // Utility methods
    std::string getProcessName(DWORD processId);
    bool isProcess64Bit(DWORD processId);
    bool isProcessElevated(DWORD processId);
    std::string detectEngineType(const std::string& executablePath);
    void cleanupResources();
};

} // namespace uevr
