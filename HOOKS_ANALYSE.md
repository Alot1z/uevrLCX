# ðŸª UEVR Hooks Files Analysis
## Komplet analyse af alle hook filer

**Genereret:** 2025-08-19 12:20:10  
**Antal filer:** 16  
**Status:** âœ… Komplet

---

## ðŸ“ D3D11Hook.cpp
**Sti:** $filePath  
**StÃ¸rrelse:** 10978 bytes  
**Sidst Ã¦ndret:** 2025-08-19 11:20:13

### ðŸ”— Includes
`cpp
#include <algorithm>
`

### ðŸ—ï¸ Classes
`cpp

`

### âš™ï¸ Hook Functions
`cpp
D3D11Hook::~D3D11Hook()
`

### âš™ï¸ General Functions
`cpp
void***)context.Get())
`

### ðŸ“„ Komplet Indhold
`cpp
#include <algorithm>
#include <spdlog/spdlog.h>
#include <utility/Thread.hpp>
#include <utility/Module.hpp>

#include "WindowFilter.hpp"
#include "Framework.hpp"

#include "D3D11Hook.hpp"

using namespace std;

static D3D11Hook* g_d3d11_hook = nullptr;

D3D11Hook::~D3D11Hook() {
    unhook();
}

bool D3D11Hook::hook() {
    spdlog::info("Hooking D3D11");

    g_d3d11_hook = this;

    HWND h_wnd = GetDesktopWindow();
    IDXGISwapChain* swap_chain = nullptr;
    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* context = nullptr;

    D3D_FEATURE_LEVEL feature_level = D3D_FEATURE_LEVEL_11_0;
    DXGI_SWAP_CHAIN_DESC swap_chain_desc;

    ZeroMemory(&swap_chain_desc, sizeof(swap_chain_desc));

    swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swap_chain_desc.BufferCount = 1;
    swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swap_chain_desc.OutputWindow = h_wnd;
    swap_chain_desc.SampleDesc.Count = 1;
    swap_chain_desc.Windowed = TRUE;
    swap_chain_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swap_chain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    const auto original_bytes = utility::get_original_bytes(&D3D11CreateDeviceAndSwapChain);

    // Temporarily unhook D3D11CreateDeviceAndSwapChain
    // it allows compatibility with ReShade and other overlays that hook it
    // this is just a dummy device anyways, we don't want the other overlays to be able to use it
    if (original_bytes) {
        spdlog::info("D3D11CreateDeviceAndSwapChain appears to be hooked, temporarily unhooking");

        std::vector<uint8_t> hooked_bytes(original_bytes->size());
        memcpy(hooked_bytes.data(), &D3D11CreateDeviceAndSwapChain, original_bytes->size());

        ProtectionOverride protection_override{ &D3D11CreateDeviceAndSwapChain, original_bytes->size(), PAGE_EXECUTE_READWRITE };
        memcpy(&D3D11CreateDeviceAndSwapChain, original_bytes->data(), original_bytes->size());
        
        if (FAILED(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_NULL, nullptr, 0, &feature_level, 1, D3D11_SDK_VERSION,
                &swap_chain_desc, &swap_chain, &device, nullptr, &context))) 
        {
            spdlog::error("Failed to create D3D11 device");
            memcpy(&D3D11CreateDeviceAndSwapChain, hooked_bytes.data(), hooked_bytes.size());
            return false;
        }
        
        spdlog::info("Restoring hooked bytes for D3D11CreateDeviceAndSwapChain");
        memcpy(&D3D11CreateDeviceAndSwapChain, hooked_bytes.data(), hooked_bytes.size());
    } else {
        if (FAILED(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_NULL, nullptr, 0, &feature_level, 1, D3D11_SDK_VERSION,
                &swap_chain_desc, &swap_chain, &device, nullptr, &context))) 
        {
            spdlog::error("Failed to create D3D11 device");
            return false;
        }
    }

    try {
        m_present_hook.reset();
        m_resize_buffers_hook.reset();

        auto& present_fn = (*(void***)swap_chain)[8];
        auto& resize_buffers_fn = (*(void***)swap_chain)[13];

        m_present_hook = std::make_unique<PointerHook>(&present_fn, (void*)&D3D11Hook::present);
        m_resize_buffers_hook = std::make_unique<PointerHook>(&resize_buffers_fn, (void*)&D3D11Hook::resize_buffers);

        m_hooked = true;
    } catch (const std::exception& e) {
        spdlog::error("Failed to hook D3D11: {}", e.what());
        m_hooked = false;
    }

    device->Release();
    context->Release();
    swap_chain->Release();
    return m_hooked;
}

bool D3D11Hook::unhook() {
    if (!m_hooked) {
        return true;
    }

    spdlog::info("Unhooking D3D11");

    if (m_present_hook->remove() && m_resize_buffers_hook->remove()) {
        m_hooked = false;
        return true;
    }

    return false;
}

thread_local bool g_inside_d3d11_present = false;
HRESULT last_d3d11_present_result = S_OK;

HRESULT WINAPI D3D11Hook::present(IDXGISwapChain* swap_chain, UINT sync_interval, UINT flags) {
    std::scoped_lock _{g_framework->get_hook_monitor_mutex()};

    auto d3d11 = g_d3d11_hook;

    // This line must be called before calling our detour function because we might have to unhook the function inside our detour.
    auto present_fn = d3d11->m_present_hook->get_original<decltype(D3D11Hook::present)*>();

    DXGI_SWAP_CHAIN_DESC swap_desc{};
    swap_chain->GetDesc(&swap_desc);

    if (WindowFilter::get().is_filtered(swap_desc.OutputWindow)) {
        return present_fn(swap_chain, sync_interval, flags);
    }

    d3d11->m_inside_present = true;

    if (d3d11->m_swapchain_0 == nullptr) {
        d3d11->m_swapchain_0 = swap_chain;
        d3d11->m_swap_chain = swap_chain;
    } else if (d3d11->m_swapchain_1 == nullptr && swap_chain != d3d11->m_swapchain_0) {
        d3d11->m_swapchain_1 = swap_chain;
    }

    /*if (d3d11->m_swap_chain != d3d11->m_swapchain_0) {
        d3d11->m_inside_present = false;
        return present_fn(swap_chain, sync_interval, flags);
    }*/

    swap_chain->GetDevice(__uuidof(d3d11->m_device), (void**)&d3d11->m_device);

    /*if (d3d11->m_set_render_targets_hook == nullptr) {
        ComPtr<ID3D11DeviceContext> context{};

        d3d11->m_device->GetImmediateContext(&context);
        auto& set_render_targets_fn = (*(void***)context.Get())[33];
        d3d11->m_set_render_targets_hook = std::make_unique<PointerHook>(&set_render_targets_fn, (void*)&set_render_targets);
        OutputDebugString("Hooked ID3D11DeviceContext::SetRenderTargets");
    }*/

    /*if (GetAsyncKeyState(VK_INSERT) & 1) {
        OutputDebugString(fmt::format("Depth stencil @ {:p} used", (void*)d3d11->m_last_depthstencil_used.Get()).c_str());
    }*/

    // Restore the original bytes
    // if an infinite loop occurs, this will prevent the game from crashing
    // while keeping our hook intact
    if (g_inside_d3d11_present) {
        auto original_bytes = utility::get_original_bytes(Address{present_fn});

        if (original_bytes) {
            ProtectionOverride protection_override{present_fn, original_bytes->size(), PAGE_EXECUTE_READWRITE};

            memcpy(present_fn, original_bytes->data(), original_bytes->size());

            spdlog::info("Present fixed");
        }

        return last_d3d11_present_result;
    }

    if (d3d11->m_on_present) {
        d3d11->m_on_present(*d3d11);

        if (d3d11->m_next_present_interval) {
            sync_interval = *d3d11->m_next_present_interval;
            d3d11->m_next_present_interval = std::nullopt;

            if (sync_interval == 0) {
                BOOL is_fullscreen = 0;
                swap_chain->GetFullscreenState(&is_fullscreen, nullptr);
                flags &= ~DXGI_PRESENT_DO_NOT_SEQUENCE;

                if (!is_fullscreen && (swap_desc.Flags & DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING) != 0) {
                    flags |= DXGI_PRESENT_ALLOW_TEARING;
                }
            }
        }
    }

    HRESULT result = S_OK;
    g_inside_d3d11_present = true;

    if (!d3d11->m_ignore_next_present) {
        result = present_fn(swap_chain, sync_interval, flags);
        last_d3d11_present_result = result;
    } else {
        d3d11->m_ignore_next_present = false;
        last_d3d11_present_result = S_OK;
    }

    g_inside_d3d11_present = false;

    if (d3d11->m_on_post_present) {
        d3d11->m_on_post_present(*d3d11);
    }

    d3d11->m_last_depthstencil_used.Reset();
    d3d11->m_inside_present = false;

    return result;
}

thread_local bool g_inside_d3d11_resize_buffers = false;
HRESULT last_d3d11_resize_buffers_result = S_OK;

HRESULT WINAPI D3D11Hook::resize_buffers(
    IDXGISwapChain* swap_chain, UINT buffer_count, UINT width, UINT height, DXGI_FORMAT new_format, UINT swap_chain_flags) {
    std::scoped_lock _{g_framework->get_hook_monitor_mutex()};

    auto d3d11 = g_d3d11_hook;
    auto resize_buffers_fn = d3d11->m_resize_buffers_hook->get_original<decltype(D3D11Hook::resize_buffers)*>();

    DXGI_SWAP_CHAIN_DESC swap_desc{};
    swap_chain->GetDesc(&swap_desc);

    if (WindowFilter::get().is_filtered(swap_desc.OutputWindow)) {
        return resize_buffers_fn(swap_chain, buffer_count, width, height, new_format, swap_chain_flags);
    }

    d3d11->m_swap_chain = swap_chain;
    d3d11->m_swapchain_0 = nullptr;
    d3d11->m_swapchain_1 = nullptr;
    d3d11->m_last_depthstencil_used.Reset();

    if (d3d11->m_on_resize_buffers) {
        d3d11->m_on_resize_buffers(*d3d11, width, height);
    }

    if (g_inside_d3d11_resize_buffers) {
        auto original_bytes = utility::get_original_bytes(Address{resize_buffers_fn});

        if (original_bytes) {
            ProtectionOverride protection_override{resize_buffers_fn, original_bytes->size(), PAGE_EXECUTE_READWRITE};

            memcpy(resize_buffers_fn, original_bytes->data(), original_bytes->size());

            spdlog::info("Resize buffers fixed");
        }

        return last_d3d11_resize_buffers_result;
    }

    g_inside_d3d11_resize_buffers = true;

    last_d3d11_resize_buffers_result = resize_buffers_fn(swap_chain, buffer_count, width, height, new_format, swap_chain_flags);

    g_inside_d3d11_resize_buffers = false;

    return last_d3d11_resize_buffers_result;
}

void WINAPI D3D11Hook::set_render_targets(
    ID3D11DeviceContext* context, UINT num_views, ID3D11RenderTargetView* const* rtvs, ID3D11DepthStencilView* dsv) {
    std::scoped_lock _{g_framework->get_hook_monitor_mutex()};

    auto d3d11 = g_d3d11_hook;

    if (dsv != nullptr) {
        //auto obj_name = fmt::format("Depthstencil @ {:p}", (void*)d3d11->m_last_depthstencil_used.Get());
        //d3d11->m_last_depthstencil_used->SetPrivateData(WKPDID_D3DDebugObjectName, obj_name.size(), obj_name.c_str());
        //OutputDebugString(fmt::format("Depth stencil @ {:p} used", (void*)d3d11->m_last_depthstencil_used.Get()).c_str());

        D3D11_DEPTH_STENCIL_VIEW_DESC desc{};
        dsv->GetDesc(&desc);

        if (desc.Flags & D3D11_DSV_FLAG::D3D11_DSV_READ_ONLY_DEPTH) {
            dsv->GetResource((ID3D11Resource**)d3d11->m_last_depthstencil_used.GetAddressOf());

            //OutputDebugString(fmt::format("Flags: {}", desc.Flags).c_str());
            //OutputDebugString(fmt::format("Format: {}", desc.Format).c_str());
            //OutputDebugString(fmt::format("ViewDimension: {}", desc.ViewDimension).c_str());   
        }
    }

    auto set_render_targets_fn = d3d11->m_set_render_targets_hook->get_original<decltype(set_render_targets)*>();

    return set_render_targets_fn(context, num_views, rtvs, dsv);
}

`

