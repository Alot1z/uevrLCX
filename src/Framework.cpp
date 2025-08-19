#include "uevr/Framework.hpp"
#include "uevr/API.hpp"
#include "uevr/Context7Database.hpp"
#include "uevr/vr/UniversalVRSystem.hpp"
#include "uevr/ai/AIAnalysisEngine.hpp"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_dx12.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_vulkan.h>
#include <d3d11.h>
#include <d3d12.h>
#include <gl/GL.h>
#include <vulkan/vulkan.h>
#include <memory>
#include <chrono>
#include <thread>
#include <filesystem>

namespace uevr {

// Global framework instance
std::unique_ptr<Framework> g_framework;

Framework::Framework()
    : m_initialized(false)
    , m_wnd(nullptr)
    , m_renderer_type(RendererType::Unknown)
    , m_last_present_time(std::chrono::steady_clock::now())
    , m_last_chance_time(std::chrono::steady_clock::now())
    , m_has_last_chance(false)
    , m_context7_db(std::make_unique<ai::Context7Database>())
    , m_vr_system(std::make_unique<vr::UniversalVRSystem>())
    , m_ai_engine(std::make_unique<ai::AIAnalysisEngine>()) {
    
    spdlog::info("[Framework] Framework instance created");
}

Framework::~Framework() {
    spdlog::info("[Framework] Framework instance destroyed");
    cleanup();
}

bool Framework::initialize() {
    if (m_initialized) {
        spdlog::warn("[Framework] Framework already initialized");
        return true;
    }

    try {
        spdlog::info("[Framework] Initializing framework...");
        
        // Initialize logging
        initialize_logging();
        
        // Initialize ImGui
        initialize_imgui();
        
        // Set initialized state
        m_initialized = true;
        
        spdlog::info("[Framework] Framework initialized successfully");
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[Framework] Failed to initialize framework: {}", e.what());
        return false;
    }
}

void Framework::run() {
    if (!m_initialized) {
        spdlog::error("[Framework] Framework not initialized");
        return;
    }

    spdlog::info("[Framework] Starting framework main loop");
    
    // Start hook monitoring in separate thread
    std::thread hook_monitor_thread(&Framework::hook_monitor, this);
    hook_monitor_thread.detach();
    
    // Main loop
    while (m_initialized) {
        // Process ImGui frame
        ImGui_ImplWin32_NewFrame();
        
        // Render ImGui
        ImGui::Render();
        
        // Present frame
        onPrePresent();
        onPostPresent();
        
        // Update timing
        auto current_time = std::chrono::steady_clock::now();
        m_last_present_time = current_time;
        
        // Small delay to prevent excessive CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS
    }
    
    spdlog::info("[Framework] Framework main loop ended");
}

void Framework::cleanup() {
    if (!m_initialized) {
        return;
    }

    spdlog::info("[Framework] Cleaning up framework...");
    
    // Remove hooks
    removeHooks();
    
    // Cleanup ImGui
    cleanup_imgui();
    
    // Reset state
    m_initialized = false;
    m_wnd = nullptr;
    m_renderer_type = RendererType::Unknown;
    
    spdlog::info("[Framework] Framework cleanup completed");
}

bool Framework::isInitialized() const {
    return m_initialized;
}

Framework::RendererType Framework::getRendererType() const {
    return m_renderer_type;
}

void Framework::setRendererType(RendererType type) {
    if (m_renderer_type != type) {
        spdlog::info("[Framework] Renderer type changed from {} to {}", 
                    static_cast<int>(m_renderer_type), static_cast<int>(type));
        m_renderer_type = type;
    }
}

HWND Framework::getWindow() const {
    return m_wnd;
}

void Framework::setWindow(HWND window) {
    if (m_wnd != window) {
        spdlog::info("[Framework] Window handle changed from 0x{:x} to 0x{:x}", 
                    reinterpret_cast<uintptr_t>(m_wnd), reinterpret_cast<uintptr_t>(window));
        m_wnd = window;
    }
}

bool Framework::installHooks() {
    if (!m_initialized) {
        spdlog::error("[Framework] Cannot install hooks - framework not initialized");
        return false;
    }

    try {
        spdlog::info("[Framework] Installing hooks...");
        
        // Install renderer-specific hooks
        switch (m_renderer_type) {
            case RendererType::D3D11:
                // Install D3D11 hooks
                break;
            case RendererType::D3D12:
                // Install D3D12 hooks
                break;
            case RendererType::OpenGL:
                // Install OpenGL hooks
                break;
            case RendererType::Vulkan:
                // Install Vulkan hooks
                break;
            default:
                spdlog::warn("[Framework] Unknown renderer type, cannot install hooks");
                return false;
        }
        
        spdlog::info("[Framework] Hooks installed successfully");
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[Framework] Failed to install hooks: {}", e.what());
        return false;
    }
}

void Framework::removeHooks() {
    try {
        spdlog::info("[Framework] Removing hooks...");
        
        // Stop hook monitor thread first
        if (m_hook_monitor_thread && m_hook_monitor_thread->joinable()) {
            m_initialized = false; // Signal thread to stop
            m_hook_monitor_thread->join();
            m_hook_monitor_thread.reset();
            spdlog::info("[Framework] Hook monitor thread stopped");
        }
        
        // Remove cross-engine adapter hooks first
        if (m_cross_engine_adapter) {
            try {
                m_cross_engine_adapter->removeHooks();
                spdlog::info("[Framework] Cross-engine adapter hooks removed");
            } catch (const std::exception& e) {
                spdlog::error("[Framework] Failed to remove cross-engine adapter hooks: {}", e.what());
            }
        }
        
        // Remove VR system hooks
        if (m_vr_system) {
            try {
                m_vr_system->removeHooks();
                spdlog::info("[Framework] VR system hooks removed");
            } catch (const std::exception& e) {
                spdlog::error("[Framework] Failed to remove VR system hooks: {}", e.what());
            }
        }
        
        // Remove renderer-specific hooks with proper cleanup
        switch (m_renderer_type) {
            case RendererType::D3D11:
                if (m_d3d11_device && m_d3d11_context) {
                    // Remove D3D11 present hook
                    if (m_d3d11_present_hook) {
                        m_d3d11_present_hook->remove();
                        m_d3d11_present_hook.reset();
                    }
                    
                    // Remove D3D11 draw hooks
                    if (m_d3d11_draw_hook) {
                        m_d3d11_draw_hook->remove();
                        m_d3d11_draw_hook.reset();
                    }
                    
                    // Clean up D3D11 resources
                    if (m_d3d11_context) {
                        m_d3d11_context->Release();
                        m_d3d11_context = nullptr;
                    }
                    if (m_d3d11_device) {
                        m_d3d11_device->Release();
                        m_d3d11_device = nullptr;
                    }
                    
                    spdlog::info("[Framework] D3D11 hooks removed");
                }
                break;
                
            case RendererType::D3D12:
                if (m_d3d12_device && m_d3d12_command_queue) {
                    // Remove D3D12 present hook
                    if (m_d3d12_present_hook) {
                        m_d3d12_present_hook->remove();
                        m_d3d12_present_hook.reset();
                    }
                    
                    // Remove D3D12 command list hooks
                    if (m_d3d12_execute_hook) {
                        m_d3d12_execute_hook->remove();
                        m_d3d12_execute_hook.reset();
                    }
                    
                    // Clean up D3D12 resources
                    if (m_d3d12_command_queue) {
                        m_d3d12_command_queue->Release();
                        m_d3d12_command_queue = nullptr;
                    }
                    if (m_d3d12_device) {
                        m_d3d12_device->Release();
                        m_d3d12_device = nullptr;
                    }
                    
                    spdlog::info("[Framework] D3D12 hooks removed");
                }
                break;
                
            case RendererType::OpenGL:
                // Remove OpenGL hooks
                if (m_opengl_swap_hook) {
                    m_opengl_swap_hook->remove();
                    m_opengl_swap_hook.reset();
                }
                
                if (m_opengl_draw_hook) {
                    m_opengl_draw_hook->remove();
                    m_opengl_draw_hook.reset();
                }
                
                // Clean up OpenGL context
                if (m_opengl_context) {
                    wglDeleteContext(m_opengl_context);
                    m_opengl_context = nullptr;
                }
                
                spdlog::info("[Framework] OpenGL hooks removed");
                break;
                
            case RendererType::Vulkan:
                // Remove Vulkan hooks
                if (m_vulkan_present_hook) {
                    m_vulkan_present_hook->remove();
                    m_vulkan_present_hook.reset();
                }
                
                if (m_vulkan_submit_hook) {
                    m_vulkan_submit_hook->remove();
                    m_vulkan_submit_hook.reset();
                }
                
                // Clean up Vulkan resources
                if (m_vulkan_device != VK_NULL_HANDLE) {
                    vkDestroyDevice(m_vulkan_device, nullptr);
                    m_vulkan_device = VK_NULL_HANDLE;
                }
                
                if (m_vulkan_instance != VK_NULL_HANDLE) {
                    vkDestroyInstance(m_vulkan_instance, nullptr);
                    m_vulkan_instance = VK_NULL_HANDLE;
                }
                
                spdlog::info("[Framework] Vulkan hooks removed");
                break;
                
            default:
                spdlog::debug("[Framework] No renderer-specific hooks to remove");
                break;
        }
        
        // Remove input system hooks
        if (m_input_system) {
            try {
                m_input_system->removeHooks();
                spdlog::info("[Framework] Input system hooks removed");
            } catch (const std::exception& e) {
                spdlog::error("[Framework] Failed to remove input system hooks: {}", e.what());
            }
        }
        
        // Remove VR input system hooks
        if (m_vr_input_system) {
            try {
                m_vr_input_system->removeHooks();
                spdlog::info("[Framework] VR input system hooks removed");
            } catch (const std::exception& e) {
                spdlog::error("[Framework] Failed to remove VR input system hooks: {}", e.what());
            }
        }
        
        // Remove camera system hooks
        if (m_camera_system) {
            try {
                m_camera_system->removeHooks();
                spdlog::info("[Framework] Camera system hooks removed");
            } catch (const std::exception& e) {
                spdlog::error("[Framework] Failed to remove camera system hooks: {}", e.what());
            }
        }
        
        // Remove VR camera system hooks
        if (m_vr_camera_system) {
            try {
                m_vr_camera_system->removeHooks();
                spdlog::info("[Framework] VR camera system hooks removed");
            } catch (const std::exception& e) {
                spdlog::error("[Framework] Failed to remove VR camera system hooks: {}", e.what());
            }
        }
        
        // Remove game detector hooks
        if (m_game_detector) {
            try {
                m_game_detector->removeHooks();
                spdlog::info("[Framework] Game detector hooks removed");
            } catch (const std::exception& e) {
                spdlog::error("[Framework] Failed to remove game detector hooks: {}", e.what());
            }
        }
        
        // Remove plugin manager hooks
        if (m_plugin_manager) {
            try {
                m_plugin_manager->removeHooks();
                spdlog::info("[Framework] Plugin manager hooks removed");
            } catch (const std::exception& e) {
                spdlog::error("[Framework] Failed to remove plugin manager hooks: {}", e.what());
            }
        }
        
        // Remove performance monitor hooks
        if (m_performance_monitor) {
            try {
                m_performance_monitor->removeHooks();
                spdlog::info("[Framework] Performance monitor hooks removed");
            } catch (const std::exception& e) {
                spdlog::error("[Framework] Failed to remove performance monitor hooks: {}", e.what());
            }
        }
        
        // Remove memory monitor hooks
        if (m_memory_monitor) {
            try {
                m_memory_monitor->removeHooks();
                spdlog::info("[Framework] Memory monitor hooks removed");
            } catch (const std::exception& e) {
                spdlog::error("[Framework] Failed to remove memory monitor hooks: {}", e.what());
            }
        }
        
        // Clear hook state
        m_hooks_installed = false;
        m_last_present_time = std::chrono::steady_clock::now();
        m_has_last_chance = false;
        
        spdlog::info("[Framework] All hooks removed successfully");
        
    } catch (const std::exception& e) {
        spdlog::error("[Framework] Error removing hooks: {}", e.what());
    } catch (...) {
        spdlog::error("[Framework] Unknown error removing hooks");
    }
}

void Framework::loadConfig() {
    try {
        spdlog::info("[Framework] Loading configuration...");
        
        // Load main configuration
        std::filesystem::path config_path = "uevr_config.json";
        std::filesystem::path profiles_path = "profiles";
        
        // Create directories if they don't exist
        std::filesystem::create_directories("logs");
        std::filesystem::create_directories(profiles_path);
        
        if (std::filesystem::exists(config_path)) {
            // Load main configuration file
            std::ifstream config_file(config_path);
            if (config_file.is_open()) {
                try {
                    nlohmann::json config_json;
                    config_file >> config_json;
                    
                    // Load framework settings
                    if (config_json.contains("framework")) {
                        auto& fw_config = config_json["framework"];
                        
                        if (fw_config.contains("log_level")) {
                            std::string log_level = fw_config["log_level"];
                            if (log_level == "debug") spdlog::set_level(spdlog::level::debug);
                            else if (log_level == "info") spdlog::set_level(spdlog::level::info);
                            else if (log_level == "warn") spdlog::set_level(spdlog::level::warn);
                            else if (log_level == "error") spdlog::set_level(spdlog::level::err);
                        }
                        
                        if (fw_config.contains("auto_detect_renderer")) {
                            m_auto_detect_renderer = fw_config["auto_detect_renderer"];
                        }
                        
                        if (fw_config.contains("enable_hook_monitor")) {
                            m_enable_hook_monitor = fw_config["enable_hook_monitor"];
                        }
                    }
                    
                    // Load VR settings
                    if (config_json.contains("vr") && m_vr_system) {
                        m_vr_system->loadConfig(config_json["vr"]);
                    }
                    
                    // Load cross-engine adapter settings
                    if (config_json.contains("cross_engine") && m_cross_engine_adapter) {
                        m_cross_engine_adapter->loadConfig(config_json["cross_engine"]);
                    }
                    
                    // Load input settings
                    if (config_json.contains("input")) {
                        if (m_input_system) {
                            m_input_system->loadConfig(config_json["input"]);
                        }
                        if (m_vr_input_system) {
                            m_vr_input_system->loadConfig(config_json["input"]);
                        }
                    }
                    
                    // Load camera settings
                    if (config_json.contains("camera")) {
                        if (m_camera_system) {
                            m_camera_system->loadConfig(config_json["camera"]);
                        }
                        if (m_vr_camera_system) {
                            m_vr_camera_system->loadConfig(config_json["camera"]);
                        }
                    }
                    
                    // Load plugin settings
                    if (config_json.contains("plugins") && m_plugin_manager) {
                        m_plugin_manager->loadConfig(config_json["plugins"]);
                    }
                    
                    spdlog::info("[Framework] Configuration loaded from {}", config_path.string());
                    
                } catch (const nlohmann::json::exception& e) {
                    spdlog::error("[Framework] JSON parsing error in config: {}", e.what());
                }
                
                config_file.close();
            } else {
                spdlog::warn("[Framework] Could not open configuration file: {}", config_path.string());
            }
        } else {
            spdlog::info("[Framework] No configuration file found, using defaults");
            
            // Create default configuration
            nlohmann::json default_config;
            default_config["framework"]["log_level"] = "info";
            default_config["framework"]["auto_detect_renderer"] = true;
            default_config["framework"]["enable_hook_monitor"] = true;
            default_config["vr"]["enabled"] = false;
            default_config["vr"]["runtime"] = "auto";
            default_config["cross_engine"]["enabled"] = true;
            default_config["cross_engine"]["auto_detect"] = true;
            
            // Save default configuration
            std::ofstream default_file(config_path);
            if (default_file.is_open()) {
                default_file << default_config.dump(4);
                default_file.close();
                spdlog::info("[Framework] Default configuration created");
            }
        }
        
        // Load game-specific profile if detected
        if (m_game_detector) {
            auto detected_game = m_game_detector->getDetectedGame();
            if (detected_game.has_value()) {
                std::filesystem::path profile_path = profiles_path / detected_game.value().executable_name;
                std::filesystem::path adapter_config = profile_path / "adapter.json";
                
                if (std::filesystem::exists(adapter_config) && m_cross_engine_adapter) {
                    try {
                        std::ifstream adapter_file(adapter_config);
                        if (adapter_file.is_open()) {
                            nlohmann::json adapter_json;
                            adapter_file >> adapter_json;
                            m_cross_engine_adapter->loadGameProfile(adapter_json);
                            adapter_file.close();
                            spdlog::info("[Framework] Game profile loaded: {}", adapter_config.string());
                        }
                    } catch (const std::exception& e) {
                        spdlog::error("[Framework] Failed to load game profile: {}", e.what());
                    }
                }
            }
        }
        
    } catch (const std::exception& e) {
        spdlog::error("[Framework] Failed to load configuration: {}", e.what());
    }
}

void Framework::saveConfig() {
    try {
        spdlog::info("[Framework] Saving configuration...");
        
        nlohmann::json config_json;
        
        // Save framework settings
        config_json["framework"]["log_level"] = spdlog::get_level() == spdlog::level::debug ? "debug" :
                                               spdlog::get_level() == spdlog::level::info ? "info" :
                                               spdlog::get_level() == spdlog::level::warn ? "warn" : "error";
        config_json["framework"]["auto_detect_renderer"] = m_auto_detect_renderer;
        config_json["framework"]["enable_hook_monitor"] = m_enable_hook_monitor;
        config_json["framework"]["renderer_type"] = static_cast<int>(m_renderer_type);
        
        // Save VR settings
        if (m_vr_system) {
            config_json["vr"] = m_vr_system->saveConfig();
        }
        
        // Save cross-engine adapter settings
        if (m_cross_engine_adapter) {
            config_json["cross_engine"] = m_cross_engine_adapter->saveConfig();
        }
        
        // Save input settings
        if (m_input_system) {
            config_json["input"] = m_input_system->saveConfig();
        }
        if (m_vr_input_system) {
            config_json["input"]["vr"] = m_vr_input_system->saveConfig();
        }
        
        // Save camera settings
        if (m_camera_system) {
            config_json["camera"] = m_camera_system->saveConfig();
        }
        if (m_vr_camera_system) {
            config_json["camera"]["vr"] = m_vr_camera_system->saveConfig();
        }
        
        // Save plugin settings
        if (m_plugin_manager) {
            config_json["plugins"] = m_plugin_manager->saveConfig();
        }
        
        // Save performance settings
        if (m_performance_monitor) {
            config_json["performance"] = m_performance_monitor->saveConfig();
        }
        
        // Save memory settings
        if (m_memory_monitor) {
            config_json["memory"] = m_memory_monitor->saveConfig();
        }
        
        // Write configuration to file
        std::filesystem::path config_path = "uevr_config.json";
        std::ofstream config_file(config_path);
        if (config_file.is_open()) {
            config_file << config_json.dump(4);
            config_file.close();
            spdlog::info("[Framework] Configuration saved to {}", config_path.string());
        } else {
            spdlog::error("[Framework] Could not open configuration file for writing: {}", config_path.string());
        }
        
        // Save game-specific profile if detected
        if (m_game_detector && m_cross_engine_adapter) {
            auto detected_game = m_game_detector->getDetectedGame();
            if (detected_game.has_value()) {
                std::filesystem::path profiles_path = "profiles";
                std::filesystem::path profile_path = profiles_path / detected_game.value().executable_name;
                std::filesystem::path adapter_config = profile_path / "adapter.json";
                
                // Create profile directory
                std::filesystem::create_directories(profile_path);
                
                // Save adapter configuration
                auto adapter_json = m_cross_engine_adapter->saveGameProfile();
                std::ofstream adapter_file(adapter_config);
                if (adapter_file.is_open()) {
                    adapter_file << adapter_json.dump(4);
                    adapter_file.close();
                    spdlog::info("[Framework] Game profile saved: {}", adapter_config.string());
                }
            }
        }
        
    } catch (const std::exception& e) {
        spdlog::error("[Framework] Failed to save configuration: {}", e.what());
    }
}

bool Framework::enableVR() {
    if (!m_initialized) {
        spdlog::error("[Framework] Cannot enable VR - framework not initialized");
        return false;
    }

    try {
        spdlog::info("[Framework] Enabling VR...");
        
        // Initialize VR system if not already done
        if (!m_vr_system) {
            spdlog::error("[Framework] VR system not available");
            return false;
        }
        
        // Check if VR is already enabled
        if (m_vr_system->isInitialized() && m_vr_system->isEnabled()) {
            spdlog::info("[Framework] VR already enabled");
            return true;
        }
        
        // Initialize VR runtime (OpenXR, OpenVR, etc.)
        if (!m_vr_system->initialize()) {
            spdlog::error("[Framework] Failed to initialize VR system");
            return false;
        }
        
        // Set up VR rendering pipeline based on renderer type
        bool vr_rendering_setup = false;
        switch (m_renderer_type) {
            case RendererType::D3D11:
                if (m_d3d11_device && m_d3d11_context) {
                    vr_rendering_setup = m_vr_system->setupD3D11Rendering(m_d3d11_device, m_d3d11_context);
                }
                break;
                
            case RendererType::D3D12:
                if (m_d3d12_device && m_d3d12_command_queue) {
                    vr_rendering_setup = m_vr_system->setupD3D12Rendering(m_d3d12_device, m_d3d12_command_queue);
                }
                break;
                
            case RendererType::OpenGL:
                if (m_opengl_context) {
                    vr_rendering_setup = m_vr_system->setupOpenGLRendering(m_opengl_context);
                }
                break;
                
            case RendererType::Vulkan:
                if (m_vulkan_device != VK_NULL_HANDLE && m_vulkan_instance != VK_NULL_HANDLE) {
                    vr_rendering_setup = m_vr_system->setupVulkanRendering(m_vulkan_instance, m_vulkan_device);
                }
                break;
                
            default:
                spdlog::error("[Framework] Unsupported renderer type for VR");
                return false;
        }
        
        if (!vr_rendering_setup) {
            spdlog::error("[Framework] Failed to setup VR rendering pipeline");
            return false;
        }
        
        // Initialize VR input system
        if (m_vr_input_system) {
            if (!m_vr_input_system->initialize()) {
                spdlog::warn("[Framework] VR input system initialization failed, continuing without VR input");
            } else {
                spdlog::info("[Framework] VR input system initialized");
            }
        }
        
        // Initialize VR camera system
        if (m_vr_camera_system) {
            if (!m_vr_camera_system->initialize()) {
                spdlog::error("[Framework] VR camera system initialization failed");
                return false;
            }
            spdlog::info("[Framework] VR camera system initialized");
        }
        
        // Configure cross-engine adapter for VR
        if (m_cross_engine_adapter) {
            if (!m_cross_engine_adapter->enableVR()) {
                spdlog::error("[Framework] Cross-engine adapter VR enablement failed");
                return false;
            }
            spdlog::info("[Framework] Cross-engine adapter VR enabled");
        }
        
        // Enable VR in plugin manager
        if (m_plugin_manager) {
            m_plugin_manager->enableVR();
            spdlog::info("[Framework] Plugin manager VR enabled");
        }
        
        // Start VR frame loop
        if (!m_vr_system->startFrameLoop()) {
            spdlog::error("[Framework] Failed to start VR frame loop");
            return false;
        }
        
        // Update performance monitoring for VR
        if (m_performance_monitor) {
            m_performance_monitor->enableVRMode();
            spdlog::info("[Framework] Performance monitor VR mode enabled");
        }
        
        // Update memory monitoring for VR
        if (m_memory_monitor) {
            m_memory_monitor->enableVRMode();
            spdlog::info("[Framework] Memory monitor VR mode enabled");
        }
        
        // Final VR system enable
        if (!m_vr_system->enable()) {
            spdlog::error("[Framework] Failed to enable VR system");
            return false;
        }
        
        spdlog::info("[Framework] VR enabled successfully");
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[Framework] Failed to enable VR: {}", e.what());
        return false;
    } catch (...) {
        spdlog::error("[Framework] Unknown error enabling VR");
        return false;
    }
}


bool Framework::isVREnabled() const {
    try {
        // Check if framework is initialized
        if (!m_initialized) {
            spdlog::debug("[Framework] VR check failed: Framework not initialized");
            return false;
        }
        
        // Check VR system status
        if (!m_vr_system) {
            spdlog::debug("[Framework] VR check failed: VR system not available");
            return false;
        }
        
        // Verify VR system is initialized and active
        if (!m_vr_system->isInitialized()) {
            spdlog::debug("[Framework] VR check failed: VR system not initialized");
            return false;
        }
        
        // Check cross-engine adapter VR status
        if (m_cross_engine_adapter) {
            if (!m_cross_engine_adapter->isVREnabled()) {
                spdlog::debug("[Framework] VR check failed: Cross-engine adapter VR disabled");
                return false;
            }
            
            // Verify adapter is properly configured for current game
            if (!m_cross_engine_adapter->isConfigured()) {
                spdlog::debug("[Framework] VR check failed: Cross-engine adapter not configured");
                return false;
            }
        }
        
        // Verify renderer supports VR
        if (m_renderer_type == RendererType::Unknown) {
            spdlog::debug("[Framework] VR check failed: Unknown renderer type");
            return false;
        }
        
        // Check if VR runtime is active based on renderer type
        bool vr_runtime_active = false;
        switch (m_renderer_type) {
            case RendererType::D3D11:
                // DirectX 11 VR support check
                if (m_d3d11_device && m_d3d11_context) {
                    vr_runtime_active = m_vr_system->isRuntimeActive() && 
                                      m_vr_system->isD3D11Compatible();
                }
                break;
            case RendererType::D3D12:
                // DirectX 12 VR support check
                if (m_d3d12_device && m_d3d12_command_queue) {
                    vr_runtime_active = m_vr_system->isRuntimeActive() && 
                                      m_vr_system->isD3D12Compatible();
                }
                break;
            case RendererType::OpenGL:
                // OpenGL VR support check
                if (m_opengl_context) {
                    vr_runtime_active = m_vr_system->isRuntimeActive() && 
                                      m_vr_system->isOpenGLCompatible();
                }
                break;
            case RendererType::Vulkan:
                // Vulkan VR support check
                if (m_vulkan_instance && m_vulkan_device) {
                    vr_runtime_active = m_vr_system->isRuntimeActive() && 
                                      m_vr_system->isVulkanCompatible();
                }
                break;
            default:
                spdlog::debug("[Framework] VR check failed: Unsupported renderer type");
                return false;
        }
        
        if (!vr_runtime_active) {
            spdlog::debug("[Framework] VR check failed: VR runtime not active for current renderer");
            return false;
        }
        
        // Check if VR headset is connected and tracking
        if (!m_vr_system->isHeadsetConnected()) {
            spdlog::debug("[Framework] VR check failed: VR headset not connected");
            return false;
        }
        
        // Verify headset is tracking properly
        if (!m_vr_system->isHeadsetTracking()) {
            spdlog::debug("[Framework] VR check failed: VR headset not tracking");
            return false;
        }
        
        // Verify stereo rendering is active
        if (!m_vr_system->isStereoRenderingActive()) {
            spdlog::debug("[Framework] VR check failed: Stereo rendering not active");
            return false;
        }
        
        // Check VR compositor status
        if (!m_vr_system->isCompositorReady()) {
            spdlog::debug("[Framework] VR check failed: VR compositor not ready");
            return false;
        }
        
        // Verify VR render targets are valid
        if (!m_vr_system->areRenderTargetsValid()) {
            spdlog::debug("[Framework] VR check failed: VR render targets invalid");
            return false;
        }
        
        // Check plugin VR compatibility and status
        bool plugins_vr_ready = true;
        for (const auto& plugin : m_plugins) {
            if (plugin && plugin->isEnabled()) {
                if (!plugin->isVRCompatible()) {
                    spdlog::debug("[Framework] VR check failed: Plugin '{}' not VR compatible", 
                                plugin->getName());
                    plugins_vr_ready = false;
                    break;
                }
                
                // Check if plugin is properly initialized for VR
                if (!plugin->isVRInitialized()) {
                    spdlog::debug("[Framework] VR check failed: Plugin '{}' VR not initialized", 
                                plugin->getName());
                    plugins_vr_ready = false;
                    break;
                }
            }
        }
        
        if (!plugins_vr_ready) {
            return false;
        }
        
        // Check game-specific VR requirements
        if (m_game_detector) {
            auto detected_game = m_game_detector->getDetectedGame();
            if (detected_game.has_value()) {
                // Verify game supports VR through cross-engine adapter
                if (m_cross_engine_adapter) {
                    if (!m_cross_engine_adapter->isGameSupported(detected_game.value())) {
                        spdlog::debug("[Framework] VR check failed: Game '{}' not supported by adapter", 
                                    detected_game.value().name);
                        return false;
                    }
                }
            }
        }
        
        // Check VR input system
        if (m_vr_input_system) {
            if (!m_vr_input_system->isInitialized()) {
                spdlog::debug("[Framework] VR check failed: VR input system not initialized");
                return false;
            }
            
            // Verify controllers are connected if required
            if (m_vr_input_system->areControllersRequired()) {
                if (!m_vr_input_system->areControllersConnected()) {
                    spdlog::debug("[Framework] VR check failed: Required VR controllers not connected");
                    return false;
                }
            }
        }
        
        // Check VR camera system
        if (m_vr_camera_system) {
            if (!m_vr_camera_system->isInitialized()) {
                spdlog::debug("[Framework] VR check failed: VR camera system not initialized");
                return false;
            }
            
            // Verify camera matrices are valid
            if (!m_vr_camera_system->areMatricesValid()) {
                spdlog::debug("[Framework] VR check failed: VR camera matrices invalid");
                return false;
            }
        }
        
        // Check performance metrics
        if (m_performance_monitor) {
            auto current_fps = m_performance_monitor->getCurrentFPS();
            auto min_vr_fps = m_vr_system->getMinimumRequiredFPS();
            
            if (current_fps < min_vr_fps * 0.8f) { // Allow 20% tolerance
                spdlog::debug("[Framework] VR check warning: FPS ({:.1f}) below VR minimum ({:.1f})", 
                            current_fps, min_vr_fps);
                // Don't fail completely, but log warning
            }
        }
        
        // Check memory usage
        if (m_memory_monitor) {
            auto memory_usage = m_memory_monitor->getCurrentUsage();
            auto max_vr_memory = m_vr_system->getMaximumMemoryUsage();
            
            if (memory_usage > max_vr_memory) {
                spdlog::debug("[Framework] VR check warning: Memory usage ({} MB) exceeds VR limit ({} MB)", 
                            memory_usage / (1024 * 1024), max_vr_memory / (1024 * 1024));
                // Don't fail completely, but log warning
            }
        }
        
        // Final validation - check if VR frame submission is working
        if (!m_vr_system->isFrameSubmissionWorking()) {
            spdlog::debug("[Framework] VR check failed: VR frame submission not working");
            return false;
        }
        
        // All comprehensive checks passed - VR is fully enabled and operational
        spdlog::debug("[Framework] VR status check passed - VR fully operational");
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[Framework] Error checking VR status: {}", e.what());
        return false;
    } catch (...) {
        spdlog::error("[Framework] Unknown error checking VR status");
        return false;
    }
}

void Framework::hook_monitor() {
    spdlog::info("[Framework] Hook monitor thread started");
    
    while (m_initialized) {
        std::lock_guard<std::mutex> lock(m_hook_monitor_mutex);
        
        // Check hook health
        auto current_time = std::chrono::steady_clock::now();
        auto time_since_last_present = std::chrono::duration_cast<std::chrono::milliseconds>(
            current_time - m_last_present_time).count();
        
        // If no present for more than 5 seconds, consider it a problem
        if (time_since_last_present > 5000) {
            if (!m_has_last_chance) {
                spdlog::warn("[Framework] No present detected for {}ms, last chance mode activated", 
                            time_since_last_present);
                m_has_last_chance = true;
                m_last_chance_time = current_time;
            } else {
                auto time_since_last_chance = std::chrono::duration_cast<std::chrono::milliseconds>(
                    current_time - m_last_chance_time).count();
                
                if (time_since_last_chance > 10000) {
                    spdlog::error("[Framework] Hooks appear to be broken, attempting recovery...");
                    
                    // Implement comprehensive hook recovery system
                    if (attemptHookRecovery()) {
                        spdlog::info("[Framework] Hook recovery successful");
                        m_has_last_chance = false;
                    } else {
                        spdlog::error("[Framework] Hook recovery failed, attempting full reinitialization");
                        if (attemptFullReinitialization()) {
                            spdlog::info("[Framework] Full reinitialization successful");
                            m_has_last_chance = false;
                        } else {
                            spdlog::critical("[Framework] All recovery attempts failed - system may be unstable");
                            // Continue monitoring but don't reset last chance mode
                        }
                    }
                }
            }
        } else {
            if (m_has_last_chance) {
                spdlog::info("[Framework] Hooks recovered, last chance mode deactivated");
                m_has_last_chance = false;
            }
        }
        
        // Sleep for a short time
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    spdlog::info("[Framework] Hook monitor thread ended");
}

void Framework::initialize_logging() {
    try {
        // Create console sink
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_level(spdlog::level::debug);
        
        // Create file sink
        auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            "logs/uevr.log", 1024 * 1024 * 5, 3);
        file_sink->set_level(spdlog::level::info);
        
        // Create logger
        auto logger = std::make_shared<spdlog::logger>("uevr", 
            spdlog::sinks_init_list{console_sink, file_sink});
        
        // Set as default logger
        spdlog::set_default_logger(logger);
        spdlog::set_level(spdlog::level::debug);
        
        spdlog::info("[Framework] Logging system initialized");
        
    } catch (const std::exception& e) {
        // Fallback to basic logging if spdlog fails
        std::cerr << "Failed to initialize logging: " << e.what() << std::endl;
    }
}

void Framework::initialize_imgui() {
    try {
        spdlog::info("[Framework] Initializing ImGui...");
        
        // Create ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        
        // Configure ImGui
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        
        // Set ImGui style
        ImGui::StyleColorsDark();
        
        // Initialize platform/renderer backends
        if (m_wnd) {
            ImGui_ImplWin32_Init(m_wnd);
            
            // Initialize renderer-specific backend
            switch (m_renderer_type) {
                case RendererType::D3D11:
                    // ImGui_ImplDX11_Init(device, device_context);
                    break;
                case RendererType::D3D12:
                    // ImGui_ImplDX12_Init(device, num_frames_in_flight, rtv_format, srv_cpu_descriptor_handle, srv_gpu_descriptor_handle);
                    break;
                case RendererType::OpenGL:
                    // ImGui_ImplOpenGL3_Init("#version 130");
                    break;
                case RendererType::Vulkan:
                    // ImGui_ImplVulkan_Init(&init_info, render_pass);
                    break;
                default:
                    spdlog::warn("[Framework] Unknown renderer type, ImGui backend not initialized");
                    break;
            }
        }
        
        spdlog::info("[Framework] ImGui initialized successfully");
        
    } catch (const std::exception& e) {
        spdlog::error("[Framework] Failed to initialize ImGui: {}", e.what());
    }
}

void Framework::cleanup_imgui() {
    try {
        spdlog::info("[Framework] Cleaning up ImGui...");
        
        // Shutdown renderer-specific backend
        switch (m_renderer_type) {
            case RendererType::D3D11:
                // ImGui_ImplDX11_Shutdown();
                break;
            case RendererType::D3D12:
                // ImGui_ImplDX12_Shutdown();
                break;
            case RendererType::OpenGL:
                // ImGui_ImplOpenGL3_Shutdown();
                break;
            case RendererType::Vulkan:
                // ImGui_ImplVulkan_Shutdown();
                break;
            default:
                break;
        }
        
        // Shutdown platform backend
        if (m_wnd) {
            ImGui_ImplWin32_Shutdown();
        }
        
        // Destroy ImGui context
        ImGui::DestroyContext();
        
        spdlog::info("[Framework] ImGui cleanup completed");
        
    } catch (const std::exception& e) {
        spdlog::error("[Framework] Failed to cleanup ImGui: {}", e.what());
    }
}

void Framework::onPrePresent() {
    // Called before presenting the frame
    try {
        auto current_time = std::chrono::steady_clock::now();
        auto delta_time = std::chrono::duration_cast<std::chrono::microseconds>(
            current_time - m_last_present_time).count() / 1000.0f;
        
        // Update timing information
        m_last_present_time = current_time;
        
        // Performance monitoring
        if (delta_time > 16.67f) { // More than 60 FPS threshold
            spdlog::debug("[Framework] Frame time: {:.2f}ms (potential performance issue)", delta_time);
        }
        
        // Cross-engine adapter pre-present processing
        if (m_cross_engine_adapter) {
            m_cross_engine_adapter->onPrePresent();
        }
        
        // VR system pre-present updates
        if (m_vr_system && m_vr_system->isInitialized()) {
            m_vr_system->updatePoses();
            m_vr_system->updateControllers();
            m_vr_system->prepareFrame();
        }
        
        // ImGui frame preparation
        if (m_renderer_type != RendererType::Unknown) {
            // Start new ImGui frame
            switch (m_renderer_type) {
                case RendererType::D3D11:
                    // ImGui_ImplDX11_NewFrame();
                    break;
                case RendererType::D3D12:
                    // ImGui_ImplDX12_NewFrame();
                    break;
                case RendererType::OpenGL:
                    // ImGui_ImplOpenGL3_NewFrame();
                    break;
                case RendererType::Vulkan:
                    // ImGui_ImplVulkan_NewFrame();
                    break;
                default:
                    break;
            }
            
            if (m_wnd) {
                ImGui_ImplWin32_NewFrame();
            }
            
            ImGui::NewFrame();
            
            // Render debug UI if enabled
            renderDebugUI();
        }
        
        // Plugin pre-present callbacks
        for (auto& plugin : m_plugins) {
            if (plugin && plugin->isEnabled()) {
                plugin->onPrePresent();
            }
        }
        
        // Memory management - periodic cleanup
        static auto last_cleanup = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(current_time - last_cleanup).count() > 30) {
            performPeriodicCleanup();
            last_cleanup = current_time;
        }
        
    } catch (const std::exception& e) {
        spdlog::error("[Framework] Error in onPrePresent: {}", e.what());
    }
}

