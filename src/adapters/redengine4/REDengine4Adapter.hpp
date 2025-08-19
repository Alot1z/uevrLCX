#pragma once

#include "../common/BaseCrossEngineAdapter.hpp"
#include <d3d12.h>
#include <dxgi.h>
#include <memory>
#include <vector>
#include <DirectXMath.h>

namespace uevr {

/**
 * @brief REDengine 4 VR Adapter for Cyberpunk 2077
 * 
 * Implements VR support for REDengine 4 games by hooking into the D3D12 rendering pipeline
 * and providing stereo rendering, camera tracking, and motion controller support.
 */
class REDengine4Adapter : public BaseCrossEngineAdapter {
public:
    REDengine4Adapter();
    virtual ~REDengine4Adapter() = default;

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
    // REDengine 4 specific functionality
    bool hookD3D12Device();
    bool hookD3D12SwapChain();
    bool hookCommandQueue();
    bool hookCameraSystem();
    bool hookRenderingPipeline();
    
    // VR rendering
    void setupStereoRendering();
    void renderLeftEye();
    void renderRightEye();
    void updateCameraMatrices();
    
    // Memory management
    bool scanForConstantBuffers();
    bool validateMemoryAddresses();
    
    // D3D12 resources
    ID3D12Device* m_d3d12Device;
    ID3D12CommandQueue* m_commandQueue;
    ID3D12GraphicsCommandList* m_commandList;
    ID3D12Resource* m_leftEyeResource;
    ID3D12Resource* m_rightEyeResource;
    ID3D12Resource* m_depthStencilResource;
    
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
        float ipd;
        float nearPlane;
        float farPlane;
        DirectX::XMMATRIX leftEyeView;
        DirectX::XMMATRIX rightEyeView;
        DirectX::XMMATRIX leftEyeProjection;
        DirectX::XMMATRIX rightEyeProjection;
    } m_vrState;
    
    // Hooking state
    struct HookState {
        bool d3d12DeviceHooked;
        bool swapChainHooked;
        bool commandQueueHooked;
        bool cameraSystemHooked;
        bool renderingPipelineHooked;
        void* originalPresent;
        void* originalResizeBuffers;
        void* originalExecuteCommandLists;
    } m_hookState;
    
    // Performance optimization
    struct PerformanceState {
        double lastFrameTime;
        double averageFrameTime;
        uint32_t frameCount;
        bool adaptiveQualityEnabled;
        float currentQualityLevel;
    } m_performanceState;
    
    // Constant buffer analysis
    struct ConstantBufferInfo {
        uint32_t rootParameterIndex;
        uint32_t descriptorTableIndex;
        uint32_t offset;
        size_t size;
        bool isViewMatrix;
        bool isProjectionMatrix;
    };
    std::vector<ConstantBufferInfo> m_constantBuffers;
};

} // namespace uevr