---
## ðŸ“ D3D11Hook.hpp
**Sti:** $filePath  
**StÃ¸rrelse:** 2488 bytes  
**Sidst Ã¦ndret:** 2025-08-19 11:20:13

### ðŸ”— Includes
`cpp
#include <functional>
`

### ðŸ—ï¸ Classes
`cpp
class D3D11Hook {
`

### âš™ï¸ Hook Functions
`cpp
D3D11Hook()
`

### âš™ï¸ General Functions
`cpp
void(D3D11Hook&)
`

### ðŸ“„ Komplet Indhold
`cpp
#pragma once

#include <functional>

#include <d3d11.h>
#include <dxgi.h>
#include <wrl.h>

#include "utility/PointerHook.hpp"

class D3D11Hook {
public:
    typedef std::function<void(D3D11Hook&)> OnPresentFn;
    typedef std::function<void(D3D11Hook&, uint32_t w, uint32_t h)> OnResizeBuffersFn;

    D3D11Hook() = default;
    virtual ~D3D11Hook();

	bool is_hooked() {
		return m_hooked;
	}

    bool is_inside_present() const {
        return m_inside_present;
    }

    void ignore_next_present() {
        m_ignore_next_present = true;
    }

    void set_next_present_interval(uint32_t interval) {
        m_next_present_interval = interval;
    }

    bool hook();
    bool unhook();

    void on_present(OnPresentFn fn) { m_on_present = fn; }
    void on_post_present(OnPresentFn fn) { m_on_post_present = fn; }
    void on_resize_buffers(OnResizeBuffersFn fn) { m_on_resize_buffers = fn; }

    ID3D11Device* get_device() { return m_device; }
    IDXGISwapChain* get_swap_chain() { return m_swap_chain; } // The "active" swap chain.
    auto get_swapchain_0() { return m_swapchain_0; }
    auto get_swapchain_1() { return m_swapchain_1; }
    auto& get_last_depthstencil_used() { return m_last_depthstencil_used; }

protected:
    template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

    ID3D11Device* m_device{ nullptr };
    IDXGISwapChain* m_swap_chain{ nullptr };
    IDXGISwapChain* m_swapchain_0{};
    IDXGISwapChain* m_swapchain_1{};
    bool m_hooked{ false };
    bool m_inside_present{false};
    bool m_ignore_next_present{false};

    std::optional<uint32_t> m_next_present_interval{};

    std::unique_ptr<PointerHook> m_present_hook{};
    std::unique_ptr<PointerHook> m_resize_buffers_hook{};
    std::unique_ptr<PointerHook> m_set_render_targets_hook{};
    OnPresentFn m_on_present{ nullptr };
    OnPresentFn m_on_post_present{ nullptr };
    OnResizeBuffersFn m_on_resize_buffers{ nullptr };
    ComPtr<ID3D11Texture2D> m_last_depthstencil_used{};

    static HRESULT WINAPI present(IDXGISwapChain* swap_chain, UINT sync_interval, UINT flags);
    static HRESULT WINAPI resize_buffers(IDXGISwapChain* swap_chain, UINT buffer_count, UINT width, UINT height, DXGI_FORMAT new_format, UINT swap_chain_flags);
    static void WINAPI set_render_targets(
        ID3D11DeviceContext* context, UINT num_views, ID3D11RenderTargetView* const* rtvs, ID3D11DepthStencilView* dsv);
};

`

---
## ðŸ“ D3D12Hook.cpp
**Sti:** $filePath  
**StÃ¸rrelse:** 25986 bytes  
**Sidst Ã¦ndret:** 2025-08-19 11:20:13

### ðŸ”— Includes
`cpp
#include <thread>
`

### ðŸ—ï¸ Classes
`cpp
CLASSEX wc{
`

### âš™ï¸ Hook Functions
`cpp
D3D12Hook::~D3D12Hook()
`

### âš™ï¸ General Functions
`cpp
void*); i += sizeof(void*))
`