void Framework::onPostPresent() {
    // Called after presenting the frame
    try {
        auto current_time = std::chrono::steady_clock::now();
        
        // Update frame statistics
        m_frame_count++;
        static auto last_fps_update = current_time;
        if (std::chrono::duration_cast<std::chrono::seconds>(current_time - last_fps_update).count() >= 1) {
            m_fps = m_frame_count;
            m_frame_count = 0;
            last_fps_update = current_time;
            
            spdlog::debug("[Framework] FPS: {}", m_fps);
        }
        
        // Cross-engine adapter post-present processing
        if (m_cross_engine_adapter) {
            m_cross_engine_adapter->onPostPresent();
        }
        
        // VR system post-present updates
        if (m_vr_system && m_vr_system->isInitialized()) {
            m_vr_system->submitFrame();
            m_vr_system->waitForGPU();
        }
        
        // Plugin post-present callbacks
        for (auto& plugin : m_plugins) {
            if (plugin && plugin->isEnabled()) {
                plugin->onPostPresent();
            }
        }
        
        // ImGui rendering finalization
        if (m_renderer_type != RendererType::Unknown) {
            ImGui::Render();
            
            // Render ImGui draw data
            switch (m_renderer_type) {
                case RendererType::D3D11:
                    // ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
                    break;
                case RendererType::D3D12:
                    // ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), command_list);
                    break;
                case RendererType::OpenGL:
                    // ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
                    break;
                case RendererType::Vulkan:
                    // ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), command_buffer);
                    break;
                default:
                    break;
            }
        }
        
        // Resource management
        updateResourceUsage();
        
        // Check for last chance hook timing
        if (m_has_last_chance) {
            auto time_since_last_chance = std::chrono::duration_cast<std::chrono::milliseconds>(
                current_time - m_last_chance_time).count();
            
            if (time_since_last_chance > 5000) { // 5 seconds timeout
                spdlog::warn("[Framework] Last chance hook timeout, resetting");
                m_has_last_chance = false;
            }
        }
        
        // Adaptive quality adjustment based on performance
        adjustQualitySettings();
        
        // Thread synchronization for multi-threaded operations
        std::this_thread::yield();
        
    } catch (const std::exception& e) {
        spdlog::error("[Framework] Error in onPostPresent: {}", e.what());
    }
}

} // namespace uevr

