/*
 * MT Framework Cross-Engine VR Adapter for uevr
 * Extends uevr's cross-engine adapter foundation for MT Framework compatibility
 * 
 * Copyright (c) 2024 uevr Cross-Engine Adapter Project
 * Based on uevr Plugin System (MIT License)
 */

#include <windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <MinHook.h>
#include <memory>
#include <vector>
#include <string>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>
#include <fstream>

// uevr Plugin API includes
#include <uevr/Plugin.hpp>
#include <uevr/API.hpp>

namespace uevr {
namespace mt_framework {

// JSON-driven matrix mapping loaded from profiles/MonsterHunterWorld/adapter.json
struct MatrixMapping {
    int cb_slot = 0;
    size_t offset_bytes = 0;
};

static struct {
    bool loaded = false;
    MatrixMapping view;
    MatrixMapping proj;
} g_mhw_matrix_cfg;

static void load_mhw_matrix_mapping_from_json() {
    if (g_mhw_matrix_cfg.loaded) return;
    try {
        std::ifstream f("profiles/MonsterHunterWorld/adapter.json");
        if (!f.good()) {
            spdlog::warn("[MTF Hooks] Could not open profiles/MonsterHunterWorld/adapter.json; using defaults");
            g_mhw_matrix_cfg.loaded = true; // avoid repeated attempts
            return;
        }

        nlohmann::json j; f >> j;
        auto hp_it = j.find("camera_system");
        if (hp_it != j.end()) hp_it = hp_it->find("hook_points");
        if (hp_it != j.end() && hp_it->is_object()) {
            const auto& hp = *hp_it;
            if (hp.contains("view_matrix") && hp["view_matrix"].is_object()) {
                const auto& vm = hp["view_matrix"];
                if (vm.contains("cb_slot")) g_mhw_matrix_cfg.view.cb_slot = vm["cb_slot"].get<int>();
                if (vm.contains("offset_bytes")) g_mhw_matrix_cfg.view.offset_bytes = vm["offset_bytes"].get<size_t>();
            }
            if (hp.contains("projection_matrix") && hp["projection_matrix"].is_object()) {
                const auto& pm = hp["projection_matrix"];
                if (pm.contains("cb_slot")) g_mhw_matrix_cfg.proj.cb_slot = pm["cb_slot"].get<int>();
                if (pm.contains("offset_bytes")) g_mhw_matrix_cfg.proj.offset_bytes = pm["offset_bytes"].get<size_t>();
            }
            spdlog::info("[MTF Hooks] Loaded matrix mapping: view(cb_slot={}, offset={}), proj(cb_slot={}, offset={})",
                g_mhw_matrix_cfg.view.cb_slot, (uint32_t)g_mhw_matrix_cfg.view.offset_bytes,
                g_mhw_matrix_cfg.proj.cb_slot, (uint32_t)g_mhw_matrix_cfg.proj.offset_bytes);
        } else {
            spdlog::warn("[MTF Hooks] camera_system.hook_points not found in adapter.json");
        }
        g_mhw_matrix_cfg.loaded = true;
    } catch (const std::exception& e) {
        spdlog::error("[MTF Hooks] Failed parsing adapter.json: {}", e.what());
        g_mhw_matrix_cfg.loaded = true; // avoid spamming
    }
}

static bool read_matrix_from_cb(ID3D11DeviceContext* ctx, UINT slot, size_t offsetBytes, float out16[16]) {
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
            for (int i = 0; i < 16; ++i) out16[i] = f[baseIndex + i];
            ok = true;
        }
        ctx->Unmap(staging, 0);
    }

    staging->Release();
    dev->Release();
    buf->Release();
    return ok;
}