### ðŸ“„ Komplet Indhold
`cpp
#include <thread>
#include <future>
#include <unordered_set>

#include <spdlog/spdlog.h>
#include <utility/Thread.hpp>
#include <utility/Module.hpp>
#include <utility/RTTI.hpp>

#include "WindowFilter.hpp"
#include "Framework.hpp"

#include "D3D12Hook.hpp"

static D3D12Hook* g_d3d12_hook = nullptr;

D3D12Hook::~D3D12Hook() {
    unhook();
}

bool D3D12Hook::hook() {
    spdlog::info("Hooking D3D12");

    g_d3d12_hook = this;

    IDXGISwapChain1* swap_chain1{ nullptr };
    IDXGISwapChain3* swap_chain{ nullptr };
    ID3D12Device* device{ nullptr };

    D3D_FEATURE_LEVEL feature_level = D3D_FEATURE_LEVEL_11_0;
    DXGI_SWAP_CHAIN_DESC1 swap_chain_desc1;

    ZeroMemory(&swap_chain_desc1, sizeof(swap_chain_desc1));

    swap_chain_desc1.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    swap_chain_desc1.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swap_chain_desc1.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    swap_chain_desc1.BufferCount = 2;
    swap_chain_desc1.SampleDesc.Count = 1;
    swap_chain_desc1.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;
    swap_chain_desc1.Width = 1;
    swap_chain_desc1.Height = 1;

    // Manually get D3D12CreateDevice export because the user may be running Windows 7
    const auto d3d12_module = LoadLibraryA("d3d12.dll");
    if (d3d12_module == nullptr) {
        spdlog::error("Failed to load d3d12.dll");
        return false;
    }

    auto d3d12_create_device = (decltype(D3D12CreateDevice)*)GetProcAddress(d3d12_module, "D3D12CreateDevice");
    if (d3d12_create_device == nullptr) {
        spdlog::error("Failed to get D3D12CreateDevice export");
        return false;
    }

    spdlog::info("Creating dummy device");

    // Get the original on-disk bytes of the D3D12CreateDevice export
    const auto original_bytes = utility::get_original_bytes(d3d12_create_device);

    // Temporarily unhook D3D12CreateDevice
    // it allows compatibility with ReShade and other overlays that hook it
    // this is just a dummy device anyways, we don't want the other overlays to be able to use it
    if (original_bytes) {
        spdlog::info("D3D12CreateDevice appears to be hooked, temporarily unhooking");

        std::vector<uint8_t> hooked_bytes(original_bytes->size());
        memcpy(hooked_bytes.data(), d3d12_create_device, original_bytes->size());

        ProtectionOverride protection_override{ d3d12_create_device, original_bytes->size(), PAGE_EXECUTE_READWRITE };
        memcpy(d3d12_create_device, original_bytes->data(), original_bytes->size());
        
        if (FAILED(d3d12_create_device(nullptr, feature_level, IID_PPV_ARGS(&device)))) {
            spdlog::error("Failed to create D3D12 Dummy device");
            memcpy(d3d12_create_device, hooked_bytes.data(), hooked_bytes.size());
            return false;
        }

        spdlog::info("Restoring hooked bytes for D3D12CreateDevice");
        memcpy(d3d12_create_device, hooked_bytes.data(), hooked_bytes.size());
    } else { // D3D12CreateDevice is not hooked
        if (FAILED(d3d12_create_device(nullptr, feature_level, IID_PPV_ARGS(&device)))) {
            spdlog::error("Failed to create D3D12 Dummy device");
            return false;
        }
    }

    spdlog::info("Dummy device: {:x}", (uintptr_t)device);

    // Manually get CreateDXGIFactory export because the user may be running Windows 7
    const auto dxgi_module = LoadLibraryA("dxgi.dll");
    if (dxgi_module == nullptr) {
        spdlog::error("Failed to load dxgi.dll");
        return false;
    }

    auto create_dxgi_factory = (decltype(CreateDXGIFactory)*)GetProcAddress(dxgi_module, "CreateDXGIFactory");

    if (create_dxgi_factory == nullptr) {
        spdlog::error("Failed to get CreateDXGIFactory export");
        return false;
    }

    spdlog::info("Creating dummy DXGI factory");

    IDXGIFactory4* factory{ nullptr };
    if (FAILED(create_dxgi_factory(IID_PPV_ARGS(&factory)))) {
        spdlog::error("Failed to create D3D12 Dummy DXGI Factory");
        return false;
    }

    D3D12_COMMAND_QUEUE_DESC queue_desc{};
    queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queue_desc.Priority = 0;
    queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queue_desc.NodeMask = 0;

    spdlog::info("Creating dummy command queue");

    ID3D12CommandQueue* command_queue{ nullptr };
    if (FAILED(device->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(&command_queue)))) {
        spdlog::error("Failed to create D3D12 Dummy Command Queue");
        return false;
    }

    spdlog::info("Creating dummy swapchain");

    // used in CreateSwapChainForHwnd fallback
    HWND hwnd = 0;
    WNDCLASSEX wc{};

    auto init_dummy_window = [&]() {
        // fallback to CreateSwapChainForHwnd
        wc.cbSize = sizeof(WNDCLASSEX);
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = DefWindowProc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = GetModuleHandle(NULL);
        wc.hIcon = NULL;
        wc.hCursor = NULL;
        wc.hbrBackground = NULL;
        wc.lpszMenuName = NULL;
        wc.lpszClassName = TEXT("REFRAMEWORK_DX12_DUMMY");
        wc.hIconSm = NULL;

        ::RegisterClassEx(&wc);

        hwnd = ::CreateWindow(wc.lpszClassName, TEXT("REF DX Dummy Window"), WS_OVERLAPPEDWINDOW, 0, 0, 100, 100, NULL, NULL, wc.hInstance, NULL);

        swap_chain_desc1.BufferCount = 3;
        swap_chain_desc1.Width = 0;
        swap_chain_desc1.Height = 0;
        swap_chain_desc1.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swap_chain_desc1.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
        swap_chain_desc1.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swap_chain_desc1.SampleDesc.Count = 1;
        swap_chain_desc1.SampleDesc.Quality = 0;
        swap_chain_desc1.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swap_chain_desc1.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
        swap_chain_desc1.Scaling = DXGI_SCALING_STRETCH;
        swap_chain_desc1.Stereo = FALSE;
    };

    std::vector<std::function<bool ()>> swapchain_attempts{
        // we call CreateSwapChainForComposition instead of CreateSwapChainForHwnd
        // because some overlays will have hooks on CreateSwapChainForHwnd
        // and all we're doing is creating a dummy swapchain
        // we don't want to screw up the overlay
        [&]() {
            return !FAILED(factory->CreateSwapChainForComposition(command_queue, &swap_chain_desc1, nullptr, &swap_chain1));
        },
        [&]() {
            init_dummy_window();

            return !FAILED(factory->CreateSwapChainForHwnd(command_queue, hwnd, &swap_chain_desc1, nullptr, nullptr, &swap_chain1));
        },
        [&]() {
            return !FAILED(factory->CreateSwapChainForHwnd(command_queue, GetDesktopWindow(), &swap_chain_desc1, nullptr, nullptr, &swap_chain1));
        },
    };

    bool any_succeed = false;

    for (auto i = 0; i < swapchain_attempts.size(); i++) {
        auto& attempt = swapchain_attempts[i];
        
        try {
            spdlog::info("Trying swapchain attempt {}", i);

            if (attempt()) {
                spdlog::info("Created dummy swapchain on attempt {}", i);
                any_succeed = true;
                break;
            }
        } catch (std::exception& e) {
            spdlog::error("Failed to create dummy swapchain on attempt {}: {}", i, e.what());
        } catch(...) {
            spdlog::error("Failed to create dummy swapchain on attempt {}: unknown exception", i);
        }

        spdlog::error("Attempt {} failed", i);
    }

    if (!any_succeed) {
        spdlog::error("Failed to create D3D12 Dummy Swap Chain");

        if (hwnd) {
            ::DestroyWindow(hwnd);
        }

        if (wc.lpszClassName != nullptr) {
            ::UnregisterClass(wc.lpszClassName, wc.hInstance);
        }

        return false;
    }

    spdlog::info("Querying dummy swapchain");

    if (FAILED(swap_chain1->QueryInterface(IID_PPV_ARGS(&swap_chain)))) {
        spdlog::error("Failed to retrieve D3D12 DXGI SwapChain");
        return false;
    }

    try {
        const auto ti = utility::rtti::get_type_info(swap_chain1);
        const auto swapchain_classname = ti != nullptr && ti->name() != nullptr ? std::string_view{ti->name()} : "unknown";
        const auto raw_name = ti != nullptr && ti->raw_name() != nullptr ? std::string_view{ti->raw_name()} : "unknown";

        spdlog::info("Swapchain type info: {}", swapchain_classname);
        spdlog::info("Swapchain raw type info: {}", raw_name);
        
        if (swapchain_classname.contains("interposer::DXGISwapChain")) { // DLSS3
            spdlog::info("Found Streamline (DLSSFG) swapchain during dummy initialization: {:x}", (uintptr_t)swap_chain1);
            m_using_frame_generation_swapchain = true;
        }
        // Need to test this one to see if it actually has the same issues - disabling it for now
        /*else if (swapchain_classname.contains("FrameInterpolationSwapChain")) { // FSR3
            spdlog::info("Found FSR3 swapchain during dummy initialization: {:x}", (uintptr_t)swap_chain1);
            m_using_frame_generation_swapchain = true;
        }*/
    } catch (const std::exception& e) {
        spdlog::error("Failed to get type info: {}", e.what());
    } catch (...) {
        spdlog::error("Failed to get type info: unknown exception");
    }

    spdlog::info("Finding command queue offset");

    m_command_queue_offset = 0;

    // Find the command queue offset in the swapchain
    for (auto i = 0; i < 512 * sizeof(void*); i += sizeof(void*)) {
        const auto base = (uintptr_t)swap_chain1 + i;

        // reached the end
        if (IsBadReadPtr((void*)base, sizeof(void*))) {
            break;
        }

        auto data = *(ID3D12CommandQueue**)base;

        if (data == command_queue) {
            m_command_queue_offset = i;
            spdlog::info("Found command queue offset: {:x}", i);
            break;
        }
    }

    auto target_swapchain = swap_chain;

    // Scan throughout the swapchain for a valid pointer to scan through
    // this is usually only necessary for Proton
    if (m_command_queue_offset == 0) {
        bool should_break = false;

        for (auto base = 0; base < 512 * sizeof(void*); base += sizeof(void*)) {
            const auto pre_scan_base = (uintptr_t)swap_chain1 + base;

            // reached the end
            if (IsBadReadPtr((void*)pre_scan_base, sizeof(void*))) {
                break;
            }

            const auto scan_base = *(uintptr_t*)pre_scan_base;

            if (scan_base == 0 || IsBadReadPtr((void*)scan_base, sizeof(void*))) {
                continue;
            }

            for (auto i = 0; i < 512 * sizeof(void*); i += sizeof(void*)) {
                const auto pre_data = scan_base + i;

                if (IsBadReadPtr((void*)pre_data, sizeof(void*))) {
                    break;
                }

                auto data = *(ID3D12CommandQueue**)pre_data;

                if (data == command_queue) {
                    // If we hook Streamline's Swapchain, the menu fails to render correctly/flickers
                    // So we switch out the swapchain with the internal one owned by Streamline
                    // Side note: Even though we are scanning for Proton here,
                    // this doubles as an offset scanner for the real swapchain inside Streamline (or FSR3)
                    if (m_using_frame_generation_swapchain) {
                        target_swapchain = (IDXGISwapChain3*)scan_base;
                    }

                    if (!m_using_frame_generation_swapchain) {
                        m_using_proton_swapchain = true;
                    }

                    m_command_queue_offset = i;
                    m_proton_swapchain_offset = base;
                    should_break = true;

                    spdlog::info("Proton potentially detected");
                    spdlog::info("Found command queue offset: {:x}", i);
                    break;
                }
            }

            if (m_using_proton_swapchain || should_break) {
                break;
            }
        }
    }

    if (m_command_queue_offset == 0) {
        spdlog::error("Failed to find command queue offset");
        return false;
    }

    try {
        spdlog::info("Initializing hooks");
        m_present_hook.reset();
        m_present1_hook.reset();
        m_swapchain_hook.reset();

        m_is_phase_1 = true;

        auto& present_fn = (*(void***)target_swapchain)[8]; // Present
        auto& present1_fn = (*(void***)target_swapchain)[22]; // Present1
        m_present_hook = std::make_unique<PointerHook>(&present_fn, (void*)&D3D12Hook::present);
        m_present1_hook = std::make_unique<PointerHook>(&present1_fn, (void*)&D3D12Hook::present1);
        m_hooked = true;
    } catch (const std::exception& e) {
        spdlog::error("Failed to initialize hooks: {}", e.what());
        m_hooked = false;
    }

    device->Release();
    command_queue->Release();
    factory->Release();
    swap_chain1->Release();
    swap_chain->Release();

    if (hwnd) {
        ::DestroyWindow(hwnd);
    }

    if (wc.lpszClassName != nullptr) {
        ::UnregisterClass(wc.lpszClassName, wc.hInstance);
    }

    return m_hooked;
}

bool D3D12Hook::unhook() {
    if (!m_hooked) {
        return true;
    }

    spdlog::info("Unhooking D3D12");

    m_present_hook.reset();
    m_present1_hook.reset();
    m_swapchain_hook.reset();

    m_hooked = false;
    m_is_phase_1 = true;

    return true;
}

thread_local int32_t g_present_depth = 0;

HRESULT D3D12Hook::present_internal(IDXGISwapChain3* swap_chain, UINT sync_interval, UINT flags, DXGI_PRESENT_PARAMETERS* params, bool present1) {
    auto d3d12 = g_d3d12_hook;

    HWND swapchain_wnd{nullptr};
    swap_chain->GetHwnd(&swapchain_wnd);

    using Present1Fn = HRESULT(*)(IDXGISwapChain3*, UINT, UINT, DXGI_PRESENT_PARAMETERS*);
    Present1Fn present_fn{nullptr};

    if (!present1) {
        present_fn = d3d12->m_present_hook->get_original<Present1Fn>();
    } else {
        present_fn = d3d12->m_present1_hook->get_original<Present1Fn>();
    }

    if (d3d12->m_is_phase_1 && WindowFilter::get().is_filtered(swapchain_wnd)) {
        return present_fn(swap_chain, sync_interval, flags, params);
    }

    if (!d3d12->m_is_phase_1 && swap_chain != d3d12->m_swapchain_hook->get_instance()) {
        const auto og_instance = d3d12->m_swapchain_hook->get_instance();

        // If the original swapchain instance is invalid, then we should not proceed, and rehook the swapchain
        if (IsBadReadPtr(og_instance, sizeof(void*)) || IsBadReadPtr(og_instance.deref(), sizeof(void*))) {
            spdlog::error("Bad read pointer for original swapchain instance, re-hooking");
            d3d12->m_is_phase_1 = true;
        }

        if (!d3d12->m_is_phase_1) {
            return present_fn(swap_chain, sync_interval, flags, params);
        }
    }

    if (d3d12->m_is_phase_1) {
        //d3d12->m_present_hook.reset();
        d3d12->m_swapchain_hook.reset();

        // vtable hook the swapchain instead of global hooking
        // this seems safer for whatever reason
        // if we globally hook the vtable pointers, it causes all sorts of weird conflicts with other hooks
        // dont hook present though via this hook so other hooks dont get confused
        d3d12->m_swapchain_hook = std::make_unique<VtableHook>(swap_chain);
        //d3d12->m_swapchain_hook->hook_method(8, (uintptr_t)&D3D12Hook::present);
        d3d12->m_swapchain_hook->hook_method(13, (uintptr_t)&D3D12Hook::resize_buffers);
        d3d12->m_swapchain_hook->hook_method(14, (uintptr_t)&D3D12Hook::resize_target);
        d3d12->m_is_phase_1 = false;
    }

    d3d12->m_inside_present = true;
    d3d12->m_swap_chain = swap_chain;

    swap_chain->GetDevice(IID_PPV_ARGS(&d3d12->m_device));

    if (d3d12->m_device != nullptr) {
        if (d3d12->m_using_proton_swapchain) {
            const auto real_swapchain = *(uintptr_t*)((uintptr_t)swap_chain + d3d12->m_proton_swapchain_offset);
            d3d12->m_command_queue = *(ID3D12CommandQueue**)(real_swapchain + d3d12->m_command_queue_offset);
        } else {
            d3d12->m_command_queue = *(ID3D12CommandQueue**)((uintptr_t)swap_chain + d3d12->m_command_queue_offset);
        }
    }

    if (d3d12->m_swapchain_0 == nullptr) {
        d3d12->m_swapchain_0 = swap_chain;
    } else if (d3d12->m_swapchain_1 == nullptr && swap_chain != d3d12->m_swapchain_0) {
        d3d12->m_swapchain_1 = swap_chain;
    }
    
    // Restore the original bytes
    // if an infinite loop occurs, this will prevent the game from crashing
    // while keeping our hook intact
    if (g_present_depth > 0) {
        auto original_bytes = utility::get_original_bytes(Address{present_fn});

        if (original_bytes) {
            ProtectionOverride protection_override{present_fn, original_bytes->size(), PAGE_EXECUTE_READWRITE};

            memcpy(present_fn, original_bytes->data(), original_bytes->size());

            spdlog::info("Present fixed");
        }

        if ((uintptr_t)present_fn != (uintptr_t)D3D12Hook::present && g_present_depth == 1) {
            spdlog::info("Attempting to call real present function");

            ++g_present_depth;
            const auto result = present_fn(swap_chain, sync_interval, flags, params);
            --g_present_depth;

            if (result != S_OK) {
                spdlog::error("Present failed: {:x}", result);
            }

            return result;
        }

        spdlog::info("Just returning S_OK");
        return S_OK;
    }

    if (d3d12->m_on_present) {
        d3d12->m_on_present(*d3d12);

        if (d3d12->m_next_present_interval) {
            sync_interval = *d3d12->m_next_present_interval;
            d3d12->m_next_present_interval = std::nullopt;

            if (sync_interval == 0) {
                BOOL is_fullscreen = 0;
                swap_chain->GetFullscreenState(&is_fullscreen, nullptr);
                flags &= ~DXGI_PRESENT_DO_NOT_SEQUENCE;

                DXGI_SWAP_CHAIN_DESC swap_desc{};
                swap_chain->GetDesc(&swap_desc);

                if (!is_fullscreen && (swap_desc.Flags & DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING) != 0) {
                    flags |= DXGI_PRESENT_ALLOW_TEARING;
                }
            }
        }
    }

    ++g_present_depth;

    auto result = S_OK;
    
    if (!d3d12->m_ignore_next_present) {
        result = present_fn(swap_chain, sync_interval, flags, params);

        if (result != S_OK) {
            spdlog::error("Present failed: {:x}", result);
        }
    } else {
        d3d12->m_ignore_next_present = false;
    }

    --g_present_depth;

    if (d3d12->m_on_post_present) {
        d3d12->m_on_post_present(*d3d12);
    }

    d3d12->m_inside_present = false;

    return result;
}

HRESULT WINAPI D3D12Hook::present(IDXGISwapChain3* swap_chain, UINT sync_interval, UINT flags) {
    std::scoped_lock _{g_framework->get_hook_monitor_mutex()};
    
    return D3D12Hook::present_internal(swap_chain, sync_interval, flags, nullptr, false);
}

HRESULT WINAPI D3D12Hook::present1(IDXGISwapChain3* swap_chain, UINT sync_interval, UINT flags, DXGI_PRESENT_PARAMETERS* params) {
    std::scoped_lock _{g_framework->get_hook_monitor_mutex()};

    return D3D12Hook::present_internal(swap_chain, sync_interval, flags, params, true);
}

thread_local int32_t g_resize_buffers_depth = 0;

HRESULT WINAPI D3D12Hook::resize_buffers(IDXGISwapChain3* swap_chain, UINT buffer_count, UINT width, UINT height, DXGI_FORMAT new_format, UINT swap_chain_flags) {
    std::scoped_lock _{g_framework->get_hook_monitor_mutex()};

    spdlog::info("D3D12 resize buffers called");
    spdlog::info(" Parameters: buffer_count {} width {} height {} new_format {} swap_chain_flags {}", buffer_count, width, height, new_format, swap_chain_flags);

    auto d3d12 = g_d3d12_hook;
    //auto& hook = d3d12->m_resize_buffers_hook;
    //auto resize_buffers_fn = hook->get_original<decltype(D3D12Hook::resize_buffers)*>();

    HWND swapchain_wnd{nullptr};
    swap_chain->GetHwnd(&swapchain_wnd);

    auto resize_buffers_fn = d3d12->m_swapchain_hook->get_method<decltype(D3D12Hook::resize_buffers)*>(13);

    if (WindowFilter::get().is_filtered(swapchain_wnd)) {
        return resize_buffers_fn(swap_chain, buffer_count, width, height, new_format, swap_chain_flags);
    }

    d3d12->m_display_width = width;
    d3d12->m_display_height = height;

    if (g_resize_buffers_depth > 0) {
        auto original_bytes = utility::get_original_bytes(Address{resize_buffers_fn});

        if (original_bytes) {
            ProtectionOverride protection_override{resize_buffers_fn, original_bytes->size(), PAGE_EXECUTE_READWRITE};

            memcpy(resize_buffers_fn, original_bytes->data(), original_bytes->size());

            spdlog::info("Resize buffers fixed");
        }

        if ((uintptr_t)resize_buffers_fn != (uintptr_t)&D3D12Hook::resize_buffers && g_resize_buffers_depth == 1) {
            spdlog::info("Attempting to call the real resize buffers function");

            ++g_resize_buffers_depth;
            const auto result = resize_buffers_fn(swap_chain, buffer_count, width, height, new_format, swap_chain_flags);
            --g_resize_buffers_depth;

            if (result != S_OK) {
                spdlog::error("Resize buffers failed: {:x}", result);
            }

            return result;
        } else {
            spdlog::info("Just returning S_OK");
            return S_OK;
        }
    }

    if (d3d12->m_on_resize_buffers) {
        d3d12->m_on_resize_buffers(*d3d12, width, height);
    }

    ++g_resize_buffers_depth;

    const auto result = resize_buffers_fn(swap_chain, buffer_count, width, height, new_format, swap_chain_flags);
    
    if (result != S_OK) {
        spdlog::error("Resize buffers failed: {:x}", result);
    }

    --g_resize_buffers_depth;

    return result;
}

thread_local int32_t g_resize_target_depth = 0;

HRESULT WINAPI D3D12Hook::resize_target(IDXGISwapChain3* swap_chain, const DXGI_MODE_DESC* new_target_parameters) {
    std::scoped_lock _{g_framework->get_hook_monitor_mutex()};

    spdlog::info("D3D12 resize target called");
    spdlog::info(" Parameters: new_target_parameters {:x}", (uintptr_t)new_target_parameters);

    auto d3d12 = g_d3d12_hook;
    //auto resize_target_fn = d3d12->m_resize_target_hook->get_original<decltype(D3D12Hook::resize_target)*>();

    HWND swapchain_wnd{nullptr};
    swap_chain->GetHwnd(&swapchain_wnd);

    auto resize_target_fn = d3d12->m_swapchain_hook->get_method<decltype(D3D12Hook::resize_target)*>(14);

    if (WindowFilter::get().is_filtered(swapchain_wnd)) {
        return resize_target_fn(swap_chain, new_target_parameters);
    }

    d3d12->m_render_width = new_target_parameters->Width;
    d3d12->m_render_height = new_target_parameters->Height;

    // Restore the original code to the resize_buffers function.
    if (g_resize_target_depth > 0) {
        auto original_bytes = utility::get_original_bytes(Address{resize_target_fn});

        if (original_bytes) {
            ProtectionOverride protection_override{resize_target_fn, original_bytes->size(), PAGE_EXECUTE_READWRITE};

            memcpy(resize_target_fn, original_bytes->data(), original_bytes->size());

            spdlog::info("Resize target fixed");
        }

        if ((uintptr_t)resize_target_fn != (uintptr_t)&D3D12Hook::resize_target && g_resize_target_depth == 1) {
            spdlog::info("Attempting to call the real resize target function");

            ++g_resize_target_depth;
            const auto result = resize_target_fn(swap_chain, new_target_parameters);
            --g_resize_target_depth;

            if (result != S_OK) {
                spdlog::error("Resize target failed: {:x}", result);
            }

            return result;
        } else {
            spdlog::info("Just returning S_OK");
            return S_OK;
        }
    }

    if (d3d12->m_on_resize_target) {
        d3d12->m_on_resize_target(*d3d12, new_target_parameters->Width, new_target_parameters->Height);
    }

    ++g_resize_target_depth;

    const auto result = resize_target_fn(swap_chain, new_target_parameters);
    
    if (result != S_OK) {
        spdlog::error("Resize target failed: {:x}", result);
    }

    --g_resize_target_depth;

    return result;
}

/*HRESULT WINAPI D3D12Hook::create_swap_chain(IDXGIFactory4* factory, IUnknown* device, HWND hwnd, const DXGI_SWAP_CHAIN_DESC* desc, const DXGI_SWAP_CHAIN_FULLSCREEN_DESC* p_fullscreen_desc, IDXGIOutput* p_restrict_to_output, IDXGISwapChain** swap_chain)
{
    spdlog::info("D3D12 create swapchain called");

    auto d3d12 = g_d3d12_hook;

    d3d12->m_command_queue = (ID3D12CommandQueue*)device;
    
    if (d3d12->m_on_create_swap_chain) {
        d3d12->m_on_create_swap_chain(*d3d12);
    }

    auto create_swap_chain_fn = d3d12->m_create_swap_chain_hook->get_original<decltype(D3D12Hook::create_swap_chain)>();

    return create_swap_chain_fn(factory, device, hwnd, desc, p_fullscreen_desc, p_restrict_to_output, swap_chain);
}*/


`