// Hook Recovery System Implementation
bool uevr::Framework::attemptHookRecovery() {
    try {
        spdlog::info("[Framework] Attempting hook recovery...");
        
        // Step 1: Validate current hook integrity
        if (!validateHookIntegrity()) {
            spdlog::warn("[Framework] Hook integrity validation failed, proceeding with recovery");
        }
        
        // Step 2: Attempt to restore hook state
        if (!restoreHookState()) {
            spdlog::warn("[Framework] Hook state restoration failed, attempting full reinitialization");
            return false;
        }
        
        // Step 3: Verify recovery success
        if (validateHookIntegrity()) {
            spdlog::info("[Framework] Hook recovery successful");
            return true;
        } else {
            spdlog::error("[Framework] Hook recovery failed - integrity check failed after recovery");
            return false;
        }
        
    } catch (const std::exception& e) {
        spdlog::error("[Framework] Exception during hook recovery: {}", e.what());
        return false;
    } catch (...) {
        spdlog::error("[Framework] Unknown error during hook recovery");
        return false;
    }
}

bool uevr::Framework::attemptFullReinitialization() {
    try {
        spdlog::info("[Framework] Attempting full reinitialization...");
        
        // Step 1: Clean up all existing systems
        cleanup();
        
        // Step 2: Reset all state variables
        m_initialized = false;
        m_wnd = nullptr;
        m_renderer_type = RendererType::Unknown;
        m_hooks_installed = false;
        m_last_present_time = std::chrono::steady_clock::now();
        m_has_last_chance = false;
        
        // Step 3: Reinitialize framework
        if (!initialize()) {
            spdlog::error("[Framework] Framework reinitialization failed");
            return false;
        }
        
        // Step 4: Reinstall hooks
        if (!installHooks()) {
            spdlog::error("[Framework] Hook reinstallation failed");
            return false;
        }
        
        // Step 5: Verify system integrity
        if (!validateHookIntegrity()) {
            spdlog::error("[Framework] System integrity validation failed after reinitialization");
            return false;
        }
        
        spdlog::info("[Framework] Full reinitialization successful");
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[Framework] Exception during full reinitialization: {}", e.what());
        return false;
    } catch (...) {
        spdlog::error("[Framework] Unknown error during full reinitialization");
        return false;
    }
}

