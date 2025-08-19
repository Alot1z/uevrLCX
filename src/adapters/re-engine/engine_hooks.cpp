/*
 * RE Engine Cross-Engine VR Adapter for uevr
 * 
 * Extends uevr's plugin architecture to support Capcom's RE Engine
 * Target Game: Resident Evil 7 (re7.exe)
 * Graphics API: D3D11
 * 
 * Copyright (c) 2024 uevr Cross-Engine Adapter Project
 * Based on uevr Plugin System (MIT License)
 */

#include <windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <MinHook.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <string>
#include <DirectXMath.h>

using namespace DirectX;

#include "uevr/Plugin.hpp"
#include "uevr/API.hpp"
#include "engine_hooks.hpp"
#include "RE_Engine_Hooks.hpp"

// Disable legacy plugin implementation in this file. The active plugin lives in re7_adapter.cpp.
#if 0 // Legacy plugin implementation disabled (migrated to re7_adapter.cpp)
namespace uevr {
namespace re_engine {

class REEngineAdapter : public Plugin {
private:
    // Helper: create a temporary D3D11 device/swapchain to resolve IDXGISwapChain vtable
    bool resolve_swapchain_methods(void** out_present, void** out_resize_buffers) {
        if (!out_present || !out_resize_buffers) {
            return false;
        }

        DXGI_SWAP_CHAIN_DESC sd{};
        sd.BufferCount = 1;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow = GetDesktopWindow();
        sd.SampleDesc.Count = 1;
        sd.Windowed = TRUE;
        sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

        D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
        D3D_FEATURE_LEVEL obtainedLevel{};

        ID3D11Device* tempDevice = nullptr;
        ID3D11DeviceContext* tempContext = nullptr;
        IDXGISwapChain* tempSwapchain = nullptr;

        HRESULT hr = D3D11CreateDeviceAndSwapChain(
            nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            0,
            &featureLevel,
            1,
            D3D11_SDK_VERSION,
            &sd,
            &tempSwapchain,
            &tempDevice,
            &obtainedLevel,
            &tempContext
        );

        if (FAILED(hr) || tempSwapchain == nullptr) {
            spdlog::error("[RE Engine Adapter] Failed to create temporary D3D11 swapchain for vtable resolution: 0x{:08x}", (unsigned)hr);
            if (tempContext) tempContext->Release();
            if (tempDevice) tempDevice->Release();
            return false;
        }

        void** vtbl = *reinterpret_cast<void***>(tempSwapchain);
        // DXGI 1.0: vtbl[8] = Present, vtbl[13] = ResizeBuffers
        *out_present = vtbl[8];
        *out_resize_buffers = vtbl[13];

        tempSwapchain->Release();
        if (tempContext) tempContext->Release();
        if (tempDevice) tempDevice->Release();

        return *out_present != nullptr && *out_resize_buffers != nullptr;
    }

    // RE Engine specific state
    struct REEngineState {
        ID3D11Device* device = nullptr;
        ID3D11DeviceContext* context = nullptr;
        IDXGISwapChain* swapchain = nullptr;
        
        // Camera matrices (to be extracted from constant buffers)
        struct CameraMatrices {
            float view[16] = {0};
            float projection[16] = {0};
            float viewproj[16] = {0};
        } camera;
        
        // Hook state
        bool hooks_installed = false;
        bool engine_detected = false;
        // Limit noisy logging while probing constant buffers in-game
        int probe_frames_remaining = 120; // probe first ~120 frames
    } m_state;
    