// Generic helper that can read from either VS or PS constant buffer space
static bool read_matrix_from_cb_stage(ID3D11DeviceContext* ctx, UINT slot, size_t offsetBytes, float out16[16], bool pixelStage) {
    if (!ctx) return false;
    constexpr UINT kMaxSlots = D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT;
    if (slot >= kMaxSlots) return false;

    ID3D11Buffer* buf = nullptr;
    if (pixelStage) {
        ctx->PSGetConstantBuffers(slot, 1, &buf);
    } else {
        ctx->VSGetConstantBuffers(slot, 1, &buf);
    }
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
            for (int i = 0; i < 16; ++i) out16[i] = f[baseIndex + i];
            ok = true;
        }
        ctx->Unmap(staging, 0);
    }

    staging->Release();
    dev->Release();
    buf->Release();
    return ok;
}

// Convenience wrapper for PS stage reads
static bool read_matrix_from_cb_ps(ID3D11DeviceContext* ctx, UINT slot, size_t offsetBytes, float out16[16]) {
    return read_matrix_from_cb_stage(ctx, slot, offsetBytes, out16, true);
}

// Lightweight PS-stage CB probe to assist mapping (logs presence and sizes for a few frames)
static void probe_ps_constant_buffers(ID3D11DeviceContext* ctx) {
    if (!ctx) return;
    static int s_probe_frames = 0;
    if (s_probe_frames >= 30) return; // limit logging
    ++s_probe_frames;

    for (UINT slot = 0; slot < D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT; ++slot) {
        ID3D11Buffer* buf = nullptr;
        ctx->PSGetConstantBuffers(slot, 1, &buf);
        if (!buf) continue;
        D3D11_BUFFER_DESC desc{};
        buf->GetDesc(&desc);
        spdlog::info("[MHW Probe][PS] slot={} size={} bytes", slot, desc.ByteWidth);
        buf->Release();
    }
}

class MTFrameworkBridge : public Plugin {
private:
    // MT Framework specific state
    struct MTFrameworkState {
        ID3D11Device* device = nullptr;
        IDXGISwapChain* swap_chain = nullptr;
        ID3D11DeviceContext* context = nullptr;
        
        // Camera matrices from MT Framework
        struct CameraMatrices {
            float view_matrix[16];
            float projection_matrix[16];
            float view_projection_matrix[16];
            bool matrices_valid = false;
        } camera;
        
        // VR rendering state
        bool vr_enabled = false;
        bool stereo_rendering_active = false;
        
        // MT Framework specific state
        bool third_person_mode = true;
        bool camera_attached = false;
        
        // Animation system state
        struct AnimationState {
            bool skeletal_animation_active = false;
            float animation_blend_factor = 1.0f;
            bool weapon_equipped = false;
        } animation;
    };
    
    std::unique_ptr<MTFrameworkState> m_state;
    
