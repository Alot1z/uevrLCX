#pragma once

#include "../common/BaseCrossEngineAdapter.hpp"
#include <d3d11.h>
#include <dxgi.h>
#include <memory>
#include <vector>
#include <DirectXMath.h>

namespace uevr {

/**
 * @brief MT Framework VR Adapter for Monster Hunter World
 * 
 * Implements VR support for MT Framework games by hooking into the D3D11 rendering pipeline
 * and providing stereo rendering, camera tracking (including third-person to first-person conversion),
 * and motion controller support.
 */
class MTFrameworkAdapter : public BaseCrossEngineAdapter {
public:
    MTFrameworkAdapter();
    virtual ~MTFrameworkAdapter() = default;

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
    // MT Framework specific functionality
    bool hookD3D11Device();
    bool hookD3D11SwapChain();
    bool hookCameraSystem();
    bool hookRenderingPipeline();
    bool hookAnimationSystem();
    
    // VR rendering
    void setupStereoRendering();
    void renderLeftEye();
    void renderRightEye();
    void updateCameraMatrices();
    
    // Third-person to first-person conversion
    void convertThirdPersonToFirstPerson();
    void updateCameraOffset();
    void handleCameraTransitions();
    
    // Memory management
    bool scanForCameraAddresses();
    bool validateMemoryAddresses();
    bool scanForAnimationAddresses();
    
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
        DirectX::XMFLOAT3 cameraOffset;
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
        bool d3d11DeviceHooked;
        bool swapChainHooked;
        bool cameraSystemHooked;
        bool animationSystemHooked;
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
    
    // Animation system
    struct AnimationState {
        uintptr_t skeletonAddr;
        uintptr_t animationAddr;
        bool addressesValid;
        float animationBlendFactor;
    } m_animationState;
};

} // namespace uevr