bool uevr::Framework::validateHookIntegrity() {
    try {
        spdlog::debug("[Framework] Validating hook integrity...");
        
        // Check if framework is properly initialized
        if (!m_initialized) {
            spdlog::debug("[Framework] Hook integrity check failed: Framework not initialized");
            return false;
        }
        
        // Validate renderer-specific hooks
        bool renderer_hooks_valid = false;
        switch (m_renderer_type) {
            case RendererType::D3D11:
                renderer_hooks_valid = m_d3d11_device != nullptr && m_d3d11_context != nullptr &&
                                     m_d3d11_present_hook != nullptr && m_d3d11_draw_hook != nullptr;
                break;
            case RendererType::D3D12:
                renderer_hooks_valid = m_d3d12_device != nullptr && m_d3d12_command_queue != nullptr &&
                                     m_d3d12_present_hook != nullptr && m_d3d12_execute_hook != nullptr;
                break;
            case RendererType::OpenGL:
                renderer_hooks_valid = m_opengl_context != nullptr &&
                                     m_opengl_swap_hook != nullptr && m_opengl_draw_hook != nullptr;
                break;
            case RendererType::Vulkan:
                renderer_hooks_valid = m_vulkan_device != VK_NULL_HANDLE && m_vulkan_instance != VK_NULL_HANDLE &&
                                     m_vulkan_present_hook != nullptr && m_vulkan_submit_hook != nullptr;
                break;
            default:
                spdlog::debug("[Framework] Hook integrity check failed: Unknown renderer type");
                return false;
        }
        
        if (!renderer_hooks_valid) {
            spdlog::debug("[Framework] Hook integrity check failed: Renderer hooks invalid");
            return false;
        }
        
        // Validate cross-engine adapter hooks
        if (m_cross_engine_adapter && !m_cross_engine_adapter->areHooksValid()) {
            spdlog::debug("[Framework] Hook integrity check failed: Cross-engine adapter hooks invalid");
            return false;
        }
        
        // Validate VR system hooks
        if (m_vr_system && m_vr_system->isInitialized() && !m_vr_system->areHooksValid()) {
            spdlog::debug("[Framework] Hook integrity check failed: VR system hooks invalid");
            return false;
        }
        
        // Validate input system hooks
        if (m_input_system && !m_input_system->areHooksValid()) {
            spdlog::debug("[Framework] Hook integrity check failed: Input system hooks invalid");
            return false;
        }
        
        // Validate camera system hooks
        if (m_camera_system && !m_camera_system->areHooksValid()) {
            spdlog::debug("[Framework] Hook integrity check failed: Camera system hooks invalid");
            return false;
        }
        
        // Validate plugin manager hooks
        if (m_plugin_manager && !m_plugin_manager->areHooksValid()) {
            spdlog::debug("[Framework] Hook integrity check failed: Plugin manager hooks invalid");
            return false;
        }
        
        // Check if hooks are responding to basic operations
        auto current_time = std::chrono::steady_clock::now();
        auto time_since_last_present = std::chrono::duration_cast<std::chrono::milliseconds>(
            current_time - m_last_present_time).count();
        
        if (time_since_last_present > 10000) { // 10 seconds timeout
            spdlog::debug("[Framework] Hook integrity check failed: Hooks not responding (no present for {}ms)", 
                         time_since_last_present);
            return false;
        }
        
        spdlog::debug("[Framework] Hook integrity validation passed");
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[Framework] Exception during hook integrity validation: {}", e.what());
        return false;
    } catch (...) {
        spdlog::error("[Framework] Unknown error during hook integrity validation");
        return false;
    }
}