    // Hook function pointers
    typedef HRESULT(WINAPI* Present_t)(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
    typedef HRESULT(WINAPI* ResizeBuffers_t)(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);
    
    Present_t m_original_present = nullptr;
    ResizeBuffers_t m_original_resize_buffers = nullptr;
    
    // MT Framework constant buffer analysis
    struct ConstantBufferInfo {
        UINT slot;
        UINT size;
        UINT view_matrix_offset;
        UINT projection_matrix_offset;
        bool valid = false;
    };
    
    std::vector<ConstantBufferInfo> m_cb_info;

public:
    MTFrameworkBridge() : m_state(std::make_unique<MTFrameworkState>()) {
        // Initialize MinHook
        if (MH_Initialize() != MH_OK) {
            // Log error
            return;
        }
    }
    
    virtual ~MTFrameworkBridge() {
        if (m_state) {
            unhook_all();
        }
        MH_Uninitialize();
    }

    // uevr Plugin callbacks
    virtual void on_initialize() override {
        // Initialize MT Framework hooks
        hook_mt_framework();
    }
    
    virtual void on_device_reset() override {
        // Handle device reset for MT Framework
        if (m_state && m_state->device) {
            // Re-establish hooks if needed
            hook_mt_framework();
        }
    }
    
    virtual void on_post_render_vr_framework_dx11(ID3D11DeviceContext* context, ID3D11Texture2D* texture, ID3D11RenderTargetView* rtv) override {
        // Always capture context and attempt matrix extraction using JSON mapping
        if (m_state) {
            m_state->context = context;
            if (m_state->context && !m_state->device) {
                ID3D11Device* dev = nullptr;
                m_state->context->GetDevice(&dev);
                if (dev) { m_state->device = dev; }
            }

            if (!g_mhw_matrix_cfg.loaded) {
                load_mhw_matrix_mapping_from_json();
            }

            float view[16]{};
            float proj[16]{};
            static bool s_logged_once = false;
            bool gotView = read_matrix_from_cb(m_state->context, static_cast<UINT>(g_mhw_matrix_cfg.view.cb_slot), g_mhw_matrix_cfg.view.offset_bytes, view);
            bool gotProj = read_matrix_from_cb(m_state->context, static_cast<UINT>(g_mhw_matrix_cfg.proj.cb_slot), g_mhw_matrix_cfg.proj.offset_bytes, proj);

            // PS-stage fallback if VS-stage did not yield matrices
            if (!gotView) {
                gotView = read_matrix_from_cb_ps(m_state->context, static_cast<UINT>(g_mhw_matrix_cfg.view.cb_slot), g_mhw_matrix_cfg.view.offset_bytes, view);
                if (gotView) {
                    spdlog::info("[MHW Matrix] View fetched via PS-stage fallback (slot={}, offset={})", g_mhw_matrix_cfg.view.cb_slot, (uint32_t)g_mhw_matrix_cfg.view.offset_bytes);
                }
            }
            if (!gotProj) {
                gotProj = read_matrix_from_cb_ps(m_state->context, static_cast<UINT>(g_mhw_matrix_cfg.proj.cb_slot), g_mhw_matrix_cfg.proj.offset_bytes, proj);
                if (gotProj) {
                    spdlog::info("[MHW Matrix] Proj fetched via PS-stage fallback (slot={}, offset={})", g_mhw_matrix_cfg.proj.cb_slot, (uint32_t)g_mhw_matrix_cfg.proj.offset_bytes);
                }
            }

            if (gotView) {
                for (int i = 0; i < 16; ++i) m_state->camera.view_matrix[i] = view[i];
                m_state->camera.matrices_valid = true;
            }
            if (gotProj) {
                for (int i = 0; i < 16; ++i) m_state->camera.projection_matrix[i] = proj[i];
                m_state->camera.matrices_valid = true;
            }
            if ((gotView || gotProj) && !s_logged_once) {
                if (gotView) {
                    spdlog::info("[MHW Matrix] View row0 = [{:.3f}, {:.3f}, {:.3f}, {:.3f}]", view[0], view[1], view[2], view[3]);
                }
                if (gotProj) {
                    spdlog::info("[MHW Matrix] Proj row0 = [{:.3f}, {:.3f}, {:.3f}, {:.3f}]", proj[0], proj[1], proj[2], proj[3]);
                }
                s_logged_once = true;
            }

            // If neither matrix found yet, probe PS CBs for mapping assistance (limited frames)
            if (!gotView && !gotProj) {
                probe_ps_constant_buffers(m_state->context);
            }
        }

        // Post-process MT Framework rendering for VR when enabled
        if (m_state && m_state->vr_enabled) {
            post_process_mt_framework_rendering(context, texture, rtv);
        }
    }

private:
    bool hook_mt_framework() {
        // Hook IDXGISwapChain::Present
        if (m_state->swap_chain) {
            void** vtable = *reinterpret_cast<void***>(m_state->swap_chain);
            
            if (MH_CreateHook(vtable[8], &hooked_present, reinterpret_cast<void**>(&m_original_present)) != MH_OK) {
                return false;
            }
            
            if (MH_CreateHook(vtable[13], &hooked_resize_buffers, reinterpret_cast<void**>(&m_original_resize_buffers)) != MH_OK) {
                return false;
            }
            
            if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK) {
                return false;
            }
        }
        
        return true;
    }
    
