#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>

// OpenGL function pointer types
typedef void (APIENTRY *PFNGLCLEARPROC)(GLbitfield mask);
typedef void (APIENTRY *PFNGLDRAWELEMENTSBASEVERTEXPROC)(GLenum mode, GLsizei count, GLenum type, const void *indices, GLint basevertex);
typedef void (APIENTRY *PFNGLDRAWARRAYSPROC)(GLenum mode, GLint first, GLsizei count);
typedef void (APIENTRY *PFNGLVIEWPORTPROC)(GLint x, GLint y, GLsizei width, GLsizei height);
typedef void (APIENTRY *PFNGLSCISSORPROC)(GLint x, GLint y, GLsizei width, GLsizei height);
typedef void (APIENTRY *PFNGLFINISHPROC)(void);
typedef void (APIENTRY *PFNGLFLUSHPROC)(void);
typedef void (APIENTRY *PFNGLBINDTEXTUREPROC)(GLenum target, GLuint texture);
typedef void (APIENTRY *PFNGLBINDFRAMEBUFFERPROC)(GLenum target, GLuint framebuffer);
typedef void (APIENTRY *PFNGLBINDVERTEXARRAYPROC)(GLuint array);
typedef void (APIENTRY *PFNGLUSEPROGRAMPROC)(GLuint program);

namespace uevr {
namespace Hooks {

// OpenGL version enumeration
enum class OpenGLVersion {
    UNKNOWN = 0,
    OPENGL_1_0 = 10,
    OPENGL_1_1 = 11,
    OPENGL_1_2 = 12,
    OPENGL_1_3 = 13,
    OPENGL_1_4 = 14,
    OPENGL_1_5 = 15,
    OPENGL_2_0 = 20,
    OPENGL_2_1 = 21,
    OPENGL_3_0 = 30,
    OPENGL_3_1 = 31,
    OPENGL_3_2 = 32,
    OPENGL_3_3 = 33,
    OPENGL_4_0 = 40,
    OPENGL_4_1 = 41,
    OPENGL_4_2 = 42,
    OPENGL_4_3 = 43,
    OPENGL_4_4 = 44,
    OPENGL_4_5 = 45,
    OPENGL_4_6 = 46
};

// Hook state enumeration
enum class HookState {
    UNINSTALLED = 0,
    INSTALLING,
    INSTALLED,
    FAILED,
    REMOVING
};

// Hook type enumeration
enum class HookType {
    CLEAR = 0,
    DRAW_ELEMENTS,
    DRAW_ARRAYS,
    VIEWPORT,
    SCISSOR,
    FINISH,
    FLUSH,
    BIND_TEXTURE,
    BIND_FRAMEBUFFER,
    BIND_VERTEX_ARRAY,
    USE_PROGRAM,
    CUSTOM
};

// Hook callback function type
using OpenGLHookCallback = std::function<void(const void*, void*)>;

// Hook information structure
struct OpenGLHookInfo {
    HookType type;
    std::string name;
    std::string description;
    OpenGLHookCallback callback;
    bool enabled;
    bool critical;
    std::chrono::steady_clock::time_point install_time;
    std::chrono::steady_clock::time_point last_call_time;
    uint64_t call_count;
    double average_execution_time;
};

// OpenGL context data
struct OpenGLContextData {
    HDC device_context;
    HGLRC rendering_context;
    GLuint current_program;
    GLuint current_texture;
    GLuint current_framebuffer;
    GLuint current_vertex_array;
    GLint viewport[4];
    GLint scissor[4];
    GLenum current_draw_mode;
    bool vsync_enabled;
    bool double_buffered;
    int color_bits;
    int depth_bits;
    int stencil_bits;
    int multisample_samples;
};

// OpenGL hooks configuration
struct OpenGLHooksConfig {
    bool enable_clear_hooks = true;
    bool enable_draw_hooks = true;
    bool enable_state_hooks = true;
    bool enable_texture_hooks = true;
    bool enable_framebuffer_hooks = true;
    bool enable_shader_hooks = true;
    bool enable_debug_hooks = false;
    bool enable_performance_monitoring = true;
    bool enable_error_logging = true;
    bool enable_call_tracking = true;
    uint32_t max_hook_count = 1000;
    uint32_t hook_timeout_ms = 5000;
    std::string log_file_path = "uevr_opengl_hooks.log";
};

// OpenGL hooks statistics
struct OpenGLHooksStats {
    uint64_t total_hooks_installed = 0;
    uint64_t total_hooks_removed = 0;
    uint64_t total_hook_calls = 0;
    uint64_t failed_hook_installations = 0;
    uint64_t failed_hook_removals = 0;
    double average_hook_installation_time = 0.0;
    double average_hook_execution_time = 0.0;
    std::chrono::steady_clock::time_point first_hook_time;
    std::chrono::steady_clock::time_point last_hook_time;
    std::unordered_map<HookType, uint64_t> hook_type_call_counts;
    std::unordered_map<std::string, uint64_t> hook_name_call_counts;
};

/**
 * @brief OpenGL Hooks Manager
 * 
 * This class manages all OpenGL hooks for the uevr framework.
 * It provides a comprehensive hooking system for intercepting OpenGL calls,
 * enabling VR modifications, performance monitoring, and debugging capabilities.
 * 
 * Features:
 * - OpenGL 1.0 to 4.6 support
 * - Multiple hook types (Clear, Draw, State, etc.)
 * - Performance monitoring and statistics
 * - Error handling and logging
 * - Hook validation and safety checks
 * - Custom hook callbacks
 * - Thread-safe operations
 */
class OpenGLHooks {
public:
    // Constructor and destructor
    explicit OpenGLHooks(const OpenGLHooksConfig& config = OpenGLHooksConfig{});
    ~OpenGLHooks();
    