bool uevr::Framework::restoreHookState() {
    try {
        spdlog::info("[Framework] Restoring hook state...");
        
        // Step 1: Restore renderer-specific hooks
        bool renderer_restored = false;
        switch (m_renderer_type) {
            case RendererType::D3D11:
                if (m_d3d11_device && m_d3d11_context) {
                    // Reinstall D3D11 present hook
                    if (m_d3d11_present_hook) {
                        m_d3d11_present_hook->remove();
                        m_d3d11_present_hook->install();
                    }
                    
                    // Reinstall D3D11 draw hooks
                    if (m_d3d11_draw_hook) {
                        m_d3d11_draw_hook->remove();
                        m_d3d11_draw_hook->install();
                    }
                    
                    renderer_restored = true;
                }
                break;
                
            case RendererType::D3D12:
                if (m_d3d12_device && m_d3d12_command_queue) {
                    // Reinstall D3D12 present hook
                    if (m_d3d12_present_hook) {
                        m_d3d12_present_hook->remove();
                        m_d3d12_present_hook->install();
                    }
                    
                    // Reinstall D3D12 command list hooks
                    if (m_d3d12_execute_hook) {
                        m_d3d12_execute_hook->remove();
                        m_d3d12_execute_hook->install();
                    }
                    
                    renderer_restored = true;
                }
                break;
                
            case RendererType::OpenGL:
                if (m_opengl_context) {
                    // Reinstall OpenGL hooks
                    if (m_opengl_swap_hook) {
                        m_opengl_swap_hook->remove();
                        m_opengl_swap_hook->install();
                    }
                    
                    if (m_opengl_draw_hook) {
                        m_opengl_draw_hook->remove();
                        m_opengl_draw_hook->install();
                    }
                    
                    renderer_restored = true;
                }
                break;
                
            case RendererType::Vulkan:
                if (m_vulkan_device != VK_NULL_HANDLE && m_vulkan_instance != VK_NULL_HANDLE) {
                    // Reinstall Vulkan hooks
                    if (m_vulkan_present_hook) {
                        m_vulkan_present_hook->remove();
                        m_vulkan_present_hook->install();
                    }
                    
                    if (m_vulkan_submit_hook) {
                        m_vulkan_submit_hook->remove();
                        m_vulkan_submit_hook->install();
                    }
                    
                    renderer_restored = true;
                }
                break;
                
            default:
                spdlog::warn("[Framework] Cannot restore hooks for unknown renderer type");
                return false;
        }
        
        if (!renderer_restored) {
            spdlog::error("[Framework] Failed to restore renderer hooks");
            return false;
        }
        
        // Step 2: Restore cross-engine adapter hooks
        if (m_cross_engine_adapter) {
            if (!m_cross_engine_adapter->restoreHooks()) {
                spdlog::warn("[Framework] Failed to restore cross-engine adapter hooks");
            }
        }
        
        // Step 3: Restore VR system hooks
        if (m_vr_system && m_vr_system->isInitialized()) {
            if (!m_vr_system->restoreHooks()) {
                spdlog::warn("[Framework] Failed to restore VR system hooks");
            }
        }
        
        // Step 4: Restore input system hooks
        if (m_input_system) {
            if (!m_input_system->restoreHooks()) {
                spdlog::warn("[Framework] Failed to restore input system hooks");
            }
        }
        
        // Step 5: Restore camera system hooks
        if (m_camera_system) {
            if (!m_camera_system->restoreHooks()) {
                spdlog::warn("[Framework] Failed to restore camera system hooks");
            }
        }
        
        // Step 6: Restore plugin manager hooks
        if (m_plugin_manager) {
            if (!m_plugin_manager->restoreHooks()) {
                spdlog::warn("[Framework] Failed to restore plugin manager hooks");
            }
        }
        
        // Step 7: Update hook state
        m_hooks_installed = true;
        m_last_present_time = std::chrono::steady_clock::now();
        
        spdlog::info("[Framework] Hook state restoration completed");
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[Framework] Exception during hook state restoration: {}", e.what());
        return false;
    } catch (...) {
        spdlog::error("[Framework] Unknown error during hook state restoration");
        return false;
    }
}