---
## ðŸ“ D3D12Hook.hpp
**Sti:** $filePath  
**StÃ¸rrelse:** 4499 bytes  
**Sidst Ã¦ndret:** 2025-08-19 11:20:13

### ðŸ”— Includes
`cpp
#include <iostream>
`

### ðŸ—ï¸ Classes
`cpp

`

### âš™ï¸ Hook Functions
`cpp
D3D12Hook()
`

### âš™ï¸ General Functions
`cpp
void(D3D12Hook&)
`

### ðŸ“„ Komplet Indhold
`cpp
#pragma once

#include <iostream>
#include <functional>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi")

#include <d3d12.h>
#include <dxgi1_4.h>

#include "utility/PointerHook.hpp"
#include "utility/VtableHook.hpp"

class D3D12Hook
{
public:
	typedef std::function<void(D3D12Hook&)> OnPresentFn;
	typedef std::function<void(D3D12Hook&, uint32_t w, uint32_t h)> OnResizeBuffersFn;
    typedef std::function<void(D3D12Hook&, uint32_t w, uint32_t h)> OnResizeTargetFn;
    typedef std::function<void(D3D12Hook&)> OnCreateSwapChainFn;

	D3D12Hook() = default;
	virtual ~D3D12Hook();

	bool hook();
	bool unhook();

    bool is_hooked() {
        return m_hooked;
    }

    void on_present(OnPresentFn fn) {
        m_on_present = fn;
    }

    void on_post_present(OnPresentFn fn) {
        m_on_post_present = fn;
    }

    void on_resize_buffers(OnResizeBuffersFn fn) {
        m_on_resize_buffers = fn;
    }

    void on_resize_target(OnResizeTargetFn fn) {
        m_on_resize_target = fn;
    }

    /*void on_create_swap_chain(OnCreateSwapChainFn fn) {
        m_on_create_swap_chain = fn;
    }*/

    ID3D12Device4* get_device() const {
        return m_device;
    }

    IDXGISwapChain3* get_swap_chain() const {
        return m_swap_chain;
    }

    auto get_swapchain_0() { return m_swapchain_0; }
    auto get_swapchain_1() { return m_swapchain_1; }

    ID3D12CommandQueue* get_command_queue() const {
        return m_command_queue;
    }

    UINT get_display_width() const {
        return m_display_width;
    }

    UINT get_display_height() const {
        return m_display_height;
    }

    UINT get_render_width() const {
        return m_render_width;
    }

    UINT get_render_height() const {
        return m_render_height;
    }

    bool is_inside_present() const {
        return m_inside_present;
    }

    bool is_proton_swapchain() const {
        return m_using_proton_swapchain;
    }

    bool is_framegen_swapchain() const {
        return m_using_frame_generation_swapchain;
    }

    void ignore_next_present() {
        m_ignore_next_present = true;
    }

    void set_next_present_interval(uint32_t interval) {
        m_next_present_interval = interval;
    }

protected:
    ID3D12Device4* m_device{ nullptr };
    IDXGISwapChain3* m_swap_chain{ nullptr };
    IDXGISwapChain3* m_swapchain_0{};
    IDXGISwapChain3* m_swapchain_1{};
    ID3D12CommandQueue* m_command_queue{ nullptr };
    UINT m_display_width{ NULL };
    UINT m_display_height{ NULL };
    UINT m_render_width{ NULL };
    UINT m_render_height{ NULL };

    uint32_t m_command_queue_offset{};
    uint32_t m_proton_swapchain_offset{};

    std::optional<uint32_t> m_next_present_interval{};

    bool m_using_proton_swapchain{ false };
    bool m_using_frame_generation_swapchain{ false };
    bool m_hooked{ false };
    bool m_is_phase_1{ true };
    bool m_inside_present{false};
    bool m_ignore_next_present{false};

    std::unique_ptr<PointerHook> m_present_hook{};
    std::unique_ptr<PointerHook> m_present1_hook{};
    std::unique_ptr<VtableHook> m_swapchain_hook{};
    //std::unique_ptr<FunctionHook> m_create_swap_chain_hook{};

    OnPresentFn m_on_present{ nullptr };
    OnPresentFn m_on_post_present{ nullptr };
    OnResizeBuffersFn m_on_resize_buffers{ nullptr };
    OnResizeTargetFn m_on_resize_target{ nullptr };
    //OnCreateSwapChainFn m_on_create_swap_chain{ nullptr };
    
    static HRESULT present_internal(IDXGISwapChain3* swap_chain, UINT sync_interval, UINT flags, DXGI_PRESENT_PARAMETERS* params, bool present1 = false);

    static HRESULT WINAPI present(IDXGISwapChain3* swap_chain, UINT sync_interval, UINT flags);
    static HRESULT WINAPI present1(IDXGISwapChain3* swap_chain, UINT sync_interval, UINT flags, DXGI_PRESENT_PARAMETERS* params);
    static HRESULT WINAPI resize_buffers(IDXGISwapChain3* swap_chain, UINT buffer_count, UINT width, UINT height, DXGI_FORMAT new_format, UINT swap_chain_flags);
    static HRESULT WINAPI resize_target(IDXGISwapChain3* swap_chain, const DXGI_MODE_DESC* new_target_parameters);
    //static HRESULT WINAPI create_swap_chain(IDXGIFactory4* factory, IUnknown* device, HWND hwnd, const DXGI_SWAP_CHAIN_DESC* desc, const DXGI_SWAP_CHAIN_FULLSCREEN_DESC* p_fullscreen_desc, IDXGIOutput* p_restrict_to_output, IDXGISwapChain** swap_chain);
};


`

