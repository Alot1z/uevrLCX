#pragma once

#include <memory>
#include <chrono>
#include <mutex>
#include <windows.h>
#include <spdlog/spdlog.h>

namespace uevr {

class Framework {
public:
    enum class RendererType {
        D3D11,
        D3D12,
        OpenGL,
        Vulkan,
        Unknown
    };

    Framework();
    ~Framework();

    // Core functionality
    bool initialize();
    void run();
    void cleanup();
    bool isInitialized() const;

    // Renderer management
    RendererType getRendererType() const;
    void setRendererType(RendererType type);

    // Window management
    HWND getWindow() const;
    void setWindow(HWND window);

    // Hook management
    bool installHooks();
    void removeHooks();

    // Configuration
    void loadConfig();
    void saveConfig();

    // VR integration
    bool enableVR();
    bool disableVR();
    bool isVREnabled() const;

private:
    // Core state
    bool m_initialized;
    HWND m_wnd;
    RendererType m_renderer_type;

    // Timing and monitoring
    std::chrono::steady_clock::time_point m_last_present_time;
    std::chrono::steady_clock::time_point m_last_chance_time;
    bool m_has_last_chance;
    std::mutex m_hook_monitor_mutex;

    // Internal methods
    void hook_monitor();
    void initialize_logging();
    void initialize_imgui();
    void cleanup_imgui();
};

// Global framework instance
extern std::unique_ptr<Framework> g_framework;

} // namespace uevr

// Backward compatibility alias
namespace uevr = uevr;