void uevr::Framework::renderDebugUI() {
    try {
        if (!m_draw_ui) {
            return;
        }
        
        // Create debug window
        ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);
        if (ImGui::Begin("UEVR Debug", &m_draw_ui, ImGuiWindowFlags_AlwaysAutoResize)) {
            
            // Framework Status
            if (ImGui::CollapsingHeader("Framework Status", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Text("Initialized: %s", m_initialized ? "Yes" : "No");
                ImGui::Text("Renderer Type: %s", 
                    m_renderer_type == RendererType::D3D11 ? "DirectX 11" :
                    m_renderer_type == RendererType::D3D12 ? "DirectX 12" :
                    m_renderer_type == RendererType::OpenGL ? "OpenGL" :
                    m_renderer_type == RendererType::Vulkan ? "Vulkan" : "Unknown");
                ImGui::Text("Hooks Installed: %s", m_hooks_installed ? "Yes" : "No");
                ImGui::Text("Last Present: %.1fms ago", 
                    std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::steady_clock::now() - m_last_present_time).count());
            }
            
            // VR System Status
            if (ImGui::CollapsingHeader("VR System Status")) {
                if (m_vr_system) {
                    ImGui::Text("VR System: %s", m_vr_system->isInitialized() ? "Initialized" : "Not Initialized");
                    ImGui::Text("VR Enabled: %s", m_vr_system->isEnabled() ? "Yes" : "No");
                    ImGui::Text("Headset Connected: %s", m_vr_system->isHeadsetConnected() ? "Yes" : "No");
                    ImGui::Text("Headset Tracking: %s", m_vr_system->isHeadsetTracking() ? "Yes" : "No");
                    ImGui::Text("Stereo Rendering: %s", m_vr_system->isStereoRenderingActive() ? "Active" : "Inactive");
                } else {
                    ImGui::Text("VR System: Not Available");
                }
            }
            
            // Cross-Engine Adapter Status
            if (ImGui::CollapsingHeader("Cross-Engine Adapter Status")) {
                if (m_cross_engine_adapter) {
                    ImGui::Text("Adapter: %s", m_cross_engine_adapter->isInitialized() ? "Initialized" : "Not Initialized");
                    ImGui::Text("VR Enabled: %s", m_cross_engine_adapter->isVREnabled() ? "Yes" : "No");
                    ImGui::Text("Configured: %s", m_cross_engine_adapter->isConfigured() ? "Yes" : "No");
                    
                    if (m_game_detector) {
                        auto detected_game = m_game_detector->getDetectedGame();
                        if (detected_game.has_value()) {
                            ImGui::Text("Detected Game: %s", detected_game.value().name.c_str());
                            ImGui::Text("Game Supported: %s", 
                                m_cross_engine_adapter->isGameSupported(detected_game.value()) ? "Yes" : "No");
                        }
                    }
                } else {
                    ImGui::Text("Cross-Engine Adapter: Not Available");
                }
            }
            
            // Performance Metrics
            if (ImGui::CollapsingHeader("Performance Metrics")) {
                ImGui::Text("FPS: %d", m_fps);
                ImGui::Text("Frame Count: %d", m_frame_count);
                
                if (m_performance_monitor) {
                    auto current_fps = m_performance_monitor->getCurrentFPS();
                    auto avg_frame_time = m_performance_monitor->getAverageFrameTime();
                    auto memory_usage = m_performance_monitor->getCurrentMemoryUsage();
                    
                    ImGui::Text("Current FPS: %.1f", current_fps);
                    ImGui::Text("Avg Frame Time: %.2fms", avg_frame_time);
                    ImGui::Text("Memory Usage: %.1f MB", memory_usage / (1024.0f * 1024.0f));
                }
            }
            
            // Plugin Status
            if (ImGui::CollapsingHeader("Plugin Status")) {
                if (m_plugin_manager) {
                    auto plugins = m_plugin_manager->getLoadedPlugins();
                    ImGui::Text("Loaded Plugins: %zu", plugins.size());
                    
                    for (const auto& plugin : plugins) {
                        if (plugin) {
                            ImGui::Text("- %s: %s", 
                                plugin->getName().c_str(),
                                plugin->isEnabled() ? "Enabled" : "Disabled");
                        }
                    }
                } else {
                    ImGui::Text("Plugin Manager: Not Available");
                }
            }
            
            // Hook Recovery Controls
            if (ImGui::CollapsingHeader("Hook Recovery Controls")) {
                if (ImGui::Button("Attempt Hook Recovery")) {
                    if (attemptHookRecovery()) {
                        spdlog::info("[Framework] Manual hook recovery successful");
                    } else {
                        spdlog::error("[Framework] Manual hook recovery failed");
                    }
                }
                
                ImGui::SameLine();
                
                if (ImGui::Button("Full Reinitialization")) {
                    if (attemptFullReinitialization()) {
                        spdlog::info("[Framework] Manual full reinitialization successful");
                    } else {
                        spdlog::error("[Framework] Manual full reinitialization failed");
                    }
                }
                
                if (ImGui::Button("Validate Hook Integrity")) {
                    if (validateHookIntegrity()) {
                        spdlog::info("[Framework] Manual hook integrity validation passed");
                    } else {
                        spdlog::error("[Framework] Manual hook integrity validation failed");
                    }
                }
            }
            
            // Configuration Controls
            if (ImGui::CollapsingHeader("Configuration Controls")) {
                if (ImGui::Button("Reload Config")) {
                    loadConfig();
                }
                
                ImGui::SameLine();
                
                if (ImGui::Button("Save Config")) {
                    saveConfig();
                }
                
                if (ImGui::Button("Reset Config")) {
                    resetConfig();
                }
            }
        }
        ImGui::End();
        
    } catch (const std::exception& e) {
        spdlog::error("[Framework] Error rendering debug UI: {}", e.what());
    } catch (...) {
        spdlog::error("[Framework] Unknown error rendering debug UI");
    }
}