---
## ðŸ“ DInputHook.cpp
**Sti:** $filePath  
**StÃ¸rrelse:** 4908 bytes  
**Sidst Ã¦ndret:** 2025-08-19 11:20:13

### ðŸ”— Includes
`cpp
#include <chrono>
`

### ðŸ—ï¸ Classes
`cpp
CLASS_GAMECTRL) {
`

### âš™ï¸ Hook Functions
`cpp
DInputHook::DInputHook()
`

### âš™ï¸ General Functions
`cpp
void** enum_devices_ptr = (void**)&(*(uintptr_t**)iface)
`

### ðŸ“„ Komplet Indhold
`cpp
#include <chrono>

#include <spdlog/spdlog.h>
#include <utility/String.hpp>

#include "Framework.hpp"
#include "mods/VR.hpp"
#include "utility/Logging.hpp"

#include "DInputHook.hpp"

DInputHook* g_dinput_hook{nullptr};

constexpr size_t ENUM_DEVICES_VTABLE_INDEX = 4;

DInputHook::DInputHook() {
    SPDLOG_INFO("[DInputHook] Constructing DInputHook");

    g_dinput_hook = this;

    SPDLOG_INFO("[DInputHook] Creating thread");

    std::thread([this]() {
        SPDLOG_INFO("[DInputHook] Entering thread");

        const auto start_time = std::chrono::steady_clock::now();
        HMODULE dinput8{nullptr};

        while (dinput8 == nullptr) {
            dinput8 = GetModuleHandleA("dinput8.dll");

            if (dinput8) {
                SPDLOG_INFO("[DInputHook] dinput8.dll loaded");
                break;
            }

            const auto now = std::chrono::steady_clock::now();

            if (now - start_time > std::chrono::seconds(10)) {
                SPDLOG_ERROR("[DInputHook] Timed out waiting for dinput8.dll to load, aborting hook");
                return;
            }

            SPDLOG_INFO("[DInputHook] Waiting for dinput8.dll to load...");
            Sleep(1000);
        }

        auto create_addr = GetProcAddress(dinput8, "DirectInput8Create");

        if (create_addr == nullptr) {
            SPDLOG_ERROR("[DInputHook] Failed to find DirectInput8Create, aborting hook");
            return;
        }

        SPDLOG_INFO("[DInputHook] Found DirectInput8Create at {:x}", (uintptr_t)create_addr);

        m_create_hook = safetyhook::create_inline(create_addr, (uintptr_t)create_hooked);

        if (!m_create_hook) {
            SPDLOG_ERROR("[DInputHook] Failed to hook DirectInput8Create, aborting hook");
            return;
        }

        SPDLOG_INFO("[DInputHook] Hooked DirectInput8Create");
    }).detach();

    SPDLOG_INFO("[DInputHook] Exiting constructor");
}

HRESULT WINAPI DInputHook::create_hooked(
    HINSTANCE hinst,
    DWORD dwVersion,
    REFIID riidltf,
    LPVOID* ppvOut,
    LPUNKNOWN punkOuter
) 
{
    SPDLOG_INFO_EVERY_N_SEC(5, "[DInputHook] DirectInput8Create called {:x} {} {:x} {:x}", (uintptr_t)hinst, dwVersion, (uintptr_t)ppvOut, (uintptr_t)punkOuter);

    const auto og = g_dinput_hook->m_create_hook.original<decltype(&create_hooked)>();
    const auto result = og(hinst, dwVersion, riidltf, ppvOut, punkOuter);

    if (result == DI_OK) {
        if (ppvOut == nullptr) {
            SPDLOG_INFO("[DInputHook] ppvOut is null");
            return result;
        }

        std::scoped_lock _{g_dinput_hook->m_mutex};

        auto iface = (LPDIRECTINPUT8W)*ppvOut;

        // TODO: IID_IDirectInput8A or we don't care?
        if (iface != nullptr && memcmp(&riidltf, &IID_IDirectInput8W, sizeof(GUID)) == 0) {
            // Its not necessary to make a full blown vtable hook for this because
            // the vtable will always be the same for IDirectInput8W
            if (g_dinput_hook->m_enum_devices_hook == nullptr) {
                SPDLOG_INFO("[DInputHook] Hooking IDirectInput8::EnumDevices");
                void** enum_devices_ptr = (void**)&(*(uintptr_t**)iface)[ENUM_DEVICES_VTABLE_INDEX];
                g_dinput_hook->m_enum_devices_hook = std::make_unique<PointerHook>(enum_devices_ptr, (void*)&enum_devices_hooked);
                SPDLOG_INFO("[DInputHook] Hooked IDirectInput8::EnumDevices");
            }
        }
    } else {
        SPDLOG_INFO("[DInputHook] DirectInput8Create failed");
    }

    return result;
}

HRESULT DInputHook::enum_devices_hooked(
    LPDIRECTINPUT8W This,
    DWORD dwDevType,
    LPDIENUMDEVICESCALLBACKW lpCallback,
    LPVOID pvRef,
    DWORD dwFlags
)
{
    SPDLOG_INFO_EVERY_N_SEC(5, "[DInputHook] IDirectInput8::EnumDevices called");

    std::scoped_lock _{g_dinput_hook->m_mutex};

    const auto og = g_dinput_hook->m_enum_devices_hook->get_original<decltype(&enum_devices_hooked)>();

    if (og == nullptr) {
        SPDLOG_INFO("[DInputHook] IDirectInput8::EnumDevices original method is null");
        return DI_OK;
    }

    // We dont care about these other ones, so just call the original
    if (dwDevType != DI8DEVCLASS_GAMECTRL) {
        auto result = og(This, dwDevType, lpCallback, pvRef, dwFlags);
        return result;
    }

    // the purpose of this is to stop some games from spamming calls to EnumDevices
    // without a real controller connected, which causes the game to drop to single digit FPS
    auto should_call_original = g_framework->is_ready() && !VR::get()->is_using_controllers_within(std::chrono::seconds(5));

    if (should_call_original) {
        auto result = og(This, dwDevType, lpCallback, pvRef, dwFlags);
        return result;
    }

    return DI_OK;
}
`

---
## ðŸ“ DInputHook.hpp
**Sti:** $filePath  
**StÃ¸rrelse:** 996 bytes  
**Sidst Ã¦ndret:** 2025-08-19 11:20:13

### ðŸ”— Includes
`cpp
#include <unordered_map>
`

### ðŸ—ï¸ Classes
`cpp
class DInputHook {
`

### âš™ï¸ Hook Functions
`cpp
DInputHook()
`

### âš™ï¸ General Functions
`cpp

`

### ðŸ“„ Komplet Indhold
`cpp
#pragma once

#include <unordered_map>
#include <mutex>
#include <memory>
#include <array>

#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0800
#endif

#include <dinput.h>

#include <safetyhook.hpp>

#include <utility/PointerHook.hpp>

class DInputHook {
public:
    DInputHook();
    virtual ~DInputHook() = default;

private:
    static HRESULT WINAPI create_hooked(
        HINSTANCE hinst,
        DWORD dwVersion,
        REFIID riidltf,
        LPVOID* ppvOut,
        LPUNKNOWN punkOuter
    );

    static HRESULT enum_devices_hooked(
        LPDIRECTINPUT8W This,
        DWORD dwDevType,
        LPDIENUMDEVICESCALLBACKW lpCallback,
        LPVOID pvRef,
        DWORD dwFlags
    );

    // This is recursive because apparently EnumDevices
    // can call DirectInput8Create again... wHAT?
    std::recursive_mutex m_mutex{};

    safetyhook::InlineHook m_create_hook{};
    std::unique_ptr<PointerHook> m_enum_devices_hook{};
};
`

---
## ðŸ“ DirectXHooks.cpp
**Sti:** $filePath  
**StÃ¸rrelse:** 643 bytes  
**Sidst Ã¦ndret:** 2025-08-19 03:26:30

### ðŸ”— Includes
`cpp
#include "uevr/hooks/DirectXHooks.hpp"
`

### ðŸ—ï¸ Classes
`cpp

`

### âš™ï¸ Hook Functions
`cpp
DirectXHooks::DirectXHooks()
`

### âš™ï¸ General Functions
`cpp
void DirectXHooks::removeHooks()
`

### ðŸ“„ Komplet Indhold
`cpp
#include "uevr/hooks/DirectXHooks.hpp"
#include <iostream>

namespace uevr {
namespace Hooks {

DirectXHooks::DirectXHooks() {
    std::cout << "DirectX Hooks initialized" << std::endl;
}

DirectXHooks::~DirectXHooks() {
    std::cout << "DirectX Hooks cleanup" << std::endl;
}

bool DirectXHooks::installHooks() {
    std::cout << "Installing DirectX hooks..." << std::endl;
    // TODO: Implement actual DirectX hooking
    return true;
}

void DirectXHooks::removeHooks() {
    std::cout << "Removing DirectX hooks..." << std::endl;
    // TODO: Implement hook removal
}

} // namespace Hooks
} // namespace uevr

`

---
## ðŸ“ EngineHooks.cpp
**Sti:** $filePath  
**StÃ¸rrelse:** 631 bytes  
**Sidst Ã¦ndret:** 2025-08-19 03:26:28

### ðŸ”— Includes
`cpp
#include "uevr/hooks/EngineHooks.hpp"
`

### ðŸ—ï¸ Classes
`cpp

`

### âš™ï¸ Hook Functions
`cpp
EngineHooks::EngineHooks()
`

### âš™ï¸ General Functions
`cpp
void EngineHooks::removeHooks()
`

### ðŸ“„ Komplet Indhold
`cpp
#include "uevr/hooks/EngineHooks.hpp"
#include <iostream>

namespace uevr {
namespace Hooks {

EngineHooks::EngineHooks() {
    std::cout << "Engine Hooks initialized" << std::endl;
}

EngineHooks::~EngineHooks() {
    std::cout << "Engine Hooks cleanup" << std::endl;
}

bool EngineHooks::installHooks() {
    std::cout << "Installing engine hooks..." << std::endl;
    // TODO: Implement actual engine hooking
    return true;
}

void EngineHooks::removeHooks() {
    std::cout << "Removing engine hooks..." << std::endl;
    // TODO: Implement hook removal
}

} // namespace Hooks
} // namespace uevr

`

---
## ðŸ“ GameHooks.h
**Sti:** $filePath  
**StÃ¸rrelse:** 1113 bytes  
**Sidst Ã¦ndret:** 2025-08-19 10:55:06

### ðŸ”— Includes
`cpp
#include <string>
`

### ðŸ—ï¸ Classes
`cpp

`

### âš™ï¸ Hook Functions
`cpp
bool initializeGameHooks()
`

### âš™ï¸ General Functions
`cpp
void cleanupGameHooks()
`