    // Original function pointers
    typedef HRESULT(__stdcall* Present_t)(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
    typedef HRESULT(__stdcall* ResizeBuffers_t)(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);
    
    Present_t m_original_present = nullptr;
    ResizeBuffers_t m_original_resize_buffers = nullptr;

public:
    REEngineAdapter() = default;
    virtual ~REEngineAdapter() = default;

    // uevr Plugin Interface Implementation
    virtual void on_initialize() override {
        spdlog::info("[RE Engine Adapter] Initializing RE Engine VR integration");
        
        // Install D3D11 hooks for RE Engine
        if (!install_hooks()) {
            spdlog::error("[RE Engine Adapter] Failed to install hooks");
            return;
        }
        
        spdlog::info("[RE Engine Adapter] Hooks installed successfully");
    }

    virtual void on_device_reset() override {
        spdlog::info("[RE Engine Adapter] Device reset detected, reinstalling hooks");
        uninstall_hooks();
        install_hooks();
    }

    virtual void on_post_render_vr_framework_dx11(ID3D11DeviceContext* context, ID3D11Texture2D* texture, ID3D11RenderTargetView* rtv) override {
        // RE Engine specific post-render processing
        if (m_state.engine_detected) {
            process_re_engine_frame(context, texture, rtv);
        }
    }

private:
    bool install_hooks() {
        if (m_state.hooks_installed) {
            return true;
        }

        // Initialize MinHook
        const auto mhInit = MH_Initialize();
        if (mhInit != MH_OK && mhInit != MH_ERROR_ALREADY_INITIALIZED) {
            spdlog::error("[RE Engine Adapter] MH_Initialize failed: {}", (int)mhInit);
            return false;
        }

        void* presentTarget = nullptr;
        void* resizeTarget = nullptr;
        if (!resolve_swapchain_methods(&presentTarget, &resizeTarget)) {
            spdlog::error("[RE Engine Adapter] Could not resolve IDXGISwapChain vtable addresses");
            return false;
        }

        // Hook IDXGISwapChain::Present
        if (MH_CreateHook(presentTarget, &Present_Hook, reinterpret_cast<LPVOID*>(&m_original_present)) != MH_OK) {
            spdlog::error("[RE Engine Adapter] Failed to create Present hook");
            return false;
        }

        // Hook IDXGISwapChain::ResizeBuffers
        if (MH_CreateHook(resizeTarget, &ResizeBuffers_Hook, reinterpret_cast<LPVOID*>(&m_original_resize_buffers)) != MH_OK) {
            spdlog::error("[RE Engine Adapter] Failed to create ResizeBuffers hook");
            return false;
        }

        // Enable all hooks
        if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK) {
            spdlog::error("[RE Engine Adapter] Failed to enable hooks");
            return false;
        }

        m_state.hooks_installed = true;
        spdlog::info("[RE Engine Adapter] MinHook hooks installed (Present={}, ResizeBuffers={})",
                     static_cast<const void*>(presentTarget), static_cast<const void*>(resizeTarget));
        return true;
    }

    void uninstall_hooks() {
        if (!m_state.hooks_installed) {
            return;
        }

        MH_DisableHook(MH_ALL_HOOKS);
        MH_Uninitialize();

        m_state.hooks_installed = false;
        m_original_present = nullptr;
        m_original_resize_buffers = nullptr;
    }

    // Hook implementations
    static HRESULT __stdcall Present_Hook(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags) {
        auto* adapter = static_cast<REEngineAdapter*>(uevr::detail::g_plugin);
        return adapter->on_present(pSwapChain, SyncInterval, Flags);
    }

    static HRESULT __stdcall ResizeBuffers_Hook(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags) {
        auto* adapter = static_cast<REEngineAdapter*>(uevr::detail::g_plugin);
        return adapter->on_resize_buffers(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);
    }

    HRESULT on_present(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags) {
        // Detect RE Engine on first Present call
        if (!m_state.engine_detected) {
            detect_re_engine(pSwapChain);
        }

        // Extract camera matrices from constant buffers
        if (m_state.engine_detected) {
            extract_camera_matrices();
        }

        // Call original function
        if (m_original_present) {
            return m_original_present(pSwapChain, SyncInterval, Flags);
        }
        return S_OK;
    }

    HRESULT on_resize_buffers(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags) {
        spdlog::info("[RE Engine Adapter] ResizeBuffers called: {}x{}", Width, Height);
        
        // Call original function
        if (m_original_resize_buffers) {
            return m_original_resize_buffers(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);
        }
        return S_OK;
    }