    // Copy and move operations
    OpenGLHooks(const OpenGLHooks&) = delete;
    OpenGLHooks& operator=(const OpenGLHooks&) = delete;
    OpenGLHooks(OpenGLHooks&&) noexcept;
    OpenGLHooks& operator=(OpenGLHooks&&) noexcept;

    // Core hook management
    bool installHooks();
    void removeHooks();
    bool isHooksInstalled() const;
    HookState getHookState() const;
    
    // OpenGL version detection
    OpenGLVersion detectOpenGLVersion() const;
    std::string getOpenGLVersionString() const;
    bool isOpenGLVersionSupported(OpenGLVersion version) const;
    
    // Hook installation for specific types
    bool installClearHook();
    bool installDrawHook();
    bool installStateHook();
    bool installTextureHook();
    bool installFramebufferHook();
    bool installShaderHook();
    bool installCustomHook(HookType type, const std::string& name, OpenGLHookCallback callback);
    
    // Hook removal for specific types
    void removeClearHook();
    void removeDrawHook();
    void removeStateHook();
    void removeTextureHook();
    void removeFramebufferHook();
    void removeShaderHook();
    void removeCustomHook(const std::string& name);
    
    // Hook management
    bool enableHook(const std::string& name);
    bool disableHook(const std::string& name);
    bool isHookEnabled(const std::string& name) const;
    void enableAllHooks();
    void disableAllHooks();
    
    // Hook information and statistics
    const std::vector<OpenGLHookInfo>& getInstalledHooks() const;
    const OpenGLHookInfo* getHookInfo(const std::string& name) const;
    const OpenGLHooksStats& getStatistics() const;
    void resetStatistics();
    
    // Configuration management
    const OpenGLHooksConfig& getConfig() const;
    void updateConfig(const OpenGLHooksConfig& config);
    void resetConfig();
    
    // Performance monitoring
    void enablePerformanceMonitoring(bool enable);
    bool isPerformanceMonitoringEnabled() const;
    void setPerformanceThreshold(double threshold_ms);
    double getPerformanceThreshold() const;
    
    // Error handling and logging
    const std::string& getLastError() const;
    void clearLastError();
    std::vector<std::string> getErrorHistory() const;
    void enableErrorLogging(bool enable);
    bool isErrorLoggingEnabled() const;
    
    // Debug and development features
    void enableDebugMode(bool enable);
    bool isDebugModeEnabled() const;
    void setLogLevel(int level);
    int getLogLevel() const;
    void dumpHookState() const;
    
    // OpenGL specific data access
    const OpenGLContextData* getContextData() const;
    bool hasContextData() const;
    
    // Hook validation and safety
    bool validateHooks() const;
    bool checkHookIntegrity() const;
    void performSafetyChecks();
    
    // Utility functions
    std::string getHookStateString() const;
    std::string getStatisticsReport() const;
    void exportStatistics(const std::string& file_path) const;

protected:
    // Internal hook management
    bool installOpenGLHooks();
    void removeOpenGLHooks();
    
    // Hook validation
    bool validateHookInstallation() const;
    bool validateHookRemoval() const;
    
    // Statistics management
    void updateStatistics(const OpenGLHookInfo& hook, double execution_time);
    void logHookCall(const OpenGLHookInfo& hook);
    
    // Error management
    void setLastError(const std::string& error);
    void logError(const std::string& error);
    
    // Internal utilities
    bool initializeMinHook();
    void cleanupMinHook();
    bool isMinHookInitialized() const;

private:
    // Configuration and state
    OpenGLHooksConfig m_config;
    HookState m_hook_state;
    OpenGLVersion m_opengl_version;
    
    // Hook storage
    std::vector<OpenGLHookInfo> m_installed_hooks;
    std::unordered_map<std::string, size_t> m_hook_name_map;
    
    // OpenGL data
    std::unique_ptr<OpenGLContextData> m_context_data;
    
    // Statistics and monitoring
    OpenGLHooksStats m_stats;
    bool m_performance_monitoring_enabled;
    double m_performance_threshold;
    
    // Error handling
    std::string m_last_error;
    std::vector<std::string> m_error_history;
    bool m_error_logging_enabled;
    
    // Debug and development
    bool m_debug_mode;
    int m_log_level;
    
    // Internal state
    bool m_minhook_initialized;
    bool m_hooks_installed;
    std::chrono::steady_clock::time_point m_initialization_time;
    
    // Private helper methods
    void initializeDefaultHooks();
    void cleanupResources();
    bool installHookInternal(const OpenGLHookInfo& hook_info);
    void removeHookInternal(const std::string& name);
    void updateHookState(HookState new_state);
    bool validateConfig() const;
    void logInfo(const std::string& message) const;
    void logWarning(const std::string& message) const;
    void logError(const std::string& message) const;
    void logDebug(const std::string& message) const;
};

} // namespace Hooks
} // namespace uevr