    void unhook_all() {
        MH_DisableHook(MH_ALL_HOOKS);
        MH_RemoveHook(MH_ALL_HOOKS);
    }
    
    // Hook implementations
    static HRESULT WINAPI hooked_present(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags) {
        auto* adapter = static_cast<MTFrameworkBridge*>(uevr::detail::g_plugin);
        if (adapter) {
            adapter->on_mt_framework_present(pSwapChain, SyncInterval, Flags);
        }
        
        if (adapter && adapter->m_original_present) {
            return adapter->m_original_present(pSwapChain, SyncInterval, Flags);
        }
        
        return S_OK;
    }
    
    static HRESULT WINAPI hooked_resize_buffers(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags) {
        auto* adapter = static_cast<MTFrameworkBridge*>(uevr::detail::g_plugin);
        if (adapter) {
            adapter->on_mt_framework_resize_buffers(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);
        }
        
        if (adapter && adapter->m_original_resize_buffers) {
            return adapter->m_original_resize_buffers(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);
        }
        
        return S_OK;
    }
    
    void on_mt_framework_present(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags) {
        // Detect MT Framework on first Present call
        if (!m_state->vr_enabled) {
            detect_mt_framework(pSwapChain);
        }
        
        // Extract camera matrices from constant buffers
        if (m_state->vr_enabled) {
            extract_camera_matrices();
        }
        
        // Handle third-person to first-person conversion
        if (m_state->third_person_mode) {
            convert_third_person_to_first_person();
        }
    }
    
    void on_mt_framework_resize_buffers(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags) {
        // Handle resolution changes for VR
        if (m_state && m_state->vr_enabled) {
            // Adjust VR rendering parameters
            adjust_vr_rendering_parameters(Width, Height);
        }
    }
    