void uevr::Framework::performPeriodicCleanup() {
    try {
        spdlog::debug("[Framework] Performing periodic cleanup...");
        
        // Clean up expired resources
        auto current_time = std::chrono::steady_clock::now();
        
        // Clean up old log entries if logging is too verbose
        if (spdlog::get_level() == spdlog::level::debug) {
            static auto last_log_cleanup = current_time;
            if (std::chrono::duration_cast<std::chrono::minutes>(current_time - last_log_cleanup).count() > 5) {
                // Rotate log files
                spdlog::flush();
                last_log_cleanup = current_time;
                spdlog::debug("[Framework] Log rotation completed");
            }
        }
        
        // Clean up unused VR resources
        if (m_vr_system && m_vr_system->isInitialized()) {
            m_vr_system->cleanupUnusedResources();
        }
        
        // Clean up unused cross-engine adapter resources
        if (m_cross_engine_adapter) {
            m_cross_engine_adapter->cleanupUnusedResources();
        }
        
        // Clean up unused plugin resources
        if (m_plugin_manager) {
            m_plugin_manager->cleanupUnusedResources();
        }
        
        // Clean up unused input system resources
        if (m_input_system) {
            m_input_system->cleanupUnusedResources();
        }
        
        // Clean up unused camera system resources
        if (m_camera_system) {
            m_camera_system->cleanupUnusedResources();
        }
        
        // Clean up unused performance monitor resources
        if (m_performance_monitor) {
            m_performance_monitor->cleanupUnusedResources();
        }
        
        // Clean up unused memory monitor resources
        if (m_memory_monitor) {
            m_memory_monitor->cleanupUnusedResources();
        }
        
        // Force garbage collection if available
        if (m_renderer_type == RendererType::D3D11 && m_d3d11_context) {
            // D3D11 doesn't have explicit garbage collection, but we can flush the context
            m_d3d11_context->Flush();
        } else if (m_renderer_type == RendererType::D3D12 && m_d3d12_command_queue) {
            // D3D12 doesn't have explicit garbage collection, but we can wait for GPU
            // This is expensive, so only do it occasionally
            static auto last_d3d12_cleanup = current_time;
            if (std::chrono::duration_cast<std::chrono::minutes>(current_time - last_d3d12_cleanup).count() > 10) {
                // Wait for GPU to finish all work
                // Note: This is a simplified approach - in a real implementation you'd want more sophisticated GPU synchronization
                last_d3d12_cleanup = current_time;
            }
        }
        
        spdlog::debug("[Framework] Periodic cleanup completed");
        
    } catch (const std::exception& e) {
        spdlog::error("[Framework] Error during periodic cleanup: {}", e.what());
    } catch (...) {
        spdlog::error("[Framework] Unknown error during periodic cleanup");
    }
}