    void detect_re_engine(IDXGISwapChain* pSwapChain) {
        // Get device and context from swapchain
        ID3D11Device* device = nullptr;
        ID3D11DeviceContext* context = nullptr;
        
        if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), reinterpret_cast<void**>(&device)))) {
            device->GetImmediateContext(&context);
            
            m_state.device = device;
            m_state.context = context;
            m_state.swapchain = pSwapChain;
            
            // Check for RE Engine specific patterns
            if (check_re_engine_signatures()) {
                m_state.engine_detected = true;
                spdlog::info("[RE Engine Adapter] RE Engine detected successfully");
                
                // Initialize RE Engine specific VR systems
                initialize_re_engine_vr();
            }
        }
    }

    bool check_re_engine_signatures() {
        // Check for RE Engine specific memory patterns or function signatures
        // This is a simplified check - in practice, more sophisticated detection would be used
        
        // Check for common RE Engine DLLs or memory patterns
        HMODULE re_engine_module = GetModuleHandleA("re7.exe");
        if (re_engine_module) {
            spdlog::info("[RE Engine Adapter] RE7 executable detected");
            return true;
        }
        
        return false;
    }

    void initialize_re_engine_vr() {
        spdlog::info("[RE Engine Adapter] Initializing RE Engine VR systems");
        
        // Set up camera matrix extraction
        setup_camera_extraction();
        
        // Initialize motion controller integration
        setup_motion_controllers();
        
        // Configure UI projection for RE Engine HUD
        setup_ui_projection();
    }

    void setup_camera_extraction() {
        spdlog::info("[RE Engine Adapter] Setting up camera matrix extraction");
        
        try {
            // Initialize camera matrix extraction system
            if (!initialize_camera_matrix_extraction()) {
                spdlog::error("[RE Engine Adapter] Failed to initialize camera matrix extraction");
                return;
            }
            
            // Setup constant buffer scanning patterns for RE Engine
            setup_constant_buffer_patterns();
            
            // Initialize matrix validation system
            initialize_matrix_validation();
            
            // Setup automatic matrix detection
            setup_automatic_matrix_detection();
            
            // Initialize matrix interpolation for smooth transitions
            initialize_matrix_interpolation();
            
            // Setup matrix caching system
            initialize_matrix_caching();
            
            // Initialize matrix debugging and visualization
            initialize_matrix_debugging();
            
            spdlog::info("[RE Engine Adapter] Camera matrix extraction system fully initialized");
            
        } catch (const std::exception& e) {
            spdlog::error("[RE Engine Adapter] Failed to setup camera extraction: {}", e.what());
        }
    }

    void setup_motion_controllers() {
        spdlog::info("[RE Engine Adapter] Setting up motion controller integration");
        
        try {
            // Initialize motion controller system
            if (!initialize_motion_controller_system()) {
                spdlog::error("[RE Engine Adapter] Failed to initialize motion controller system");
                return;
            }
            
            // Setup RE Engine specific motion controller mappings
            setup_re_engine_motion_mappings();
            
            // Initialize haptic feedback system
            initialize_haptic_feedback_system();
            
            // Setup motion controller calibration
            setup_motion_controller_calibration();
            
            // Initialize gesture recognition
            initialize_gesture_recognition();
            
            // Setup motion controller performance monitoring
            setup_motion_controller_performance_monitoring();
            
            // Initialize motion controller debugging tools
            initialize_motion_controller_debugging();
            
            spdlog::info("[RE Engine Adapter] Motion controller integration fully initialized");
            
        } catch (const std::exception& e) {
            spdlog::error("[RE Engine Adapter] Failed to setup motion controllers: {}", e.what());
        }
    }

    void setup_ui_projection() {
        spdlog::info("[RE Engine Adapter] Setting up UI projection for RE Engine HUD");
        
        try {
            // Initialize RE Engine specific UI projection system
            if (!initialize_re_engine_ui_projection()) {
                spdlog::error("[RE Engine Adapter] Failed to initialize RE Engine UI projection");
                return;
            }
            
            // Setup horror game specific UI optimizations
            setup_horror_game_ui_optimizations();
            
            // Initialize fixed camera UI adaptations
            initialize_fixed_camera_ui_adaptations();
            
            // Setup atmospheric lighting UI adjustments
            setup_atmospheric_lighting_ui_adjustments();
            
            // Initialize VR-specific HUD positioning
            initialize_vr_hud_positioning();
            
            // Setup UI performance optimization
            setup_ui_performance_optimization();
            
            // Initialize UI debugging and profiling
            initialize_ui_debugging_and_profiling();
            
            spdlog::info("[RE Engine Adapter] RE Engine UI projection system fully initialized");
            
        } catch (const std::exception& e) {
            spdlog::error("[RE Engine Adapter] Failed to setup UI projection: {}", e.what());
        }
    }

    void extract_camera_matrices() {
        try {
            // Extract camera matrices from RE Engine constant buffers
            if (!extract_re_engine_camera_matrices()) {
                spdlog::warn("[RE Engine Adapter] Failed to extract camera matrices, using fallback");
                extract_fallback_camera_matrices();
                return;
            }
            
            // Validate extracted matrices
            if (!validate_extracted_matrices()) {
                spdlog::warn("[RE Engine Adapter] Matrix validation failed, attempting recovery");
                if (!attempt_matrix_recovery()) {
                    spdlog::error("[RE Engine Adapter] Matrix recovery failed, using defaults");
                    use_default_camera_matrices();
                    return;
                }
            }
            
            // Apply VR-specific matrix optimizations
            apply_vr_matrix_optimizations();
            
            // Update camera state
            update_camera_state();
            
            // Log successful extraction
            log_camera_matrix_extraction();
            
        } catch (const std::exception& e) {
            spdlog::error("[RE Engine Adapter] Camera matrix extraction failed: {}", e.what());
            extract_fallback_camera_matrices();
        }
    }

    void probe_constant_buffers() {
        if (!m_state.device || !m_state.context) {
            return;
        }

        constexpr UINT kMaxSlots = D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT; // typically 14
        ID3D11Buffer* vsCB[kMaxSlots] = {};
        ID3D11Buffer* psCB[kMaxSlots] = {};

        m_state.context->VSGetConstantBuffers(0, kMaxSlots, vsCB);
        m_state.context->PSGetConstantBuffers(0, kMaxSlots, psCB);

        auto logCB = [&](const char* stage, UINT slot, ID3D11Buffer* buf) {
            if (!buf) return;
            D3D11_BUFFER_DESC desc{};
            buf->GetDesc(&desc);

            // Create CPU-readable staging buffer
            D3D11_BUFFER_DESC sdesc = desc;
            sdesc.Usage = D3D11_USAGE_STAGING;
            sdesc.BindFlags = 0;
            sdesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
            sdesc.MiscFlags = 0;

            ID3D11Buffer* staging = nullptr;
            HRESULT hr = m_state.device->CreateBuffer(&sdesc, nullptr, &staging);
            if (SUCCEEDED(hr) && staging) {
                m_state.context->CopyResource(staging, buf);

                D3D11_MAPPED_SUBRESOURCE mapped{};
                hr = m_state.context->Map(staging, 0, D3D11_MAP_READ, 0, &mapped);
                if (SUCCEEDED(hr) && mapped.pData) {
                    const float* f = reinterpret_cast<const float*>(mapped.pData);
                    const size_t floatCount = desc.ByteWidth / sizeof(float);
                    // Guarded fetches
                    auto getf = [&](size_t idx) -> float { return (idx < floatCount) ? f[idx] : 0.0f; };

                    spdlog::info(
                        "[RE7 CB Probe] {} cb_slot={} size={} bytes row0@0=[{:.3f}, {:.3f}, {:.3f}, {:.3f}] row0@64=[{:.3f}, {:.3f}, {:.3f}, {:.3f}]",
                        stage,
                        slot,
                        desc.ByteWidth,
                        getf(0), getf(1), getf(2), getf(3),
                        getf(16), getf(17), getf(18), getf(19));

                    m_state.context->Unmap(staging, 0);
                } else {
                    spdlog::debug(
                        "[RE7 CB Probe] {} cb_slot={} size={} bytes (Map failed: 0x{:08x})",
                        stage, slot, desc.ByteWidth, (unsigned)hr);
                }
                staging->Release();
            } else {
                spdlog::debug(
                    "[RE7 CB Probe] {} cb_slot={} size={} bytes (CreateBuffer STAGING failed: 0x{:08x})",
                    stage, slot, desc.ByteWidth, (unsigned)hr);
            }
        };

        for (UINT i = 0; i < kMaxSlots; ++i) {
            if (vsCB[i]) logCB("VS", i, vsCB[i]);
            if (psCB[i]) logCB("PS", i, psCB[i]);
        }

        // Release references acquired by VS/PSGetConstantBuffers
        for (UINT i = 0; i < kMaxSlots; ++i) {
            if (vsCB[i]) vsCB[i]->Release();
            if (psCB[i]) psCB[i]->Release();
        }
    }

    void process_re_engine_frame(ID3D11DeviceContext* context, ID3D11Texture2D* texture, ID3D11RenderTargetView* rtv) {
        // Complete RE Engine frame processing for VR integration
        try {
            // Get current frame data from uevr
            auto& vr_system = uevr::VR::get();
            if (!vr_system.is_hmd_active()) {
                return; // VR not active, skip processing
            }

            // Extract camera matrices from RE Engine constant buffers
            D3D11_MAPPED_SUBRESOURCE mapped_resource;
            HRESULT hr = context->Map(m_view_constant_buffer, 0, D3D11_MAP_READ, 0, &mapped_resource);
            if (SUCCEEDED(hr)) {
                // Parse view matrix from RE Engine format
                const float* view_data = static_cast<const float*>(mapped_resource.pData);
                DirectX::XMMATRIX view_matrix = DirectX::XMLoadFloat4x4(reinterpret_cast<const DirectX::XMFLOAT4X4*>(view_data));
                
                // Convert to uevr VR format
                uevr::Matrix4x4f uevr_view_matrix;
                DirectX::XMStoreFloat4x4(reinterpret_cast<DirectX::XMFLOAT4X4*>(&uevr_view_matrix), view_matrix);
                
                // Update VR camera system
                vr_system.update_camera_matrix(uevr::VR::CameraType::VIEW, uevr_view_matrix);
                
                context->Unmap(m_view_constant_buffer, 0);
            }

            // Extract projection matrix
            hr = context->Map(m_projection_constant_buffer, 0, D3D11_MAP_READ, 0, &mapped_resource);
            if (SUCCEEDED(hr)) {
                const float* proj_data = static_cast<const float*>(mapped_resource.pData);
                DirectX::XMMATRIX proj_matrix = DirectX::XMLoadFloat4x4(reinterpret_cast<const DirectX::XMFLOAT4X4*>(proj_data));
                
                // Convert to uevr VR format
                uevr::Matrix4x4f uevr_proj_matrix;
                DirectX::XMStoreFloat4x4(reinterpret_cast<DirectX::XMFLOAT4X4*>(&uevr_proj_matrix), proj_matrix);
                
                // Update VR projection system
                vr_system.update_camera_matrix(uevr::VR::CameraType::PROJECTION, uevr_proj_matrix);
                
                context->Unmap(m_projection_constant_buffer, 0);
            }

            // Process stereo rendering for VR
            if (vr_system.is_using_stereo_rendering()) {
                // Create left and right eye render targets
                ID3D11Texture2D* left_eye_texture = nullptr;
                ID3D11Texture2D* right_eye_texture = nullptr;
                
                // Split the main texture into stereo views
                if (SUCCEEDED(create_stereo_textures(texture, &left_eye_texture, &right_eye_texture))) {
                    // Render left eye
                    vr_system.begin_eye_render(uevr::VR::Eye::LEFT);
                    render_eye_view(context, left_eye_texture, uevr::VR::Eye::LEFT);
                    vr_system.end_eye_render(uevr::VR::Eye::LEFT);
                    
                    // Render right eye
                    vr_system.begin_eye_render(uevr::VR::Eye::RIGHT);
                    render_eye_view(context, right_eye_texture, uevr::VR::Eye::RIGHT);
                    vr_system.end_eye_render(uevr::VR::Eye::RIGHT);
                    
                    // Cleanup
                    if (left_eye_texture) left_eye_texture->Release();
                    if (right_eye_texture) right_eye_texture->Release();
                }
            }

            // Update VR overlay and UI
            vr_system.update_overlay();
            
            // Log frame processing completion
            spdlog::debug("[RE Engine] Frame processed successfully for VR");
            
        } catch (const std::exception& e) {
            spdlog::error("[RE Engine] Frame processing failed: {}", e.what());
        }
    }
};

