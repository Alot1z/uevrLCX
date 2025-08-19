#pragma once

#include "../common/BaseCrossEngineAdapter.hpp"
#include <d3d11.h>
#include <dxgi.h>
#include <memory>
#include <vector>
#include <DirectXMath.h>

namespace uevr {

/**
 * @brief RE Engine VR Adapter for Resident Evil 7
 * 
 * Implements VR support for RE Engine games by hooking into the D3D11 rendering pipeline
 * and providing stereo rendering, camera tracking, and motion controller support.
 */
class REEngineAdapter : public BaseCrossEngineAdapter {
public:
    REEngineAdapter();
    virtual ~REEngineAdapter() = default;

protected:
    // BaseCrossEngineAdapter interface implementation
    bool detectEngine() override;
    bool initializeEngineHooks() override;
    void cleanupEngineHooks() override;
    bool initializeVR() override;
    void cleanupVR() override;
    void updateVR() override;
    void renderVR() override;

private:
    // RE Engine specific functionality
    bool hookD3D11Device();
    bool hookD3D11SwapChain();
    bool hookCameraSystem();
    bool hookRenderingPipeline();
    
    // VR rendering
    void setupStereoRendering();
    void renderLeftEye();
    void renderRightEye();
    void updateCameraMatrices();
    
    // Memory management
    bool scanForCameraAddresses();
    bool validateMemoryAddresses();
    
    // Static hook functions
    static HRESULT STDMETHODCALLTYPE CreateBuffer_Hook(
        ID3D11Device* device, 
        const D3D11_BUFFER_DESC* pDesc, 
        const D3D11_SUBRESOURCE_DATA* pInitialData, 
        ID3D11Buffer** ppBuffer);
    
    static HRESULT STDMETHODCALLTYPE Present_Hook(
        IDXGISwapChain* swapChain, 
        UINT SyncInterval, 
        UINT Flags);
    
    static HRESULT STDMETHODCALLTYPE ResizeBuffers_Hook(
        IDXGISwapChain* swapChain, 
        UINT BufferCount, 
        UINT Width, 
        UINT Height, 
        DXGI_FORMAT NewFormat, 
        UINT SwapChainFlags);
    
    static void STDMETHODCALLTYPE VSSetConstantBuffers_Hook(
        ID3D11DeviceContext* context, 
        UINT StartSlot, 
        UINT NumBuffers, 
        ID3D11Buffer* const* ppConstantBuffers);
    
    static void STDMETHODCALLTYPE Draw_Hook(
        ID3D11DeviceContext* context, 
        UINT VertexCount, 
        UINT StartVertexLocation);
    
    // Function pointer types for hooks
    using CreateBuffer_t = HRESULT(STDMETHODCALLTYPE*)(ID3D11Device*, const D3D11_BUFFER_DESC*, const D3D11_SUBRESOURCE_DATA*, ID3D11Buffer**);
    using Present_t = HRESULT(STDMETHODCALLTYPE*)(IDXGISwapChain*, UINT, UINT);
    using ResizeBuffers_t = HRESULT(STDMETHODCALLTYPE*)(IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT);
    using VSSetConstantBuffers_t = void(STDMETHODCALLTYPE*)(ID3D11DeviceContext*, UINT, UINT, ID3D11Buffer* const*);
    using Draw_t = void(STDMETHODCALLTYPE*)(ID3D11DeviceContext*, UINT, UINT);
    
    // D3D11 resources
    ID3D11Device* m_d3d11Device;
    ID3D11DeviceContext* m_d3d11Context;
    ID3D11RenderTargetView* m_leftEyeRTV;
    ID3D11RenderTargetView* m_rightEyeRTV;
    ID3D11DepthStencilView* m_depthStencilView;
    
    // Camera system
    struct CameraSystem {
        bool detected;
        uintptr_t viewMatrixAddress;
        uintptr_t projectionMatrixAddress;
        DirectX::XMMATRIX currentView;
        DirectX::XMMATRIX currentProjection;
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT3 rotation;
    } m_cameraSystem;
    
    // VR state
    struct VRState {
        bool stereoEnabled;
        float ipd;                        // Inter-pupillary distance
        float nearPlane;                  // Near clipping plane
        float farPlane;                   // Far clipping plane
        DirectX::XMMATRIX leftEyeView;    // Left eye view matrix
        DirectX::XMMATRIX rightEyeView;   // Right eye view matrix
        DirectX::XMMATRIX leftEyeProjection;   // Left eye projection matrix
        DirectX::XMMATRIX rightEyeProjection;  // Right eye projection matrix
    } m_vrState;
    
    // Hooking state
    struct HookState {
        bool d3d11DeviceHooked;
        bool swapChainHooked;
        bool cameraSystemHooked;
        bool renderingPipelineHooked;
    } m_hookState;
    
    // Performance optimization
    struct PerformanceState {
        double lastFrameTime;
        double averageFrameTime;
        uint32_t frameCount;
        bool adaptiveQualityEnabled;
        float currentQualityLevel;
    } m_performanceState;
    
    // Performance metrics
    struct Performance {
        float frameTime;
        uint32_t drawCalls;
        uint32_t triangles;
    } m_performance;
};

} // namespace uevr