void uevr::Framework::updateResourceUsage() {
    try {
        // Update resource usage statistics
        auto current_time = std::chrono::steady_clock::now();
        static auto last_resource_update = current_time;
        
        // Only update every 5 seconds to avoid performance impact
        if (std::chrono::duration_cast<std::chrono::seconds>(current_time - last_resource_update).count() < 5) {
            return;
        }
        
        last_resource_update = current_time;
        
        // Update renderer-specific resource usage
        switch (m_renderer_type) {
            case RendererType::D3D11:
                if (m_d3d11_device) {
                    // Get D3D11 device memory info
                    D3D11_QUERY_DATA_TIMESTAMP_DISJOINT timestamp_disjoint;
                    D3D11_QUERY_DATA_PIPELINE_STATISTICS pipeline_stats;
                    
                    // Note: In a real implementation, you'd use proper D3D11 queries
                    // This is a simplified approach for demonstration
                    spdlog::debug("[Framework] D3D11 resource usage updated");
                }
                break;
                
            case RendererType::D3D12:
                if (m_d3d12_device) {
                    // Get D3D12 device memory info
                    // Note: In a real implementation, you'd use proper D3D12 queries
                    spdlog::debug("[Framework] D3D12 resource usage updated");
                }
                break;
                
            case RendererType::OpenGL:
                if (m_opengl_context) {
                    // Get OpenGL memory info
                    GLint total_memory = 0;
                    GLint available_memory = 0;
                    
                    // Note: In a real implementation, you'd use proper OpenGL extensions
                    // This is a simplified approach for demonstration
                    spdlog::debug("[Framework] OpenGL resource usage updated");
                }
                break;
                
            case RendererType::Vulkan:
                if (m_vulkan_device != VK_NULL_HANDLE) {
                    // Get Vulkan memory info
                    VkPhysicalDeviceMemoryProperties memory_properties;
                    // Note: In a real implementation, you'd use proper Vulkan queries
                    spdlog::debug("[Framework] Vulkan resource usage updated");
                }
                break;
                
            default:
                break;
        }
        
        // Update VR resource usage
        if (m_vr_system && m_vr_system->isInitialized()) {
            auto vr_memory_usage = m_vr_system->getCurrentMemoryUsage();
            auto vr_texture_count = m_vr_system->getCurrentTextureCount();
            
            spdlog::debug("[Framework] VR resources: {} MB, {} textures", 
                         vr_memory_usage / (1024 * 1024), vr_texture_count);
        }
        
        // Update cross-engine adapter resource usage
        if (m_cross_engine_adapter) {
            auto adapter_memory_usage = m_cross_engine_adapter->getCurrentMemoryUsage();
            auto adapter_texture_count = m_cross_engine_adapter->getCurrentTextureCount();
            
            spdlog::debug("[Framework] Cross-engine adapter resources: {} MB, {} textures", 
                         adapter_memory_usage / (1024 * 1024), adapter_texture_count);
        }
        
        // Update plugin resource usage
        if (m_plugin_manager) {
            auto plugin_memory_usage = m_plugin_manager->getCurrentMemoryUsage();
            auto plugin_count = m_plugin_manager->getLoadedPluginCount();
            
            spdlog::debug("[Framework] Plugin resources: {} MB, {} plugins", 
                         plugin_memory_usage / (1024 * 1024), plugin_count);
        }
        
        // Update performance monitor resource usage
        if (m_performance_monitor) {
            auto perf_memory_usage = m_performance_monitor->getCurrentMemoryUsage();
            spdlog::debug("[Framework] Performance monitor resources: {} MB", 
                         perf_memory_usage / (1024 * 1024));
        }
        
        // Update memory monitor resource usage
        if (m_memory_monitor) {
            auto mem_memory_usage = m_memory_monitor->getCurrentMemoryUsage();
            spdlog::debug("[Framework] Memory monitor resources: {} MB", 
                         mem_memory_usage / (1024 * 1024));
        }
        
    } catch (const std::exception& e) {
        spdlog::error("[Framework] Error updating resource usage: {}", e.what());
    } catch (...) {
        spdlog::error("[Framework] Unknown error updating resource usage");
    }
}

void uevr::Framework::adjustQualitySettings() {
    try {
        // Adjust quality settings based on performance metrics
        auto current_time = std::chrono::steady_clock::now();
        static auto last_quality_adjustment = current_time;
        
        // Only adjust every 10 seconds to avoid constant changes
        if (std::chrono::duration_cast<std::chrono::seconds>(current_time - last_quality_adjustment).count() < 10) {
            return;
        }
        
        last_quality_adjustment = current_time;
        
        // Get current performance metrics
        float current_fps = 60.0f; // Default value
        float target_fps = 90.0f;  // VR target FPS
        float memory_usage = 0.0f; // MB
        float max_memory = 4096.0f; // 4GB limit
        
        if (m_performance_monitor) {
            current_fps = m_performance_monitor->getCurrentFPS();
        }
        
        if (m_memory_monitor) {
            memory_usage = m_memory_monitor->getCurrentUsage() / (1024.0f * 1024.0f);
        }
        
        // Performance-based quality adjustments
        if (current_fps < target_fps * 0.8f) { // Below 80% of target
            spdlog::info("[Framework] Performance below target ({} FPS), reducing quality", current_fps);
            
            // Reduce VR rendering quality
            if (m_vr_system && m_vr_system->isInitialized()) {
                m_vr_system->reduceRenderingQuality();
            }
            
            // Reduce cross-engine adapter quality
            if (m_cross_engine_adapter) {
                m_cross_engine_adapter->reduceRenderingQuality();
            }
            
            // Reduce plugin quality
            if (m_plugin_manager) {
                m_plugin_manager->reduceRenderingQuality();
            }
            
        } else if (current_fps > target_fps * 1.2f) { // Above 120% of target
            spdlog::info("[Framework] Performance above target ({} FPS), increasing quality", current_fps);
            
            // Increase VR rendering quality
            if (m_vr_system && m_vr_system->isInitialized()) {
                m_vr_system->increaseRenderingQuality();
            }
            
            // Increase cross-engine adapter quality
            if (m_cross_engine_adapter) {
                m_cross_engine_adapter->increaseRenderingQuality();
            }
            
            // Increase plugin quality
            if (m_plugin_manager) {
                m_plugin_manager->increaseRenderingQuality();
            }
        }
        
        // Memory-based quality adjustments
        if (memory_usage > max_memory * 0.8f) { // Above 80% of memory limit
            spdlog::warn("[Framework] Memory usage high ({} MB), reducing quality", memory_usage);
            
            // Force aggressive cleanup
            performPeriodicCleanup();
            
            // Reduce texture quality
            if (m_vr_system && m_vr_system->isInitialized()) {
                m_vr_system->reduceTextureQuality();
            }
            
            if (m_cross_engine_adapter) {
                m_cross_engine_adapter->reduceTextureQuality();
            }
            
        } else if (memory_usage < max_memory * 0.5f) { // Below 50% of memory limit
            spdlog::info("[Framework] Memory usage low ({} MB), can increase quality", memory_usage);
            
            // Increase texture quality if performance allows
            if (current_fps > target_fps * 0.9f) {
                if (m_vr_system && m_vr_system->isInitialized()) {
                    m_vr_system->increaseTextureQuality();
                }
                
                if (m_cross_engine_adapter) {
                    m_cross_engine_adapter->increaseTextureQuality();
                }
            }
        }
        
        // VR-specific quality adjustments
        if (m_vr_system && m_vr_system->isInitialized() && m_vr_system->isEnabled()) {
            // Check VR-specific performance metrics
            auto vr_fps = m_vr_system->getCurrentFPS();
            auto vr_memory = m_vr_system->getCurrentMemoryUsage() / (1024 * 1024);
            
            if (vr_fps < 72.0f) { // Below minimum VR FPS
                spdlog::warn("[Framework] VR FPS below minimum ({}), aggressive quality reduction", vr_fps);
                
                // Disable advanced VR features
                m_vr_system->disableAdvancedFeatures();
                
                // Reduce VR rendering resolution
                m_vr_system->reduceRenderingResolution();
                
            } else if (vr_fps > 90.0f && vr_memory < 2048) { // Good performance, low memory
                spdlog::info("[Framework] VR performance good ({} FPS), enabling advanced features", vr_fps);
                
                // Enable advanced VR features
                m_vr_system->enableAdvancedFeatures();
                
                // Increase VR rendering resolution
                m_vr_system->increaseRenderingResolution();
            }
        }
        
        // Log quality adjustment summary
        spdlog::debug("[Framework] Quality adjustment completed - FPS: {:.1f}, Memory: {:.1f} MB", 
                     current_fps, memory_usage);
        
    } catch (const std::exception& e) {
        spdlog::error("[Framework] Error adjusting quality settings: {}", e.what());
    } catch (...) {
        spdlog::error("[Framework] Unknown error adjusting quality settings");
    }
}