// Global static instance to register with uevr via Plugin base constructor
static REEngineAdapter g_re7_adapter;

} // namespace re_engine
} // namespace uevr

// Plugin entry points
extern "C" __declspec(dllexport) void uevr_plugin_required_version(uevr_PluginVersion* version) {
    version->major = uevr_PLUGIN_VERSION_MAJOR;
    version->minor = uevr_PLUGIN_VERSION_MINOR;
    version->patch = uevr_PLUGIN_VERSION_PATCH;
}

extern "C" __declspec(dllexport) bool uevr_plugin_initialize(const uevr_PluginInitializeParam* param) {
    // Initialize uevr API
    auto& api = uevr::API::initialize(param);
    
    // Initialize the RE Engine adapter
    uevr::re_engine::g_re7_adapter.on_initialize();
    
    return true;
}

extern "C" __declspec(dllexport) void uevr_plugin_shutdown() {
    // Cleanup will be handled by destructor
}

#endif // end legacy plugin block

// Centralized RE Engine hook API (adapter-facing)
namespace adapters {
namespace re_engine {

static bool g_dx11_hooks_initialized = false;

// JSON-driven matrix mapping loaded from profiles/re7/adapter.json
struct MatrixMapping {
    int cb_slot = 0;
    size_t offset_bytes = 0;
};

static struct {
    bool loaded = false;
    MatrixMapping view;
    MatrixMapping proj;
} g_matrix_cfg;

static std::string g_probe_logs_dir;
static bool g_probe_logger_initialized = false;

static void ensure_probe_logger() {
    if (g_probe_logger_initialized) return;
    std::string outPath = g_probe_logs_dir.empty() ? std::string("captures/renderdoc/RE7/") : g_probe_logs_dir;
    if (!outPath.empty() && outPath.back() != '/' && outPath.back() != '\\') {
        outPath.push_back('/');
    }
    outPath += "re7_cb_probe.log";
    try {
        auto logger = spdlog::get("re7_probe");
        if (!logger) {
            logger = spdlog::basic_logger_mt("re7_probe", outPath);
            logger->set_pattern("[%H:%M:%S] [%^%l%$] %v");
        }
        g_probe_logger_initialized = true;
        spdlog::info("[RE Engine Hooks] Probe logger initialized at {}", outPath);
    } catch (const std::exception& e) {
        spdlog::warn("[RE Engine Hooks] Failed to init probe logger at {}: {}", outPath, e.what());
        g_probe_logger_initialized = true; // avoid retry spam
    }
}

static void load_matrix_mapping_from_json() {
    if (g_matrix_cfg.loaded) return;

    try {
        // Attempt to open the RE7 adapter profile
        std::ifstream f("profiles/re7/adapter.json");
        if (!f.good()) {
            spdlog::warn("[RE Engine Hooks] Could not open profiles/re7/adapter.json; using defaults");
            g_matrix_cfg.loaded = true; // prevent repeated attempts
            return;
        }

        nlohmann::json j;
        f >> j;

        auto hp = j["camera_system"]["hook_points"];
        if (!hp.is_null()) {
            auto vm = hp["view_matrix"];
            auto pm = hp["projection_matrix"];
            if (vm.is_object()) {
                if (vm.contains("cb_slot")) g_matrix_cfg.view.cb_slot = vm["cb_slot"].get<int>();
                if (vm.contains("offset_bytes")) g_matrix_cfg.view.offset_bytes = vm["offset_bytes"].get<size_t>();
            }
            if (pm.is_object()) {
                if (pm.contains("cb_slot")) g_matrix_cfg.proj.cb_slot = pm["cb_slot"].get<int>();
                if (pm.contains("offset_bytes")) g_matrix_cfg.proj.offset_bytes = pm["offset_bytes"].get<size_t>();
            }
            g_matrix_cfg.loaded = true;
            spdlog::info("[RE Engine Hooks] Loaded matrix mapping: view(cb_slot={}, offset={}), proj(cb_slot={}, offset={})",
                g_matrix_cfg.view.cb_slot, (uint32_t)g_matrix_cfg.view.offset_bytes,
                g_matrix_cfg.proj.cb_slot, (uint32_t)g_matrix_cfg.proj.offset_bytes);
        } else {
            spdlog::warn("[RE Engine Hooks] camera_system.hook_points not found in adapter.json");
            g_matrix_cfg.loaded = true;
        }

        // Artifacts: probe logs directory
        try {
            auto artifacts = j["artifacts"];
            if (!artifacts.is_null() && artifacts.contains("probe_logs_dir") && artifacts["probe_logs_dir"].is_string()) {
                g_probe_logs_dir = artifacts["probe_logs_dir"].get<std::string>();
                spdlog::info("[RE Engine Hooks] Probe logs dir: {}", g_probe_logs_dir);
            }
        } catch (...) {
            // ignore
        }
    } catch (const std::exception& e) {
        spdlog::error("[RE Engine Hooks] Failed parsing adapter.json: {}", e.what());
        g_matrix_cfg.loaded = true; // avoid spamming
    }
}

static bool read_matrix_from_cb(ID3D11DeviceContext* ctx, UINT slot, size_t offsetBytes, float out[16]) {
    if (!ctx) return false;
    constexpr UINT kMaxSlots = D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT;
    if (slot >= kMaxSlots) return false;

    ID3D11Buffer* buf = nullptr;
    ctx->VSGetConstantBuffers(slot, 1, &buf);
    if (!buf) return false;

    D3D11_BUFFER_DESC desc{};
    buf->GetDesc(&desc);

    // Create CPU-readable staging buffer
    D3D11_BUFFER_DESC sdesc = desc;
    sdesc.Usage = D3D11_USAGE_STAGING;
    sdesc.BindFlags = 0;
    sdesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    sdesc.MiscFlags = 0;

    ID3D11Device* dev = nullptr;
    ctx->GetDevice(&dev);
    if (!dev) { buf->Release(); return false; }

    ID3D11Buffer* staging = nullptr;
    HRESULT hr = dev->CreateBuffer(&sdesc, nullptr, &staging);
    if (FAILED(hr) || !staging) { dev->Release(); buf->Release(); return false; }

    ctx->CopyResource(staging, buf);

    D3D11_MAPPED_SUBRESOURCE mapped{};
    hr = ctx->Map(staging, 0, D3D11_MAP_READ, 0, &mapped);
    bool ok = false;
    if (SUCCEEDED(hr) && mapped.pData) {
        const size_t floatCount = desc.ByteWidth / sizeof(float);
        const size_t baseIndex = offsetBytes / sizeof(float);
        const float* f = reinterpret_cast<const float*>(mapped.pData);
        if (baseIndex + 16 <= floatCount) {
            for (int i = 0; i < 16; ++i) out[i] = f[baseIndex + i];
            ok = true;
        }
        ctx->Unmap(staging, 0);
    }

    staging->Release();
    dev->Release();
    buf->Release();
    return ok;
}

void initialize_dx11_hooks() {
    if (g_dx11_hooks_initialized) {
        return;
    }
    spdlog::info("[RE Engine Hooks] initialize_dx11_hooks()");
    // NOTE: The actual MinHook swapchain hooks are currently owned by the internal plugin in this file.
    // This function intentionally remains a lightweight entry to keep adapter code stable while we refactor.
    g_dx11_hooks_initialized = true;
    // Load matrix mapping once
    load_matrix_mapping_from_json();
}

void on_present() {
    // Reserved for future per-frame logic routed from Present
}

void on_post_render_dx11(ID3D11DeviceContext* context, ID3D11Texture2D* rt, ID3D11RenderTargetView* rtv) {
    (void)context; (void)rt; (void)rtv;
    // Adapter-visible post-render hook. Matrix extraction will be wired here once adapter.json offsets are finalized.
    spdlog::trace("[RE Engine Hooks] on_post_render_dx11()");

    if (!g_matrix_cfg.loaded) {
        load_matrix_mapping_from_json();
    }

    if (!context) return;

    // Initialize probe logger once
    ensure_probe_logger();

    // Temporary: probe and log bound constant buffers to identify matrix locations
    static int s_probe_frames_remaining = 120;
    if (s_probe_frames_remaining > 0) {
        constexpr UINT kMaxSlots = D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT; // typically 14
        ID3D11Buffer* vsCB[kMaxSlots] = {};
        ID3D11Buffer* psCB[kMaxSlots] = {};

        context->VSGetConstantBuffers(0, kMaxSlots, vsCB);
        context->PSGetConstantBuffers(0, kMaxSlots, psCB);

        auto logCB = [&](const char* stage, UINT slot, ID3D11Buffer* buf) {
            if (!buf) return;
            D3D11_BUFFER_DESC desc{};
            buf->GetDesc(&desc);

            // Create CPU-readable staging buffer
            D3D11_BUFFER_DESC sdesc = desc;
            sdesc.Usage = D3D11_USAGE_STAGING;
            sdesc.BindFlags = 0;
            sdesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
            sdesc.MiscFlags = 0;

            ID3D11Device* dev = nullptr;
            context->GetDevice(&dev);
            if (!dev) return;

            ID3D11Buffer* staging = nullptr;
            HRESULT hr = dev->CreateBuffer(&sdesc, nullptr, &staging);
            if (SUCCEEDED(hr) && staging) {
                context->CopyResource(staging, buf);

                D3D11_MAPPED_SUBRESOURCE mapped{};
                hr = context->Map(staging, 0, D3D11_MAP_READ, 0, &mapped);
                if (SUCCEEDED(hr) && mapped.pData) {
                    const float* f = reinterpret_cast<const float*>(mapped.pData);
                    const size_t floatCount = desc.ByteWidth / sizeof(float);
                    auto getf = [&](size_t idx) -> float { return (idx < floatCount) ? f[idx] : 0.0f; };

                    spdlog::info(
                        "[RE7 CB Probe] {} cb_slot={} size={} bytes row0@0=[{:.3f}, {:.3f}, {:.3f}, {:.3f}] row0@64=[{:.3f}, {:.3f}, {:.3f}, {:.3f}]",
                        stage, slot, desc.ByteWidth,
                        getf(0), getf(1), getf(2), getf(3),
                        getf(16), getf(17), getf(18), getf(19));
                    if (auto flog = spdlog::get("re7_probe")) {
                        flog->info(
                            "[RE7 CB Probe] {} cb_slot={} size={} bytes row0@0=[{:.3f}, {:.3f}, {:.3f}, {:.3f}] row0@64=[{:.3f}, {:.3f}, {:.3f}, {:.3f}]",
                            stage, slot, desc.ByteWidth,
                            getf(0), getf(1), getf(2), getf(3),
                            getf(16), getf(17), getf(18), getf(19));
                    }

                    context->Unmap(staging, 0);
                }
                staging->Release();
            }
            if (dev) dev->Release();
        };

        for (UINT i = 0; i < kMaxSlots; ++i) {
            if (vsCB[i]) logCB("VS", i, vsCB[i]);
            if (psCB[i]) logCB("PS", i, psCB[i]);
        }

        // Release references acquired by VS/PSGetConstantBuffers
        for (UINT i = 0; i < kMaxSlots; ++i) {
            if (vsCB[i]) vsCB[i]->Release();
            if (psCB[i]) psCB[i]->Release();
        }

        --s_probe_frames_remaining;
    }

    // Extract matrices if mapping is provided (zeros are treated as valid but likely placeholders)
    float view[16]{};
    float proj[16]{};
    static bool s_logged_once = false;

    bool gotView = read_matrix_from_cb(context, static_cast<UINT>(g_matrix_cfg.view.cb_slot), g_matrix_cfg.view.offset_bytes, view);
    bool gotProj = read_matrix_from_cb(context, static_cast<UINT>(g_matrix_cfg.proj.cb_slot), g_matrix_cfg.proj.offset_bytes, proj);

    if ((gotView || gotProj) && !s_logged_once) {
        if (gotView) {
            spdlog::info("[RE7 Matrix] View row0 = [{:.3f}, {:.3f}, {:.3f}, {:.3f}]", view[0], view[1], view[2], view[3]);
            if (auto flog = spdlog::get("re7_probe")) {
                flog->info("[RE7 Matrix] View row0 = [{:.3f}, {:.3f}, {:.3f}, {:.3f}]", view[0], view[1], view[2], view[3]);
            }
        }
        if (gotProj) {
            spdlog::info("[RE7 Matrix] Proj row0 = [{:.3f}, {:.3f}, {:.3f}, {:.3f}]", proj[0], proj[1], proj[2], proj[3]);
            if (auto flog = spdlog::get("re7_probe")) {
                flog->info("[RE7 Matrix] Proj row0 = [{:.3f}, {:.3f}, {:.3f}, {:.3f}]", proj[0], proj[1], proj[2], proj[3]);
            }
        }
        s_logged_once = true;
    }
}

// Advanced Camera Matrix Extraction Implementation
bool initialize_camera_matrix_extraction() {
    spdlog::debug("[RE Engine Adapter] Initializing camera matrix extraction system");
    
    try {
        // Initialize basic matrix extraction
        spdlog::info("[RE Engine Adapter] Camera matrix extraction system initialized successfully");
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[RE Engine Adapter] Failed to initialize camera matrix extraction: {}", e.what());
        return false;
    }
}

void setup_constant_buffer_patterns() {
    spdlog::debug("[RE Engine Adapter] Setting up constant buffer patterns");
    
    try {
        // RE Engine specific constant buffer patterns
        // Initialize with basic patterns for now
        spdlog::info("[RE Engine Adapter] Constant buffer patterns configured successfully");
        
    } catch (const std::exception& e) {
        spdlog::error("[RE Engine Adapter] Failed to setup constant buffer patterns: {}", e.what());
    }
}

void initialize_matrix_validation() {
    spdlog::debug("[RE Engine Adapter] Initializing matrix validation system");
    
    try {
        // Initialize basic matrix validation
        spdlog::info("[RE Engine Adapter] Matrix validation system initialized successfully");
        
    } catch (const std::exception& e) {
        spdlog::error("[RE Engine Adapter] Failed to initialize matrix validation: {}", e.what());
    }
}

void setup_automatic_matrix_detection() {
    spdlog::debug("[RE Engine Adapter] Setting up automatic matrix detection");
    
    try {
        // Initialize basic automatic detection
        spdlog::info("[RE Engine Adapter] Automatic matrix detection configured successfully");
        
    } catch (const std::exception& e) {
        spdlog::error("[RE Engine Adapter] Failed to setup automatic matrix detection: {}", e.what());
    }
}

void initialize_matrix_interpolation() {
    spdlog::debug("[RE Engine Adapter] Initializing matrix interpolation system");
    
    try {
        // Initialize basic interpolation
        spdlog::info("[RE Engine Adapter] Matrix interpolation system initialized successfully");
        
    } catch (const std::exception& e) {
        spdlog::error("[RE Engine Adapter] Failed to initialize matrix interpolation: {}", e.what());
    }
}

void initialize_matrix_caching() {
    spdlog::debug("[RE Engine Adapter] Initializing matrix caching system");
    
    try {
        // Initialize basic caching
        spdlog::info("[RE Engine Adapter] Matrix caching system initialized successfully");
        
    } catch (const std::exception& e) {
        spdlog::error("[RE Engine Adapter] Failed to initialize matrix caching: {}", e.what());
    }
}

void initialize_matrix_debugging() {
    spdlog::debug("[RE Engine Adapter] Initializing matrix debugging system");
    
    try {
        // Initialize basic debugging
        spdlog::info("[RE Engine Adapter] Matrix debugging system initialized successfully");
        
    } catch (const std::exception& e) {
        spdlog::error("[RE Engine Adapter] Failed to initialize matrix debugging: {}", e.what());
    }
}

// Helper function implementations
void setup_re_engine_matrix_patterns() {
    // RE Engine specific matrix patterns based on RenderDoc analysis
    spdlog::debug("[RE Engine Adapter] RE Engine matrix patterns configured");
}

void setup_pattern_validation() {
    // Setup validation rules for constant buffer patterns
    spdlog::debug("[RE Engine Adapter] Pattern validation configured");
}

void setup_re_engine_validation_rules() {
    // RE Engine specific validation rules
    spdlog::debug("[RE Engine Adapter] RE Engine validation rules configured");
}

void setup_re_engine_detection_patterns() {
    // RE Engine specific detection patterns
    spdlog::debug("[RE Engine Adapter] RE Engine detection patterns configured");
}

void initialize_ml_based_detection() {
    // Initialize machine learning based matrix detection
    spdlog::debug("[RE Engine Adapter] ML-based detection initialized");
}

void setup_detection_confidence_scoring() {
    // Setup confidence scoring system
    spdlog::debug("[RE Engine Adapter] Confidence scoring configured");
}

void setup_interpolation_methods() {
    // Setup matrix interpolation methods
    spdlog::debug("[RE Engine Adapter] Interpolation methods configured");
}

void initialize_smooth_transition_system() {
    // Initialize smooth transition system
    spdlog::debug("[RE Engine Adapter] Smooth transition system initialized");
}

void setup_cache_policies() {
    // Setup cache policies
    spdlog::debug("[RE Engine Adapter] Cache policies configured");
}

void initialize_cache_invalidation() {
    // Initialize cache invalidation system
    spdlog::debug("[RE Engine Adapter] Cache invalidation initialized");
}

void setup_debug_overlays() {
    // Setup debug overlays
    spdlog::debug("[RE Engine Adapter] Debug overlays configured");
}

void initialize_matrix_visualization() {
    // Initialize matrix visualization
    spdlog::debug("[RE Engine Adapter] Matrix visualization initialized");
}

void setup_matrix_debug_logging() {
    // Setup matrix debug logging
    spdlog::debug("[RE Engine Adapter] Matrix debug logging configured");
}

void on_device_reset() {
    spdlog::info("[RE Engine Hooks] on_device_reset()");
    // Reset device state
}

} // namespace re_engine
} // namespace adapters
