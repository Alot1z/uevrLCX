#pragma once

#include "../../common/BaseCrossEngineAdapter.hpp"
#include <memory>
#include <unordered_map>
#include <vector>
#include <string>

namespace uevr {

/**
 * @brief Universal Unreal Engine VR Adapter
 * 
 * Supports UE4.20+ and UE5.x for 500+ games including:
 * - Fortnite, Gears series, Borderlands 3
 * - Generic UObject manipulation and rendering pipeline hooks
 * - Universal blueprint injection, scalable LOD systems
 * - Cross-platform compatibility
 */
class UnrealEngineAdapter : public BaseCrossEngineAdapter {
public:
    UnrealEngineAdapter(const std::string& name, const std::string& engineType, const std::string& gameExecutable);
    virtual ~UnrealEngineAdapter() = default;

    // ICrossEngineAdapter interface implementation
    bool initialize() override;
    void cleanup() override;
    bool isVREnabled() const override;
    void update() override;
    void render() override;
    std::string getName() const override;
    std::string getEngineType() const override;
    std::string getGameExecutable() const override;
    std::string getVersion() const override;

    // VR rendering callbacks
    void on_initialize() override;
    void on_device_reset() override;
    void on_post_render_vr_framework_dx11(ID3D11DeviceContext* context, ID3D11Texture2D* texture, ID3D11RenderTargetView* rtv) override;
    void on_post_render_vr_framework_dx12(ID3D12GraphicsCommandList* command_list, ID3D12Resource* rt, D3D12_CPU_DESCRIPTOR_HANDLE* rtv) override;

protected:
    // Common VR functionality
    bool initializeVR() override;
    void cleanupVR() override;
    void updateVR() override;
    void renderVR() override;
    
    // Engine-specific functionality
    bool detectEngine() override;
    bool initializeEngineHooks() override;
    void cleanupEngineHooks() override;

private:
    // Unreal Engine specific functionality
    bool detectUEVersion();
    bool initializeUObjectHooks();
    bool initializeRenderingHooks();
    bool initializeBlueprintHooks();
    bool initializeLODSystem();
    
    // UObject manipulation
    bool hookUObjectArray();
    bool hookUObjectFunctions();
    bool hookBlueprintFunctions();
    
    // Rendering pipeline hooks
    bool hookRenderPipeline();
    bool hookPostProcess();
    bool hookStereoRendering();
    
    // Performance optimization
    bool optimizeLODSystem();
    bool implementDynamicResolution();
    bool enableMotionReprojection();
    
    // VR-specific features
    bool setupVRStereoRendering();
    bool implementDynamicFOV();
    bool setupHapticFeedback();
    bool implementMotionControllers();
    
    // Engine version detection
    enum class UEVersion {
        UNKNOWN,
        UE4_20_TO_4_27,
        UE5_0_TO_5_3,
        UE5_4_PLUS
    };
    
    UEVersion m_ueVersion;
    std::string m_engineVersion;
    
    // Hook addresses and function pointers
    struct UEOffsets {
        uintptr_t uObjectArray;
        uintptr_t uObjectFunctions;
        uintptr_t renderPipeline;
        uintptr_t postProcess;
        uintptr_t stereoRendering;
        uintptr_t blueprintSystem;
        uintptr_t lodSystem;
    } m_offsets;
    
    // VR state
    struct VRState {
        bool stereoEnabled;
        bool motionReprojectionEnabled;
        bool dynamicFOVEnabled;
        bool hapticFeedbackEnabled;
        float currentFOV;
        float targetFOV;
        float ipd;
        bool motionControllersConnected;
    } m_vrState;
    
    // Performance metrics
    struct PerformanceState {
        uint32_t drawCalls;
        uint32_t triangles;
        uint32_t vertices;
        float frameTime;
        float gpuTime;
        float cpuTime;
        bool vsyncEnabled;
        uint32_t targetFPS;
    } m_performance;
    
    // Game-specific configurations
    struct GameConfig {
        std::string gameName;
        std::string engineVersion;
        bool supportsRayTracing;
        bool supportsDLSS;
        bool supportsFSR;
        std::vector<std::string> requiredHooks;
        std::vector<std::string> optionalHooks;
    } m_gameConfig;
    
    // Hook management
    std::vector<void*> m_activeHooks;
    bool m_hooksInitialized;
    
    // Error handling
    void logEngineError(const std::string& error, bool critical = false);
    bool validateEngineState();
    
    // Utility functions
    std::string getUEVersionString() const;
    bool isUE5() const;
    bool supportsAdvancedFeatures() const;
};

} // namespace uevr
