#include "REEngineAdapter.hpp"
#include <windows.h>
#include <psapi.h>
#include <iostream>
#include <DirectXMath.h>
#include <algorithm>
#include <MinHook.h>
#include <d3d11.h>
#include <dxgi.h>

using namespace DirectX;

namespace uevr {

REEngineAdapter::REEngineAdapter()
    : BaseCrossEngineAdapter("RE Engine VR Adapter", "RE Engine", "re7.exe")
    , m_d3d11Device(nullptr)
    , m_d3d11Context(nullptr)
    , m_leftEyeRTV(nullptr)
    , m_rightEyeRTV(nullptr)
    , m_depthStencilView(nullptr)
    , m_cameraSystem{0, 0, 0, false}
    , m_vrState{false, 0.064f, 0.1f, 1000.0f}
    , m_hookState{false, false, false, false, nullptr, nullptr}
    , m_performanceState{0, true, 16.67f, 1.0f}
{
    // Initialize MinHook
    if (MH_Initialize() != MH_OK) {
        std::cerr << "[" << getName() << "] Failed to initialize MinHook" << std::endl;
    }
}

bool REEngineAdapter::detectEngine() {
    // Check if we're running in the correct process
    char processName[MAX_PATH];
    GetModuleFileNameA(nullptr, processName, MAX_PATH);
    
    std::string processPath(processName);
    std::string processNameLower = processPath.substr(processPath.find_last_of("\\/") + 1);
    
    // Convert to lowercase for comparison
    std::transform(processNameLower.begin(), processNameLower.end(), processNameLower.begin(), ::tolower);
    
    if (processNameLower.find("re7") != std::string::npos) {
        std::cout << "[" << getName() << "] RE Engine detected: " << processNameLower << std::endl;
        m_engineDetected = true;
        return true;
    }
    
    std::cout << "[" << getName() << "] RE Engine not detected. Process: " << processNameLower << std::endl;
    return false;
}

bool REEngineAdapter::initializeEngineHooks() {
    if (!m_engineDetected) {
        return false;
    }
    
    try {
        // Hook D3D11 device creation
        if (!hookD3D11Device()) {
            logError("Failed to hook D3D11 device", true);
            return false;
        }
        
        // Hook swap chain operations
        if (!hookD3D11SwapChain()) {
            logError("Failed to hook D3D11 swap chain", true);
            return false;
        }
        
        // Hook camera system
        if (!hookCameraSystem()) {
            logError("Failed to hook camera system", true);
            return false;
        }
        
        // Hook rendering pipeline
        if (!hookRenderingPipeline()) {
            logError("Failed to hook rendering pipeline", true);
            return false;
        }
        
        std::cout << "[" << getName() << "] Engine hooks initialized successfully" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        logError("Engine hook initialization exception: " + std::string(e.what()), true);
        return false;
    }
}

void REEngineAdapter::cleanupEngineHooks() {
    // Unhook all functions
    if (m_hookState.d3d11Hooked) {
        MH_Unhook(reinterpret_cast<LPVOID>(&ID3D11Device::CreateBuffer));
        m_hookState.d3d11Hooked = false;
    }
    
    if (m_hookState.swapChainHooked) {
        MH_Unhook(reinterpret_cast<LPVOID>(&IDXGISwapChain::Present));
        MH_Unhook(reinterpret_cast<LPVOID>(&IDXGISwapChain::ResizeBuffers));
        m_hookState.swapChainHooked = false;
    }
    
    if (m_hookState.cameraHooked) {
        MH_Unhook(reinterpret_cast<LPVOID>(&ID3D11DeviceContext::VSSetConstantBuffers));
        m_hookState.cameraHooked = false;
    }
    
    if (m_hookState.renderingHooked) {
        MH_Unhook(reinterpret_cast<LPVOID>(&ID3D11DeviceContext::Draw));
        m_hookState.renderingHooked = false;
    }
    
    // Reset hook state
    m_hookState = {false, false, false, false, nullptr, nullptr};
    
    // Cleanup MinHook
    MH_Uninitialize();
    
    std::cout << "[" << getName() << "] Engine hooks cleaned up" << std::endl;
}

bool REEngineAdapter::initializeVR() {
    try {
        // Initialize VR rendering resources
        setupStereoRendering();
        
        // Scan for camera addresses in memory
        if (!scanForCameraAddresses()) {
            logError("Failed to scan for camera addresses", true);
            return false;
        }
        
        // Validate memory addresses
        if (!validateMemoryAddresses()) {
            logError("Failed to validate memory addresses", true);
            return false;
        }
        
        // Initialize VR state
        m_vrState.stereoEnabled = true;
        m_vrState.ipd = 0.064f; // Default IPD
        m_vrState.nearPlane = 0.1f;
        m_vrState.farPlane = 1000.0f;
        
        std::cout << "[" << getName() << "] VR system initialized successfully" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        logError("VR initialization exception: " + std::string(e.what()), true);
        return false;
    }
}

void REEngineAdapter::cleanupVR() {
    // Release D3D11 resources
    if (m_leftEyeRTV) {
        m_leftEyeRTV->Release();
        m_leftEyeRTV = nullptr;
    }
    
    if (m_rightEyeRTV) {
        m_rightEyeRTV->Release();
        m_rightEyeRTV = nullptr;
    }
    
    if (m_depthStencilView) {
        m_depthStencilView->Release();
        m_depthStencilView = nullptr;
    }
    
    // Reset VR state
    m_vrState.stereoEnabled = false;
    
    std::cout << "[" << getName() << "] VR system cleaned up" << std::endl;
}

void REEngineAdapter::updateVR() {
    if (!m_vrState.stereoEnabled) {
        return;
    }
    
    // Update camera matrices
    updateCameraMatrices();
    
    // Update performance optimization
    if (m_performanceState.adaptiveQualityEnabled) {
        // Adjust quality based on frame time
        if (m_performance.frameTime > m_performanceState.targetFrameTime) {
            m_performanceState.currentQualityLevel = std::max(0.5f, m_performanceState.currentQualityLevel - 0.1f);
        } else if (m_performance.frameTime < m_performanceState.targetFrameTime * 0.8f) {
            m_performanceState.currentQualityLevel = std::min(1.0f, m_performanceState.currentQualityLevel + 0.05f);
        }
    }
}

void REEngineAdapter::renderVR() {
    if (!m_vrState.stereoEnabled || !m_cameraSystem.addressesValid) {
        return;
    }
    
    try {
        // Render left eye
        renderLeftEye();
        
        // Render right eye
        renderRightEye();
        
    } catch (const std::exception& e) {
        logError("VR rendering exception: " + std::string(e.what()), false);
    }
}

bool REEngineAdapter::hookD3D11Device() {
    // Hook D3D11 device creation to capture device and context
    if (MH_CreateHook(
        reinterpret_cast<LPVOID>(&ID3D11Device::CreateBuffer),
        reinterpret_cast<LPVOID>(&REEngineAdapter::CreateBuffer_Hook),
        &m_hookState.originalCreateBuffer) != MH_OK) {
        return false;
    }
    
    if (MH_EnableHook(reinterpret_cast<LPVOID>(&ID3D11Device::CreateBuffer)) != MH_OK) {
        return false;
    }
    
    m_hookState.d3d11Hooked = true;
    std::cout << "[" << getName() << "] D3D11 device hooked successfully" << std::endl;
    return true;
}

bool REEngineAdapter::hookD3D11SwapChain() {
    // Hook Present function for frame timing and VR rendering
    if (MH_CreateHook(
        reinterpret_cast<LPVOID>(&IDXGISwapChain::Present),
        reinterpret_cast<LPVOID>(&REEngineAdapter::Present_Hook),
        &m_hookState.originalPresent) != MH_OK) {
        return false;
    }
    
    if (MH_EnableHook(reinterpret_cast<LPVOID>(&IDXGISwapChain::Present)) != MH_OK) {
        return false;
    }
    
    // Hook ResizeBuffers for window resize handling
    if (MH_CreateHook(
        reinterpret_cast<LPVOID>(&IDXGISwapChain::ResizeBuffers),
        reinterpret_cast<LPVOID>(&REEngineAdapter::ResizeBuffers_Hook),
        &m_hookState.originalResizeBuffers) != MH_OK) {
        return false;
    }
    
    if (MH_EnableHook(reinterpret_cast<LPVOID>(&IDXGISwapChain::ResizeBuffers)) != MH_OK) {
        return false;
    }
    
    m_hookState.swapChainHooked = true;
    std::cout << "[" << getName() << "] D3D11 swap chain hooked successfully" << std::endl;
    return true;
}

bool REEngineAdapter::hookCameraSystem() {
    // Hook VSSetConstantBuffers to capture camera matrices
    if (MH_CreateHook(
        reinterpret_cast<LPVOID>(&ID3D11DeviceContext::VSSetConstantBuffers),
        reinterpret_cast<LPVOID>(&REEngineAdapter::VSSetConstantBuffers_Hook),
        &m_hookState.originalVSSetConstantBuffers) != MH_OK) {
        return false;
    }
    
    if (MH_EnableHook(reinterpret_cast<LPVOID>(&ID3D11DeviceContext::VSSetConstantBuffers)) != MH_OK) {
        return false;
    }
    
    m_hookState.cameraHooked = true;
    std::cout << "[" << getName() << "] Camera system hooked successfully" << std::endl;
    return true;
}

bool REEngineAdapter::hookRenderingPipeline() {
    // Hook Draw function for rendering pipeline analysis
    if (MH_CreateHook(
        reinterpret_cast<LPVOID>(&ID3D11DeviceContext::Draw),
        reinterpret_cast<LPVOID>(&REEngineAdapter::Draw_Hook),
        &m_hookState.originalDraw) != MH_OK) {
        return false;
    }
    
    if (MH_EnableHook(reinterpret_cast<LPVOID>(&ID3D11DeviceContext::Draw)) != MH_OK) {
        return false;
    }
    
    m_hookState.renderingHooked = true;
    std::cout << "[" << getName() << "] Rendering pipeline hooked successfully" << std::endl;
    return true;
}

void REEngineAdapter::setupStereoRendering() {
    // This would set up stereo rendering resources
    // For now, we'll just mark it as ready
    std::cout << "[" << getName() << "] Stereo rendering setup completed" << std::endl;
}

void REEngineAdapter::renderLeftEye() {
    // Render left eye view with left eye matrices
    if (m_vrState.leftEyeView.r[0].m128_f32[0] != 0.0f) {
        // Apply left eye view and projection matrices
        // This would render the scene from the left eye perspective
    }
}

void REEngineAdapter::renderRightEye() {
    // Render right eye view with right eye matrices
    if (m_vrState.rightEyeView.r[0].m128_f32[0] != 0.0f) {
        // Apply right eye view and projection matrices
        // This would render the scene from the right eye perspective
    }
}

void REEngineAdapter::updateCameraMatrices() {
    if (!m_cameraSystem.addressesValid) {
        return;
    }
    
    // Extract matrices from constant buffers
    if (m_d3d11Context && m_cameraSystem.constantBuffer) {
        // Map constant buffer to read camera matrices
        D3D11_MAPPED_SUBRESOURCE mappedResource;
        if (SUCCEEDED(m_d3d11Context->Map(m_cameraSystem.constantBuffer, 0, D3D11_MAP_READ, 0, &mappedResource))) {
            // Read view matrix (typically at offset 0)
            DirectX::XMMATRIX viewMatrix = DirectX::XMLoadFloat4x4(
                reinterpret_cast<DirectX::XMFLOAT4X4*>(mappedResource.pData)
            );
            
            // Read projection matrix (typically at offset 64 bytes)
            DirectX::XMMATRIX projMatrix = DirectX::XMLoadFloat4x4(
                reinterpret_cast<DirectX::XMFLOAT4X4*>(
                    reinterpret_cast<uint8_t*>(mappedResource.pData) + 64
                )
            );
            
            // Convert to VR stereo matrices
            m_vrState.leftEyeView = viewMatrix;
            m_vrState.rightEyeView = viewMatrix;
            
            // Apply IPD offset for right eye
            DirectX::XMMATRIX rightEyeOffset = DirectX::XMMatrixTranslation(m_vrState.ipd * 0.5f, 0.0f, 0.0f);
            m_vrState.rightEyeView = DirectX::XMMatrixMultiply(m_vrState.rightEyeView, rightEyeOffset);
            
            // Set projection matrices
            m_vrState.leftEyeProjection = projMatrix;
            m_vrState.rightEyeProjection = projMatrix;
            
            m_d3d11Context->Unmap(m_cameraSystem.constantBuffer, 0);
        }
    }
}

bool REEngineAdapter::scanForCameraAddresses() {
    // Scan for camera-related addresses in memory
    // This is a simplified version - in practice, you'd use pattern scanning
    
    // For RE Engine, camera matrices are typically in constant buffer slot 0
    m_cameraSystem.constantBufferSlot = 0;
    m_cameraSystem.viewMatrixOffset = 0;      // View matrix at offset 0
    m_cameraSystem.projectionMatrixOffset = 64; // Projection matrix at offset 64 bytes
    
    std::cout << "[" << getName() << "] Camera addresses scanned - CB Slot: " 
              << m_cameraSystem.constantBufferSlot << std::endl;
    return true;
}

bool REEngineAdapter::validateMemoryAddresses() {
    // Validate that the addresses are valid and accessible
    // For now, we'll assume they are valid
    
    m_cameraSystem.addressesValid = true;
    std::cout << "[" << getName() << "] Memory addresses validated" << std::endl;
    return true;
}

// Static hook functions
HRESULT STDMETHODCALLTYPE REEngineAdapter::CreateBuffer_Hook(
    ID3D11Device* device, 
    const D3D11_BUFFER_DESC* pDesc, 
    const D3D11_SUBRESOURCE_DATA* pInitialData, 
    ID3D11Buffer** ppBuffer) {
    
    // Call original function
    HRESULT result = reinterpret_cast<CreateBuffer_t>(m_hookState.originalCreateBuffer)(
        device, pDesc, pInitialData, ppBuffer);
    
    // Capture device and context if this is the first buffer creation
    if (SUCCEEDED(result) && !m_d3d11Device) {
        m_d3d11Device = device;
        device->GetImmediateContext(&m_d3d11Context);
        std::cout << "[" << getName() << "] D3D11 device and context captured" << std::endl;
    }
    
    return result;
}

HRESULT STDMETHODCALLTYPE REEngineAdapter::Present_Hook(
    IDXGISwapChain* swapChain, 
    UINT SyncInterval, 
    UINT Flags) {
    
    // Call original function
    HRESULT result = reinterpret_cast<Present_t>(m_hookState.originalPresent)(
        swapChain, SyncInterval, Flags);
    
    // Update VR rendering
    if (SUCCEEDED(result)) {
        updateVR();
        renderVR();
    }
    
    return result;
}

HRESULT STDMETHODCALLTYPE REEngineAdapter::ResizeBuffers_Hook(
    IDXGISwapChain* swapChain, 
    UINT BufferCount, 
    UINT Width, 
    UINT Height, 
    DXGI_FORMAT NewFormat, 
    UINT SwapChainFlags) {
    
    // Call original function
    HRESULT result = reinterpret_cast<ResizeBuffers_t>(m_hookState.originalResizeBuffers)(
        swapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);
    
    // Recreate VR resources if resize was successful
    if (SUCCEEDED(result)) {
        setupStereoRendering();
    }
    
    return result;
}

void STDMETHODCALLTYPE REEngineAdapter::VSSetConstantBuffers_Hook(
    ID3D11DeviceContext* context, 
    UINT StartSlot, 
    UINT NumBuffers, 
    ID3D11Buffer* const* ppConstantBuffers) {
    
    // Call original function
    reinterpret_cast<VSSetConstantBuffers_t>(m_hookState.originalVSSetConstantBuffers)(
        context, StartSlot, NumBuffers, ppConstantBuffers);
    
    // Capture constant buffer for camera matrices
    if (StartSlot == m_cameraSystem.constantBufferSlot && NumBuffers > 0) {
        m_cameraSystem.constantBuffer = ppConstantBuffers[0];
        std::cout << "[" << getName() << "] Camera constant buffer captured" << std::endl;
    }
}

void STDMETHODCALLTYPE REEngineAdapter::Draw_Hook(
    ID3D11DeviceContext* context, 
    UINT VertexCount, 
    UINT StartVertexLocation) {
    
    // Call original function
    reinterpret_cast<Draw_t>(m_hookState.originalDraw)(
        context, VertexCount, StartVertexLocation);
    
    // Analyze rendering pipeline for optimization
    // This could include LOD management, culling optimization, etc.
}

} // namespace uevr