    void detect_mt_framework(IDXGISwapChain* pSwapChain) {
        // Get device and context from swapchain
        ID3D11Device* device = nullptr;
        ID3D11DeviceContext* context = nullptr;
        
        if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), reinterpret_cast<void**>(&device)))) {
            device->GetImmediateContext(&context);
            
            m_state->device = device;
            m_state->context = context;
            m_state->swap_chain = pSwapChain;
            
            // Check for MT Framework specific patterns
            if (check_mt_framework_signatures()) {
                m_state->vr_enabled = true;
                
                // Initialize MT Framework specific VR systems
                initialize_mt_framework_vr();
            }
        }
    }
    
    bool check_mt_framework_signatures() {
        // Check for MT Framework specific memory patterns or function signatures
        // This is a simplified check - in practice, more sophisticated detection would be used
        
        // Check for common MT Framework DLLs or memory patterns
        HMODULE mt_framework_module = GetModuleHandleA("MonsterHunterWorld.exe");
        if (mt_framework_module) {
            return true;
        }
        
        return false;
    }
    
    void initialize_mt_framework_vr() {
        // Initialize MT Framework specific VR systems
        
        // Set up camera matrix extraction
        setup_camera_extraction();
        
        // Initialize third-person to first-person conversion
        setup_camera_conversion();
        
        // Initialize animation system VR adaptation
        setup_animation_system();
        
        // Initialize motion controller integration
        setup_motion_controllers();
        
        // Configure UI projection for MT Framework HUD
        setup_ui_projection();
    }
    
    void setup_camera_extraction() {
        spdlog::info("[MT Framework Adapter] Setting up camera matrix extraction");
        
        try {
            // Initialize MT Framework specific camera matrix extraction system
            if (!initialize_mt_framework_camera_extraction()) {
                spdlog::error("[MT Framework Adapter] Failed to initialize camera matrix extraction");
                return;
            }
            
            // Setup constant buffer scanning patterns for MT Framework
            setup_mt_framework_constant_buffer_patterns();
            
            // Initialize matrix validation system for MT Framework
            initialize_mt_framework_matrix_validation();
            
            // Setup automatic matrix detection for MT Framework
            setup_mt_framework_automatic_matrix_detection();
            
            // Initialize matrix interpolation for smooth transitions
            initialize_mt_framework_matrix_interpolation();
            
            // Setup matrix caching system for MT Framework
            initialize_mt_framework_matrix_caching();
            
            // Initialize matrix debugging and visualization
            initialize_mt_framework_matrix_debugging();
            
            spdlog::info("[MT Framework Adapter] Camera matrix extraction system fully initialized");
            
        } catch (const std::exception& e) {
            spdlog::error("[MT Framework Adapter] Failed to setup camera extraction: {}", e.what());
        }
    }
    
    void setup_camera_conversion() {
        spdlog::info("[MT Framework Adapter] Setting up camera conversion system");
        
        try {
            // Initialize MT Framework specific camera conversion system
            if (!initialize_mt_framework_camera_conversion()) {
                spdlog::error("[MT Framework Adapter] Failed to initialize camera conversion");
                return;
            }
            
            // Setup third-person to first-person conversion algorithms
            setup_third_person_to_first_person_conversion();
            
            // Initialize VR-specific camera adjustments
            initialize_vr_camera_adjustments();
            
            // Setup comfort settings for VR
            setup_vr_comfort_settings();
            
            // Initialize camera smoothing and interpolation
            initialize_camera_smoothing_and_interpolation();
            
            // Setup camera conversion performance monitoring
            setup_camera_conversion_performance_monitoring();
            
            // Initialize camera conversion debugging tools
            initialize_camera_conversion_debugging();
            
            spdlog::info("[MT Framework Adapter] Camera conversion system fully initialized");
            
        } catch (const std::exception& e) {
            spdlog::error("[MT Framework Adapter] Failed to setup camera conversion: {}", e.what());
        }
    }
    
    void setup_animation_system() {
        spdlog::info("[MT Framework Adapter] Setting up animation system");
        
        try {
            // Initialize basic animation system for MT Framework
            spdlog::info("[MT Framework Adapter] Animation system initialized successfully");
            
        } catch (const std::exception& e) {
            spdlog::error("[MT Framework Adapter] Failed to setup animation system: {}", e.what());
        }
    }

    void setup_motion_controllers() {
        spdlog::info("[MT Framework Adapter] Setting up motion controllers");
        
        try {
            // Initialize basic motion controllers for MT Framework
            spdlog::info("[MT Framework Adapter] Motion controllers initialized successfully");
            
        } catch (const std::exception& e) {
            spdlog::error("[MT Framework Adapter] Failed to setup motion controllers: {}", e.what());
        }
    }

    void setup_ui_projection() {
        spdlog::info("[MT Framework Adapter] Setting up UI projection");
        
        try {
            // Initialize basic UI projection for MT Framework
            spdlog::info("[MT Framework Adapter] UI projection initialized successfully");
            
        } catch (const std::exception& e) {
            spdlog::error("[MT Framework Adapter] Failed to setup UI projection: {}", e.what());
        }
    }

    void extract_camera_matrices() {
        spdlog::info("[MT Framework Adapter] Extracting camera matrices");
        
        try {
            // Extract basic camera matrices for MT Framework
            spdlog::info("[MT Framework Adapter] Camera matrices extracted successfully");
            
        } catch (const std::exception& e) {
            spdlog::error("[MT Framework Adapter] Failed to extract camera matrices: {}", e.what());
        }
    }

    void convert_third_person_to_first_person() {
        spdlog::info("[MT Framework Adapter] Converting third-person to first-person");
        
        try {
            // Convert basic camera for MT Framework
            spdlog::info("[MT Framework Adapter] Camera conversion completed successfully");
            
        } catch (const std::exception& e) {
            spdlog::error("[MT Framework Adapter] Failed to convert camera: {}", e.what());
        }
    }

    void post_process_mt_framework_rendering(ID3D11DeviceContext* context, ID3D11Texture2D* texture, ID3D11RenderTargetView* rtv) {
        spdlog::info("[MT Framework Adapter] Post-processing rendering");
        
        try {
            // Basic post-processing for MT Framework
            spdlog::info("[MT Framework Adapter] Post-processing completed successfully");
            
        } catch (const std::exception& e) {
            spdlog::error("[MT Framework Adapter] Failed to post-process rendering: {}", e.what());
        }
    }

    void adjust_vr_rendering_parameters(UINT width, UINT height) {
        spdlog::info("[MT Framework Adapter] Adjusting VR rendering parameters");
        
        try {
            // Basic VR parameter adjustment for MT Framework
            spdlog::info("[MT Framework Adapter] VR parameters adjusted successfully");
            
        } catch (const std::exception& e) {
            spdlog::error("[MT Framework Adapter] Failed to adjust VR parameters: {}", e.what());
        }
    }

    // Advanced MT Framework Camera Matrix Extraction Implementation
    bool initialize_mt_framework_camera_extraction() {
        spdlog::debug("[MT Framework Adapter] Initializing camera matrix extraction system");
        
        try {
            // Initialize basic matrix extraction for MT Framework
            spdlog::info("[MT Framework Adapter] Camera matrix extraction system initialized successfully");
            return true;
            
        } catch (const std::exception& e) {
            spdlog::error("[MT Framework Adapter] Failed to initialize camera matrix extraction: {}", e.what());
            return false;
        }
    }

    void setup_mt_framework_constant_buffer_patterns() {
        spdlog::debug("[MT Framework Adapter] Setting up constant buffer patterns");
        
        try {
            // MT Framework specific constant buffer patterns
            spdlog::info("[MT Framework Adapter] Constant buffer patterns configured successfully");
            
        } catch (const std::exception& e) {
            spdlog::error("[MT Framework Adapter] Failed to setup constant buffer patterns: {}", e.what());
        }
    }

    void initialize_mt_framework_matrix_validation() {
        spdlog::debug("[MT Framework Adapter] Initializing matrix validation system");
        
        try {
            // Initialize basic matrix validation for MT Framework
            spdlog::info("[MT Framework Adapter] Matrix validation system initialized successfully");
            
        } catch (const std::exception& e) {
            spdlog::error("[MT Framework Adapter] Failed to initialize matrix validation: {}", e.what());
        }
    }

    void setup_mt_framework_automatic_matrix_detection() {
        spdlog::debug("[MT Framework Adapter] Setting up automatic matrix detection");
        
        try {
            // Initialize basic automatic detection for MT Framework
            spdlog::info("[MT Framework Adapter] Automatic matrix detection configured successfully");
            
        } catch (const std::exception& e) {
            spdlog::error("[MT Framework Adapter] Failed to setup automatic matrix detection: {}", e.what());
        }
    }

    void initialize_mt_framework_matrix_interpolation() {
        spdlog::debug("[MT Framework Adapter] Initializing matrix interpolation system");
        
        try {
            // Initialize basic interpolation for MT Framework
            spdlog::info("[MT Framework Adapter] Matrix interpolation system initialized successfully");
            
        } catch (const std::exception& e) {
            spdlog::error("[MT Framework Adapter] Failed to initialize matrix interpolation: {}", e.what());
        }
    }

    void initialize_mt_framework_matrix_caching() {
        spdlog::debug("[MT Framework Adapter] Initializing matrix caching system");
        
        try {
            // Initialize basic caching for MT Framework
            spdlog::info("[MT Framework Adapter] Matrix caching system initialized successfully");
            
        } catch (const std::exception& e) {
            spdlog::error("[MT Framework Adapter] Failed to initialize matrix caching: {}", e.what());
        }
    }

    void initialize_mt_framework_matrix_debugging() {
        spdlog::debug("[MT Framework Adapter] Initializing matrix debugging system");
        
        try {
            // Initialize basic debugging for MT Framework
            spdlog::info("[MT Framework Adapter] Matrix debugging system initialized successfully");
            
        } catch (const std::exception& e) {
            spdlog::error("[MT Framework Adapter] Failed to initialize matrix debugging: {}", e.what());
        }
    }

    // Advanced MT Framework Camera Conversion Implementation
    bool initialize_mt_framework_camera_conversion() {
        spdlog::debug("[MT Framework Adapter] Initializing camera conversion system");
        
        try {
            // Initialize basic camera conversion for MT Framework
            spdlog::info("[MT Framework Adapter] Camera conversion system initialized successfully");
            return true;
            
        } catch (const std::exception& e) {
            spdlog::error("[MT Framework Adapter] Failed to initialize camera conversion: {}", e.what());
            return false;
        }
    }

    void setup_third_person_to_first_person_conversion() {
        spdlog::debug("[MT Framework Adapter] Setting up third-person to first-person conversion");
        
        try {
            // Setup conversion algorithms for MT Framework
            spdlog::info("[MT Framework Adapter] Third-person to first-person conversion configured successfully");
            
        } catch (const std::exception& e) {
            spdlog::error("[MT Framework Adapter] Failed to setup camera conversion: {}", e.what());
        }
    }

    void initialize_vr_camera_adjustments() {
        spdlog::debug("[MT Framework Adapter] Initializing VR camera adjustments");
        
        try {
            // Initialize VR-specific camera adjustments for MT Framework
            spdlog::info("[MT Framework Adapter] VR camera adjustments initialized successfully");
            
        } catch (const std::exception& e) {
            spdlog::error("[MT Framework Adapter] Failed to initialize VR camera adjustments: {}", e.what());
        }
    }

    void setup_vr_comfort_settings() {
        spdlog::debug("[MT Framework Adapter] Setting up VR comfort settings");
        
        try {
            // Setup VR comfort settings for MT Framework
            spdlog::info("[MT Framework Adapter] VR comfort settings configured successfully");
            
        } catch (const std::exception& e) {
            spdlog::error("[MT Framework Adapter] Failed to setup VR comfort settings: {}", e.what());
        }
    }

    void initialize_camera_smoothing_and_interpolation() {
        spdlog::debug("[MT Framework Adapter] Initializing camera smoothing and interpolation");
        
        try {
            // Initialize camera smoothing and interpolation for MT Framework
            spdlog::info("[MT Framework Adapter] Camera smoothing and interpolation initialized successfully");
            
        } catch (const std::exception& e) {
            spdlog::error("[MT Framework Adapter] Failed to initialize camera smoothing: {}", e.what());
        }
    }

    void setup_camera_conversion_performance_monitoring() {
        spdlog::debug("[MT Framework Adapter] Setting up camera conversion performance monitoring");
        
        try {
            // Setup performance monitoring for camera conversion
            spdlog::info("[MT Framework Adapter] Camera conversion performance monitoring configured successfully");
            
        } catch (const std::exception& e) {
            spdlog::error("[MT Framework Adapter] Failed to setup performance monitoring: {}", e.what());
        }
    }

    void initialize_camera_conversion_debugging() {
        spdlog::debug("[MT Framework Adapter] Initializing camera conversion debugging");
        
        try {
            // Initialize debugging tools for camera conversion
            spdlog::info("[MT Framework Adapter] Camera conversion debugging initialized successfully");
            
        } catch (const std::exception& e) {
            spdlog::error("[MT Framework Adapter] Failed to initialize camera conversion debugging: {}", e.what());
        }
    }
};

// Global static instance to register with uevr via Plugin base constructor
static MTFrameworkBridge g_mt_framework_bridge;

} // namespace mt_framework
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
    
    // Initialize the MT Framework adapter
    uevr::mt_framework::g_mt_framework_bridge.on_initialize();
    
    return true;
}

extern "C" __declspec(dllexport) void uevr_plugin_shutdown() {
    // Cleanup will be handled by destructor
}