### ðŸ“„ Komplet Indhold
`cpp
#pragma once

#include <string>
#include <vector>

namespace UEVR {
namespace Hooks {

// Game hook management
bool initializeGameHooks();
void cleanupGameHooks();
bool installEngineHooks(const std::string& engine_name);
bool uninstallEngineHooks();

// Matrix hook management
bool hookViewMatrix();
bool hookProjectionMatrix();
bool hookViewProjectionMatrix();
bool injectVRMatrices();

// Rendering hooks
bool hookSwapchainPresent();
bool hookDrawCall();
bool hookVertexShader();
bool hookPixelShader();

// Input hooks
bool hookGameInput();
bool injectVRInput();
bool overrideGameInput();

// UObjectHook system
bool initializeUObjectHooks();
bool scanForGameObjects();
bool attachObjectToController(const std::string& object_path, int controller);
bool detachObjectFromController(const std::string& object_path);

// Hook validation
bool validateHooks();
bool testHookFunctionality();
std::vector<std::string> getActiveHooks();

// Performance monitoring
bool enableHookProfiling();
bool getHookPerformanceStats();

} // namespace Hooks
} // namespace UEVR



`

---
## ðŸ“ HookManager.cpp
**Sti:** $filePath  
**StÃ¸rrelse:** 650 bytes  
**Sidst Ã¦ndret:** 2025-08-19 03:26:28

### ðŸ”— Includes
`cpp
#include "uevr/hooks/HookManager.hpp"
`

### ðŸ—ï¸ Classes
`cpp

`

### âš™ï¸ Hook Functions
`cpp
HookManager::HookManager()
`

### âš™ï¸ General Functions
`cpp
void HookManager::removeAllHooks()
`

### ðŸ“„ Komplet Indhold
`cpp
#include "uevr/hooks/HookManager.hpp"
#include <iostream>

namespace uevr {
namespace Hooks {

HookManager::HookManager() {
    std::cout << "Hook Manager initialized" << std::endl;
}

HookManager::~HookManager() {
    std::cout << "Hook Manager cleanup" << std::endl;
}

bool HookManager::installAllHooks() {
    std::cout << "Installing all hooks..." << std::endl;
    // TODO: Implement hook installation for all supported APIs
    return true;
}

void HookManager::removeAllHooks() {
    std::cout << "Removing all hooks..." << std::endl;
    // TODO: Implement hook removal
}

} // namespace Hooks
} // namespace uevr

`

---
## ðŸ“ OpenGLHooks.cpp
**Sti:** $filePath  
**StÃ¸rrelse:** 631 bytes  
**Sidst Ã¦ndret:** 2025-08-19 03:26:26

### ðŸ”— Includes
`cpp
#include "uevr/hooks/OpenGLHooks.hpp"
`

### ðŸ—ï¸ Classes
`cpp

`

### âš™ï¸ Hook Functions
`cpp
OpenGLHooks::OpenGLHooks()
`

### âš™ï¸ General Functions
`cpp
void OpenGLHooks::removeHooks()
`

### ðŸ“„ Komplet Indhold
`cpp
#include "uevr/hooks/OpenGLHooks.hpp"
#include <iostream>

namespace uevr {
namespace Hooks {

OpenGLHooks::OpenGLHooks() {
    std::cout << "OpenGL Hooks initialized" << std::endl;
}

OpenGLHooks::~OpenGLHooks() {
    std::cout << "OpenGL Hooks cleanup" << std::endl;
}

bool OpenGLHooks::installHooks() {
    std::cout << "Installing OpenGL hooks..." << std::endl;
    // TODO: Implement actual OpenGL hooking
    return true;
}

void OpenGLHooks::removeHooks() {
    std::cout << "Removing OpenGL hooks..." << std::endl;
    // TODO: Implement hook removal
}

} // namespace Hooks
} // namespace uevr

`

---
## ðŸ“ VulkanHooks.cpp
**Sti:** $filePath  
**StÃ¸rrelse:** 12231 bytes  
**Sidst Ã¦ndret:** 2025-08-19 03:38:38

### ðŸ”— Includes
`cpp
#include <uevr/hooks/VulkanHooks.hpp>
`

### ðŸ—ï¸ Classes
`cpp

`

### âš™ï¸ Hook Functions
`cpp
VulkanHooks::VulkanHooks()
`

### âš™ï¸ General Functions
`cpp
void**>(&m_originalFunctions.vkCreateSwapchainKHR)
`

### ðŸ“„ Komplet Indhold
`cpp
#include <uevr/hooks/VulkanHooks.hpp>
#include <MinHook.h>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <cstring>

namespace uevr {

VulkanHooks::VulkanHooks() 
    : m_isHooked(false)
    , m_vrEnabled(false)
    , m_lastError("")
    , m_performanceMetrics{0, 0, 0.0, 0.0, 0}
{
    // Initialize MinHook
    if (MH_Initialize() != MH_OK) {
        m_lastError = "Failed to initialize MinHook";
        return;
    }
    
    // Initialize VR state
    m_vrState.stereoEnabled = false;
    m_vrState.ipd = 0.064f; // Default IPD
    m_vrState.nearPlane = 0.1f;
    m_vrState.farPlane = 1000.0f;
    
    // Initialize matrices to identity
    std::memset(m_vrState.leftEye.viewMatrix, 0, sizeof(m_vrState.leftEye.viewMatrix));
    std::memset(m_vrState.leftEye.projectionMatrix, 0, sizeof(m_vrState.leftEye.projectionMatrix));
    std::memset(m_vrState.rightEye.viewMatrix, 0, sizeof(m_vrState.rightEye.viewMatrix));
    std::memset(m_vrState.rightEye.projectionMatrix, 0, sizeof(m_vrState.rightEye.projectionMatrix));
    
    // Set identity matrices
    m_vrState.leftEye.viewMatrix[0] = m_vrState.leftEye.viewMatrix[5] = 
    m_vrState.leftEye.viewMatrix[10] = m_vrState.leftEye.viewMatrix[15] = 1.0f;
    
    m_vrState.leftEye.projectionMatrix[0] = m_vrState.leftEye.projectionMatrix[5] = 
    m_vrState.leftEye.projectionMatrix[10] = m_vrState.leftEye.projectionMatrix[15] = 1.0f;
    
    m_vrState.rightEye.viewMatrix[0] = m_vrState.rightEye.viewMatrix[5] = 
    m_vrState.rightEye.viewMatrix[10] = m_vrState.rightEye.viewMatrix[15] = 1.0f;
    
    m_vrState.rightEye.projectionMatrix[0] = m_vrState.rightEye.projectionMatrix[5] = 
    m_vrState.rightEye.projectionMatrix[10] = m_vrState.rightEye.projectionMatrix[15] = 1.0f;
}

VulkanHooks::~VulkanHooks() {
    cleanupHooks();
    MH_Uninitialize();
}

bool VulkanHooks::initializeHooks() {
    if (m_isHooked) {
        return true; // Already hooked
    }
    
    std::cout << "[VulkanHooks] Initializing Vulkan hooks..." << std::endl;
    
    // Get Vulkan function pointers from loaded modules
    HMODULE vulkanModule = GetModuleHandleA("vulkan-1.dll");
    if (!vulkanModule) {
        vulkanModule = GetModuleHandleA("vulkan-1");
    }
    
    if (!vulkanModule) {
        m_lastError = "Vulkan module not found";
        return false;
    }
    
    // Get original function pointers
    m_originalFunctions.vkCreateSwapchainKHR = reinterpret_cast<PFN_vkCreateSwapchainKHR>(
        GetProcAddress(vulkanModule, "vkCreateSwapchainKHR")
    );
    m_originalFunctions.vkQueuePresentKHR = reinterpret_cast<PFN_vkQueuePresentKHR>(
        GetProcAddress(vulkanModule, "vkQueuePresentKHR")
    );
    m_originalFunctions.vkCmdBeginRenderPass = reinterpret_cast<PFN_vkCmdBeginRenderPass>(
        GetProcAddress(vulkanModule, "vkCmdBeginRenderPass")
    );
    m_originalFunctions.vkCmdDraw = reinterpret_cast<PFN_vkCmdDraw>(
        GetProcAddress(vulkanModule, "vkCmdDraw")
    );
    
    if (!m_originalFunctions.vkCreateSwapchainKHR || !m_originalFunctions.vkQueuePresentKHR ||
        !m_originalFunctions.vkCmdBeginRenderPass || !m_originalFunctions.vkCmdDraw) {
        m_lastError = "Failed to get Vulkan function pointers";
        return false;
    }
    
    // Install hooks
    if (!installHook(reinterpret_cast<void**>(&m_originalFunctions.vkCreateSwapchainKHR),
                     reinterpret_cast<void*>(CreateSwapchainKHR_Hook), "vkCreateSwapchainKHR")) {
        return false;
    }
    
    if (!installHook(reinterpret_cast<void**>(&m_originalFunctions.vkQueuePresentKHR),
                     reinterpret_cast<void*>(QueuePresentKHR_Hook), "vkQueuePresentKHR")) {
        return false;
    }
    
    if (!installHook(reinterpret_cast<void**>(&m_originalFunctions.vkCmdBeginRenderPass),
                     reinterpret_cast<void*>(CmdBeginRenderPass_Hook), "vkCmdBeginRenderPass")) {
        return false;
    }
    
    if (!installHook(reinterpret_cast<void**>(&m_originalFunctions.vkCmdDraw),
                     reinterpret_cast<void*>(CmdDraw_Hook), "vkCmdDraw")) {
        return false;
    }
    
    m_isHooked = true;
    std::cout << "[VulkanHooks] All Vulkan hooks installed successfully" << std::endl;
    return true;
}

void VulkanHooks::cleanupHooks() {
    if (!m_isHooked) {
        return;
    }
    
    std::cout << "[VulkanHooks] Cleaning up Vulkan hooks..." << std::endl;
    
    // Disable all hooks
    MH_DisableHook(MH_ALL_HOOKS);
    
    // Uninstall hooks
    MH_Unhook(reinterpret_cast<void*>(m_originalFunctions.vkCreateSwapchainKHR));
    MH_Unhook(reinterpret_cast<void*>(m_originalFunctions.vkQueuePresentKHR));
    MH_Unhook(reinterpret_cast<void*>(m_originalFunctions.vkCmdBeginRenderPass));
    MH_Unhook(reinterpret_cast<void*>(m_originalFunctions.vkCmdDraw));
    
    m_isHooked = false;
    std::cout << "[VulkanHooks] Vulkan hooks cleaned up" << std::endl;
}

void VulkanHooks::registerSwapchainCreateHook(SwapchainCreateInfoHook hook) {
    m_swapchainCreateHooks.push_back(hook);
}

void VulkanHooks::registerQueuePresentHook(QueuePresentHook hook) {
    m_queuePresentHooks.push_back(hook);
}

void VulkanHooks::registerCmdBeginRenderPassHook(CmdBeginRenderPassHook hook) {
    m_cmdBeginRenderPassHooks.push_back(hook);
}

void VulkanHooks::registerCmdDrawHook(CmdDrawHook hook) {
    m_cmdDrawHooks.push_back(hook);
}

bool VulkanHooks::extractViewProjectionMatrices(VkBuffer uniformBuffer, VkDeviceSize offset, VkDeviceSize size) {
    // This is a simplified implementation
    // In a real scenario, you would need to map the buffer and extract matrices
    
    std::cout << "[VulkanHooks] Extracting matrices from buffer (offset: " << offset << ", size: " << size << ")" << std::endl;
    
    // Simulate matrix extraction
    if (size >= 128) { // Minimum size for view + projection matrices
        // Update performance metrics
        m_performanceMetrics.frameCount++;
        updatePerformanceMetrics();
        
        return true;
    }
    
    return false;
}

bool VulkanHooks::enableVRStereoRendering() {
    if (m_vrEnabled) {
        return true; // Already enabled
    }
    
    std::cout << "[VulkanHooks] Enabling VR stereo rendering..." << std::endl;
    
    m_vrState.stereoEnabled = true;
    m_vrEnabled = true;
    
    // Apply VR transformations
    applyVRTransformation();
    
    std::cout << "[VulkanHooks] VR stereo rendering enabled" << std::endl;
    return true;
}

bool VulkanHooks::disableVRStereoRendering() {
    if (!m_vrEnabled) {
        return true; // Already disabled
    }
    
    std::cout << "[VulkanHooks] Disabling VR stereo rendering..." << std::endl;
    
    m_vrState.stereoEnabled = false;
    m_vrEnabled = false;
    
    std::cout << "[VulkanHooks] VR stereo rendering disabled" << std::endl;
    return true;
}

VulkanHooks::PerformanceMetrics VulkanHooks::getPerformanceMetrics() const {
    return m_performanceMetrics;
}

void VulkanHooks::clearErrors() {
    m_lastError.clear();
}

// Static hook implementations
VkResult VKAPI_CALL VulkanHooks::CreateSwapchainKHR_Hook(
    VkDevice device,
    const VkSwapchainCreateInfoKHR* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkSwapchainKHR* pSwapchain
) {
    std::cout << "[VulkanHooks] vkCreateSwapchainKHR called" << std::endl;
    
    // Call original function
    auto originalFunc = reinterpret_cast<PFN_vkCreateSwapchainKHR>(
        GetProcAddress(GetModuleHandleA("vulkan-1.dll"), "vkCreateSwapchainKHR")
    );
    
    if (originalFunc) {
        return originalFunc(device, pCreateInfo, pAllocator, pSwapchain);
    }
    
    return VK_ERROR_INITIALIZATION_FAILED;
}

VkResult VKAPI_CALL VulkanHooks::QueuePresentKHR_Hook(
    VkQueue queue,
    const VkPresentInfoKHR* pPresentInfo
) {
    static auto startTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - startTime);
    
    std::cout << "[VulkanHooks] vkQueuePresentKHR called (frame time: " << duration.count() << " Î¼s)" << std::endl;
    
    // Call original function
    auto originalFunc = reinterpret_cast<PFN_vkQueuePresentKHR>(
        GetProcAddress(GetModuleHandleA("vulkan-1.dll"), "vkQueuePresentKHR")
    );
    
    if (originalFunc) {
        startTime = currentTime; // Reset for next frame
        return originalFunc(queue, pPresentInfo);
    }
    
    return VK_ERROR_INITIALIZATION_FAILED;
}

void VKAPI_CALL VulkanHooks::CmdBeginRenderPass_Hook(
    VkCommandBuffer commandBuffer,
    const VkRenderPassBeginInfo* pRenderPassBegin,
    VkSubpassContents contents
) {
    std::cout << "[VulkanHooks] vkCmdBeginRenderPass called" << std::endl;
    
    // Call original function
    auto originalFunc = reinterpret_cast<PFN_vkCmdBeginRenderPass>(
        GetProcAddress(GetModuleHandleA("vulkan-1.dll"), "vkCmdBeginRenderPass")
    );
    
    if (originalFunc) {
        originalFunc(commandBuffer, pRenderPassBegin, contents);
    }
}

void VKAPI_CALL VulkanHooks::CmdDraw_Hook(
    VkCommandBuffer commandBuffer,
    uint32_t vertexCount,
    uint32_t instanceCount,
    uint32_t firstVertex,
    uint32_t firstInstance
) {
    std::cout << "[VulkanHooks] vkCmdDraw called (vertices: " << vertexCount << ", instances: " << instanceCount << ")" << std::endl;
    
    // Call original function
    auto originalFunc = reinterpret_cast<PFN_vkCmdDraw>(
        GetProcAddress(GetModuleHandleA("vulkan-1.dll"), "vkCmdDraw")
    );
    
    if (originalFunc) {
        originalFunc(commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
    }
}

// Private helper methods
bool VulkanHooks::installHook(void** originalFunction, void* hookFunction, const char* functionName) {
    if (MH_CreateHook(originalFunction, hookFunction, nullptr) != MH_OK) {
        m_lastError = "Failed to create hook for " + std::string(functionName);
        return false;
    }
    
    if (MH_EnableHook(originalFunction) != MH_OK) {
        m_lastError = "Failed to enable hook for " + std::string(functionName);
        return false;
    }
    
    std::cout << "[VulkanHooks] Hook installed for " << functionName << std::endl;
    return true;
}

void VulkanHooks::logError(const std::string& error) {
    m_lastError = error;
    std::cerr << "[VulkanHooks] ERROR: " << error << std::endl;
}

void VulkanHooks::updatePerformanceMetrics() {
    // Update average frame time
    static auto lastFrameTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto frameTime = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - lastFrameTime);
    
    if (m_performanceMetrics.frameCount > 0) {
        m_performanceMetrics.averageFrameTime = 
            (m_performanceMetrics.averageFrameTime * (m_performanceMetrics.frameCount - 1) + frameTime.count()) / 
            m_performanceMetrics.frameCount;
    }
    
    lastFrameTime = currentTime;
}

bool VulkanHooks::findMatrixInBuffer(const void* bufferData, VkDeviceSize bufferSize) {
    // Simplified matrix detection
    // In reality, you would scan for matrix patterns in the buffer
    return bufferSize >= 128; // Minimum size for view + projection matrices
}

void VulkanHooks::applyVRTransformation() {
    if (!m_vrState.stereoEnabled) {
        return;
    }
    
    // Apply IPD offset for right eye
    float ipdOffset = m_vrState.ipd * 0.5f;
    
    // Modify right eye view matrix to include IPD offset
    // This is a simplified transformation - in reality you'd use proper matrix math
    m_vrState.rightEye.viewMatrix[12] += ipdOffset; // X translation
    
    std::cout << "[VulkanHooks] Applied VR transformation (IPD: " << m_vrState.ipd << "m)" << std::endl;
}

} // namespace uevr

`

