#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <mutex>
#include <atomic>
#include <chrono>

namespace UEVR {

// Forward declarations
class Logger;
class ConfigurationManager;

/**
 * @brief Hook types
 */
enum class HookType {
    DETOUR,         // Function detour/redirect
    INLINE,         // Inline hook
    IAT,            // Import Address Table hook
    EAT,            // Export Address Table hook
    VTABLE,         // Virtual table hook
    MEMORY_PATCH    // Direct memory patching
};

/**
 * @brief Hook status
 */
enum class HookStatus {
    INACTIVE,       // Hook not installed
    ACTIVE,         // Hook installed and active
    DISABLED,       // Hook installed but disabled
    ERROR,          // Hook installation failed
    PENDING         // Hook installation pending
};

/**
 * @brief Hook information structure
 */
struct HookInfo {
    std::string name;
    std::string description;
    HookType type;
    HookStatus status;
    void* targetAddress;
    void* detourFunction;
    void* originalFunction;
    std::string moduleName;
    std::string functionName;
    size_t installCount;
    size_t callCount;
    std::chrono::system_clock::time_point lastCall;
    std::chrono::system_clock::time_point installTime;
    bool enabled;
    int priority;
    
    HookInfo() : 
        type(HookType::DETOUR), 
        status(HookStatus::INACTIVE),
        targetAddress(nullptr),
        detourFunction(nullptr),
        originalFunction(nullptr),
        installCount(0),
        callCount(0),
        enabled(true),
        priority(0) {}
};

/**
 * @brief Hook callback function type
 */
using HookCallback = std::function<void(const std::string& hookName, void* context)>;

/**
 * @brief Hook filter function type
 */
using HookFilter = std::function<bool(void* returnAddress, void* context)>;

/**
 * @brief Hook manager class
 * 
 * Manages function hooks for intercepting and modifying game behavior
 */
class HookManager {
public:
    /**
     * @brief Constructor
     */
    HookManager();
    
    /**
     * @brief Destructor
     */
    ~HookManager();
    
    /**
     * @brief Initialize the hook manager
     * @param configManager Configuration manager instance
     * @param logger Logger instance
     * @return true if initialization successful, false otherwise
     */
    bool Initialize(std::shared_ptr<ConfigurationManager> configManager, std::shared_ptr<Logger> logger);
    
    /**
     * @brief Shutdown the hook manager
     */
    void Shutdown();
    
    /**
     * @brief Install a function hook
     * @param name Hook name (unique identifier)
     * @param targetAddress Target function address
     * @param detourFunction Detour function address
     * @param originalFunction Pointer to store original function address
     * @param type Hook type
     * @return true if hook installed successfully, false otherwise
     */
    bool InstallHook(const std::string& name, void* targetAddress, void* detourFunction, 
                     void** originalFunction, HookType type = HookType::DETOUR);
    
    /**
     * @brief Install a hook by module and function name
     * @param name Hook name (unique identifier)
     * @param moduleName Target module name
     * @param functionName Target function name
     * @param detourFunction Detour function address
     * @param originalFunction Pointer to store original function address
     * @param type Hook type
     * @return true if hook installed successfully, false otherwise
     */
    bool InstallHookByName(const std::string& name, const std::string& moduleName, 
                          const std::string& functionName, void* detourFunction, 
                          void** originalFunction, HookType type = HookType::DETOUR);
    
    /**
     * @brief Remove a hook
     * @param name Hook name
     * @return true if hook removed successfully, false otherwise
     */
    bool RemoveHook(const std::string& name);
    
    /**
     * @brief Enable a hook
     * @param name Hook name
     * @return true if hook enabled successfully, false otherwise
     */
    bool EnableHook(const std::string& name);
    
    /**
     * @brief Disable a hook
     * @param name Hook name
     * @return true if hook disabled successfully, false otherwise
     */
    bool DisableHook(const std::string& name);
    
    /**
     * @brief Enable all hooks
     * @return Number of hooks enabled
     */
    int EnableAllHooks();
    
    /**
     * @brief Disable all hooks
     * @return Number of hooks disabled
     */
    int DisableAllHooks();
    
    /**
     * @brief Check if hook is installed
     * @param name Hook name
     * @return true if hook is installed, false otherwise
     */
    bool IsHookInstalled(const std::string& name) const;
    
    /**
     * @brief Check if hook is enabled
     * @param name Hook name
     * @return true if hook is enabled, false otherwise
     */
    bool IsHookEnabled(const std::string& name) const;
    
    /**
     * @brief Get hook information
     * @param name Hook name
     * @return Hook information or nullptr if not found
     */
    std::shared_ptr<HookInfo> GetHookInfo(const std::string& name) const;
    
    /**
     * @brief Get all hook information
     * @return Vector of hook information
     */
    std::vector<std::shared_ptr<HookInfo>> GetAllHookInfo() const;
    
    /**
     * @brief Get hook statistics
     * @return Hook statistics as JSON string
     */
    std::string GetHookStatistics() const;
    
    /**
     * @brief Get function address by name
     * @param moduleName Module name
     * @param functionName Function name
     * @return Function address or nullptr if not found
     */
    void* GetFunctionAddress(const std::string& moduleName, const std::string& functionName) const;
    
    /**
     * @brief Get module base address
     * @param moduleName Module name
     * @return Module base address or nullptr if not found
     */
    void* GetModuleBaseAddress(const std::string& moduleName) const;
    
    /**
     * @brief Find pattern in memory
     * @param pattern Byte pattern to search for
     * @param mask Pattern mask (e.g., "xxx?xx?x")
     * @param startAddress Start search address
     * @param searchSize Search size in bytes
     * @return Address of found pattern or nullptr if not found
     */
    void* FindPattern(const std::vector<uint8_t>& pattern, const std::string& mask, 
                     void* startAddress, size_t searchSize) const;
    
    /**
     * @brief Read memory safely
     * @param address Memory address to read from
     * @param buffer Buffer to read into
     * @param size Number of bytes to read
     * @return true if read successful, false otherwise
     */
    bool ReadMemory(void* address, void* buffer, size_t size) const;
    
    /**
     * @brief Write memory safely
     * @param address Memory address to write to
     * @param buffer Buffer to write from
     * @param size Number of bytes to write
     * @return true if write successful, false otherwise
     */
    bool WriteMemory(void* address, const void* buffer, size_t size) const;

private:
    // Internal structures
    struct HookEntry {
        std::shared_ptr<HookInfo> info;
        HookCallback callback;
        HookFilter filter;
        std::vector<std::string> errors;
        void* trampoline;
        
        HookEntry() : trampoline(nullptr) {}
    };
    
    // Member variables
    mutable std::mutex m_mutex;
    std::unordered_map<std::string, std::unique_ptr<HookEntry>> m_hooks;
    std::shared_ptr<ConfigurationManager> m_configManager;
    std::shared_ptr<Logger> m_logger;
    std::atomic<bool> m_initialized;
    
    // Internal methods
    void LogHookEvent(const std::string& hookName, const std::string& event, const std::string& details = "");
    bool IsAddressValid(void* address) const;
    void UpdateHookStatistics(const std::string& hookName);
    
    // Configuration keys
    static constexpr const char* CONFIG_SECTION = "HookManager";
    static constexpr const char* CONFIG_ENABLED_HOOKS = "EnabledHooks";
    static constexpr const char* CONFIG_HOOK_PRIORITIES = "HookPriorities";
    static constexpr const char* CONFIG_SAFETY_CHECKS = "SafetyChecks";
};

} // namespace UEVR