---
## ðŸ“ WindowsMessageHook.cpp
**Sti:** $filePath  
**StÃ¸rrelse:** 2425 bytes  
**Sidst Ã¦ndret:** 2025-08-19 11:20:13

### ðŸ”— Includes
`cpp
#include <unordered_map>
`

### ðŸ—ï¸ Classes
`cpp

`

### âš™ï¸ Hook Functions
`cpp
return CallWindowProc(g_windows_message_hook->get_original(), wnd, message, w_param, l_param)
`

### âš™ï¸ General Functions
`cpp

`

### ðŸ“„ Komplet Indhold
`cpp
#include <unordered_map>
#include <vector>

#include <spdlog/spdlog.h>

#include "utility/Thread.hpp"

#include "WindowsMessageHook.hpp"

using namespace std;

static WindowsMessageHook* g_windows_message_hook{ nullptr };
std::recursive_mutex g_proc_mutex{};

LRESULT WINAPI window_proc(HWND wnd, UINT message, WPARAM w_param, LPARAM l_param) {
    std::lock_guard _{ g_proc_mutex };

    if (g_windows_message_hook == nullptr) {
        return 0;
    }

    // Call our onMessage callback.
    auto& on_message = g_windows_message_hook->on_message;

    if (on_message) {
        // If it returns false we don't call the original window procedure.
        if (!on_message(wnd, message, w_param, l_param)) {
            return DefWindowProc(wnd, message, w_param, l_param);
        }
    }

    // Call the original message procedure.
    return CallWindowProc(g_windows_message_hook->get_original(), wnd, message, w_param, l_param);
}

WindowsMessageHook::WindowsMessageHook(HWND wnd)
    : m_wnd{ wnd },
    m_original_proc{ nullptr }
{
    std::lock_guard _{ g_proc_mutex };
    spdlog::info("Initializing WindowsMessageHook");

    g_windows_message_hook = this;

    // Set it to our "hook" procedure.
    m_original_proc = (WNDPROC)SetWindowLongPtr(m_wnd, GWLP_WNDPROC, (LONG_PTR)&window_proc);

    spdlog::info("Hooked Windows message handler");
}

WindowsMessageHook::~WindowsMessageHook() {
    std::lock_guard _{ g_proc_mutex };
    spdlog::info("Destroying WindowsMessageHook");
    
    remove();
    g_windows_message_hook = nullptr;
}

bool WindowsMessageHook::remove() {
    // Don't attempt to restore invalid original window procedures.
    if (m_original_proc == nullptr || m_wnd == nullptr) {
        return true;
    }

    // Restore the original window procedure.
    auto current_proc = (WNDPROC)GetWindowLongPtr(m_wnd, GWLP_WNDPROC);

    // lets not try to restore the original window procedure if it's not ours.
    if (current_proc == &window_proc) {
        SetWindowLongPtr(m_wnd, GWLP_WNDPROC, (LONG_PTR)m_original_proc);
    }

    // Invalidate this message hook.
    m_wnd = nullptr;
    m_original_proc = nullptr;

    return true;
}

bool WindowsMessageHook::is_hook_intact() {
    if (!m_wnd) {
        return false;
    }

    return GetWindowLongPtr(m_wnd, GWLP_WNDPROC) == (LONG_PTR)&window_proc;
}

`

---
## ðŸ“ WindowsMessageHook.hpp
**Sti:** $filePath  
**StÃ¸rrelse:** 1304 bytes  
**Sidst Ã¦ndret:** 2025-08-19 11:20:13

### ðŸ”— Includes
`cpp
#include <functional>
`

### ðŸ—ï¸ Classes
`cpp
class WindowsMessageHook {
`

### âš™ï¸ Hook Functions
`cpp
WindowsMessageHook()
`

### âš™ï¸ General Functions
`cpp
void window_toggle_cursor(bool show)
`

### ðŸ“„ Komplet Indhold
`cpp
#pragma once

#include <functional>

#include <Windows.h>

#define RE_TOGGLE_CURSOR WM_APP + 1

// This type of hook replaces a windows message procedure so that it can intercept
// messages sent to the window.
class WindowsMessageHook {
public:
    std::function<bool(HWND, UINT, WPARAM, LPARAM)> on_message;

    WindowsMessageHook() = delete;
    WindowsMessageHook(const WindowsMessageHook& other) = delete;
    WindowsMessageHook(WindowsMessageHook&& other) = delete;
    WindowsMessageHook(HWND wnd);
    virtual ~WindowsMessageHook();

    // This gets called automatically by the destructor but you can call it
    // explicitly if you need to remove the message hook for some reason.
    bool remove();

    auto is_valid() const {
        return m_original_proc != nullptr;
    }

    auto get_original() const {
        return m_original_proc;
    }

    inline void window_toggle_cursor(bool show) {
        // ONLY FOR RE ENGINE NOT UE!!!!
        //::PostMessage(m_wnd, RE_TOGGLE_CURSOR, show, 1);
    }

    WindowsMessageHook& operator=(const WindowsMessageHook& other) = delete;
    WindowsMessageHook& operator=(const WindowsMessageHook&& other) = delete;

    bool is_hook_intact();

private:
    HWND m_wnd;
    WNDPROC m_original_proc;
};
`

---
## ðŸ“ XInputHook.cpp
**Sti:** $filePath  
**StÃ¸rrelse:** 10944 bytes  
**Sidst Ã¦ndret:** 2025-08-19 11:20:13

### ðŸ”— Includes
`cpp
#include <chrono>
`

### ðŸ—ï¸ Classes
`cpp

`

### âš™ï¸ Hook Functions
`cpp
XInputHook::XInputHook()
`

### âš™ï¸ General Functions
`cpp
void*)GetProcAddress(xinput_1_4_dll, "XInputGetState")
`

### ðŸ“„ Komplet Indhold
`cpp
#include <chrono>
#include <spdlog/spdlog.h>
#include <utility/String.hpp>
#include <utility/Scan.hpp>

#include <SafetyHook.hpp>

#include "Framework.hpp"
#include "Mods.hpp"
#include "XInputHook.hpp"

XInputHook* g_hook{nullptr};

XInputHook::XInputHook() {
    g_hook = this;
    spdlog::info("[XInputHook] Entry");

    static auto find_dll = [](const std::string& name) -> HMODULE {
        const auto start_time = std::chrono::system_clock::now();
        auto found_dll = GetModuleHandleA(name.c_str());

        while (found_dll == nullptr) {
            if (found_dll = GetModuleHandleA(name.c_str()); found_dll != nullptr) {
                // Load it from the system directory instead, it might be hooked
                /*wchar_t system_dir[MAX_PATH]{};
                if (GetSystemDirectoryW(system_dir, MAX_PATH) != 0) {
                    const auto new_dir = (std::wstring{system_dir} + L"\\" + utility::widen(name));

                    spdlog::info("[XInputHook] Loading {} from {}", name, utility::narrow(new_dir));
                    const auto new_dll = LoadLibraryW(new_dir.c_str());

                    if (new_dll != nullptr) {
                        found_dll = LoadLibraryW(new_dir.c_str());
                    }
                }*/

                break;
            }

            const auto elapsed_time = std::chrono::system_clock::now() - start_time;

            if (elapsed_time > std::chrono::seconds(10)) {
                spdlog::error("[XInputHook] Failed to find {} after 10 seconds", name);
                return nullptr;
            }

            std::this_thread::yield();
        }

        return found_dll;
    };

    auto recursive_resolve_jmp = [](this const auto& self, uint8_t* instr) -> uintptr_t {
        try {
            const auto decoded = utility::decode_one(instr);

            if (decoded) {
                const auto mnem = std::string_view{decoded->Mnemonic};

                if (mnem.starts_with("JMP")) {
                    const auto target = utility::resolve_displacement((uintptr_t)instr);

                    if (target.has_value()) {
                        if (instr[0] == 0xFF && instr[1] == 0x25) {
                            const auto real_target = *(uintptr_t*)*target;

                            if (real_target == 0) {
                                return (uintptr_t)instr;
                            }

                            return self((uint8_t*)real_target);
                        }

                        return self((uint8_t*)target.value());
                    }
                }
            }
        } catch(...) {
            SPDLOG_ERROR("[XInputHook] recursive_resolve_jmp exception");
        }

        return (uintptr_t)instr;
    };

    auto perform_hooks_1_4 = [&]() {
        const auto xinput_1_4_dll = find_dll("xinput1_4.dll");

        if (xinput_1_4_dll != nullptr) {
            std::scoped_lock _{g_framework->get_hook_monitor_mutex()};

            const auto get_state_fn = (void*)GetProcAddress(xinput_1_4_dll, "XInputGetState");
            const auto set_state_fn = (void*)GetProcAddress(xinput_1_4_dll, "XInputSetState");

            if (get_state_fn != nullptr) {
                m_xinput_1_4_get_state_hook = safetyhook::create_inline(get_state_fn, get_state_hook_1_4);

                if (!m_xinput_1_4_get_state_hook) {
                    spdlog::error("Failed to hook XInputGetState (1_4), trying jmp");

                    // Check if there is a jmp instruction at the start of the function and try to hook that instead
                    const auto jmp_addr = recursive_resolve_jmp((uint8_t*)get_state_fn);

                    if (jmp_addr != (uintptr_t)get_state_fn) {
                        m_xinput_1_4_get_state_hook = safetyhook::create_inline((void*)jmp_addr, get_state_hook_1_4);

                        if (!m_xinput_1_4_get_state_hook) {
                            spdlog::error("Failed to hook XInputGetState (1_4) (jmp)");
                        }
                    } else {
                        spdlog::error("Cannot try jmp hook for XInputGetState (1_4) (jmp) (recursive_resolve_jmp failed)");
                    }
                }
            } else {
                spdlog::error("[XInputHook] Failed to find XInputGetState");
            }

            if (set_state_fn != nullptr) {
                m_xinput_1_4_set_state_hook = safetyhook::create_inline(set_state_fn, set_state_hook_1_4);

                if (!m_xinput_1_4_set_state_hook) {
                    spdlog::error("Failed to hook XInputSetState (1_4), trying jmp");

                    // Check if there is a jmp instruction at the start of the function and try to hook that instead
                    const auto jmp_addr = recursive_resolve_jmp((uint8_t*)set_state_fn);

                    if (jmp_addr != (uintptr_t)set_state_fn) {
                        m_xinput_1_4_set_state_hook = safetyhook::create_inline((void*)jmp_addr, set_state_hook_1_4);

                        if (!m_xinput_1_4_set_state_hook) {
                            spdlog::error("Failed to hook XInputSetState (1_4) (jmp)");
                        }
                    } else {
                        spdlog::error("Cannot try jmp hook for XInputSetState (1_4) (jmp) (recursive_resolve_jmp failed)");
                    }
                }
            } else {
                spdlog::error("[XInputHook] Failed to find XInputSetState");
            }
        }

         spdlog::info("[XInputHook] Done (1_4)");
    };

    auto perform_hooks_1_3 = [&]() {
        const auto xinput_1_3_dll = find_dll("xinput1_3.dll");

        if (xinput_1_3_dll != nullptr) {
            std::scoped_lock _{g_framework->get_hook_monitor_mutex()};

            const auto get_state_fn = (void*)GetProcAddress(xinput_1_3_dll, "XInputGetState");
            const auto set_state_fn = (void*)GetProcAddress(xinput_1_3_dll, "XInputSetState");

            if (get_state_fn != nullptr) {
                m_xinput_1_3_get_state_hook = safetyhook::create_inline(get_state_fn, get_state_hook_1_3);

                if (!m_xinput_1_3_get_state_hook) {
                    spdlog::error("Failed to hook XInputGetState (1_3), trying jmp");

                    // Check if there is a jmp instruction at the start of the function and try to hook that instead
                    const auto jmp_addr = recursive_resolve_jmp((uint8_t*)get_state_fn);

                    if (jmp_addr != (uintptr_t)get_state_fn) {
                        m_xinput_1_3_get_state_hook = safetyhook::create_inline((void*)jmp_addr, get_state_hook_1_3);

                        if (!m_xinput_1_3_get_state_hook) {
                            spdlog::error("Failed to hook XInputGetState (1_3) (jmp)");
                        }
                    } else {
                        spdlog::error("Cannot try jmp hook for XInputGetState (1_3) (jmp) (recursive_resolve_jmp failed)");
                    }
                }
            } else {
                spdlog::error("[XInputHook] Failed to find XInputGetState");
            }

            if (set_state_fn != nullptr) {
                m_xinput_1_3_set_state_hook = safetyhook::create_inline(set_state_fn, set_state_hook_1_3);

                if (!m_xinput_1_3_set_state_hook) {
                    spdlog::error("Failed to hook XInputSetState (1_3), trying jmp");

                    // Check if there is a jmp instruction at the start of the function and try to hook that instead
                    const auto jmp_addr = recursive_resolve_jmp((uint8_t*)set_state_fn);

                    if (jmp_addr != (uintptr_t)set_state_fn) {
                        m_xinput_1_3_set_state_hook = safetyhook::create_inline((void*)jmp_addr, set_state_hook_1_3);

                        if (!m_xinput_1_3_set_state_hook) {
                            spdlog::error("Failed to hook XInputSetState (1_3) (jmp)");
                        }
                    } else {
                        spdlog::error("Cannot try jmp hook for XInputSetState (1_3) (jmp) (recursive_resolve_jmp failed)");
                    }
                }
            } else {
                spdlog::error("[XInputHook] Failed to find XInputSetState");
            }
        }

        spdlog::info("[XInputHook] Done (1_3)");
    };

    // We use a thread because this may possibly take a while to search for the DLLs
    // and it shouldn't cause any issues anyway
    spdlog::info("[XInputHook] Starting hook thread");
    m_hook_thread_1_4 = std::make_unique<std::jthread>(perform_hooks_1_4);
    m_hook_thread_1_3 = std::make_unique<std::jthread>(perform_hooks_1_3);
    spdlog::info("[XInputHook] Hook thread started");
}

uint32_t XInputHook::get_state_hook_1_4(uint32_t user_index, XINPUT_STATE* state) {
    if (!g_framework->is_ready()) {
        return g_hook->m_xinput_1_4_get_state_hook.call<uint32_t>(user_index, state);
    }

    auto ret = g_hook->m_xinput_1_4_get_state_hook.call<uint32_t>(user_index, state);

    const auto& mods = g_framework->get_mods()->get_mods();

    for (auto& mod : mods) {
        mod->on_xinput_get_state(&ret, user_index, state);
    }

    return ret;
}

uint32_t XInputHook::set_state_hook_1_4(uint32_t user_index, XINPUT_VIBRATION* vibration) {
    if (!g_framework->is_ready()) {
        return g_hook->m_xinput_1_4_set_state_hook.call<uint32_t>(user_index, vibration);
    }

    auto ret = g_hook->m_xinput_1_4_set_state_hook.call<uint32_t>(user_index, vibration);

    const auto& mods = g_framework->get_mods()->get_mods();

    for (auto& mod : mods) {
        mod->on_xinput_set_state(&ret, user_index, vibration);
    }

    return ret;
}

uint32_t XInputHook::get_state_hook_1_3(uint32_t user_index, XINPUT_STATE* state) {
    if (!g_framework->is_ready()) {
        return g_hook->m_xinput_1_3_get_state_hook.call<uint32_t>(user_index, state);
    }

    auto ret = g_hook->m_xinput_1_3_get_state_hook.call<uint32_t>(user_index, state);

    const auto& mods = g_framework->get_mods()->get_mods();

    for (auto& mod : mods) {
        mod->on_xinput_get_state(&ret, user_index, state);
    }

    return ret;
}

uint32_t XInputHook::set_state_hook_1_3(uint32_t user_index, XINPUT_VIBRATION* vibration) {
    if (!g_framework->is_ready()) {
        return g_hook->m_xinput_1_3_set_state_hook.call<uint32_t>(user_index, vibration);
    }

    auto ret = g_hook->m_xinput_1_3_set_state_hook.call<uint32_t>(user_index, vibration);

    const auto& mods = g_framework->get_mods()->get_mods();

    for (auto& mod : mods) {
        mod->on_xinput_set_state(&ret, user_index, vibration);
    }

    return ret;
}
`

---
## ðŸ“ XInputHook.hpp
**Sti:** $filePath  
**StÃ¸rrelse:** 855 bytes  
**Sidst Ã¦ndret:** 2025-08-19 11:20:13

### ðŸ”— Includes
`cpp
#include <thread>
`

### ðŸ—ï¸ Classes
`cpp
class XInputHook {
`

### âš™ï¸ Hook Functions
`cpp
XInputHook()
`

### âš™ï¸ General Functions
`cpp

`

### ðŸ“„ Komplet Indhold
`cpp
#pragma once

#include <thread>

#include <safetyhook/inline_hook.hpp>
#include <Xinput.h>

class XInputHook {
public:
    XInputHook();

private:
    static uint32_t get_state_hook_1_4(uint32_t user_index, XINPUT_STATE* state);
    static uint32_t set_state_hook_1_4(uint32_t user_index, XINPUT_VIBRATION* vibration);

    static uint32_t get_state_hook_1_3(uint32_t user_index, XINPUT_STATE* state);
    static uint32_t set_state_hook_1_3(uint32_t user_index, XINPUT_VIBRATION* vibration);

    safetyhook::InlineHook m_xinput_1_4_get_state_hook;
    safetyhook::InlineHook m_xinput_1_4_set_state_hook;
    safetyhook::InlineHook m_xinput_1_3_get_state_hook;
    safetyhook::InlineHook m_xinput_1_3_set_state_hook;

    std::unique_ptr<std::jthread> m_hook_thread_1_4{};
    std::unique_ptr<std::jthread> m_hook_thread_1_3{};
};
`

---
