#pragma once

#include "../../common/BaseCrossEngineAdapter.hpp"
#include <memory>
#include <unordered_map>
#include <vector>
#include <string>

namespace uevr {

/**
 * @brief Universal CryEngine VR Adapter
 * 
 * Supports CryEngine 3.x through CRYENGINE 5.7 for games including:
 * - Hunt: Showdown, Kingdom Come: Deliverance, Crysis series
 * - Advanced lighting adaptation, vegetation rendering optimization
 * - High-performance rendering with advanced culling systems
 */
class CryEngineAdapter : public BaseCrossEngineAdapter {
public:
    CryEngineAdapter(const std::string& name, const std::string& engineType, const std::string& gameExecutable);
    virtual ~CryEngineAdapter() = default;

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
    // CryEngine specific functionality
    bool detectCryEngineVersion();
    bool initializeCrySystemHooks();
    bool initializeCryRenderHooks();
    bool initializeCry3DEngineHooks();
    bool initializeCryAnimationHooks();
    
    // Core system hooks
    bool hookCrySystem();
    bool hookCryRender();
    bool hookCry3DEngine();
    bool hookCryAnimation();
    bool hookCryInput();
    bool hookCryAudio();
    
    // Rendering system hooks
    bool hookRenderPipeline();
    bool hookPostProcess();
    bool hookLightingSystem();
    bool hookVegetationSystem();
    bool hookTerrainSystem();
    bool hookWaterSystem();
    
    // Advanced features
    bool implementAdvancedLighting();
    bool optimizeVegetationRendering();
    bool implementDynamicLOD();
    bool optimizeTerrainRendering();
    bool implementWaterOptimization();
    
    // VR-specific features
    bool setupVRStereoRendering();
    bool implementDynamicFOV();
    bool setupHapticFeedback();
    bool implementMotionControllers();
    bool optimizeForVR();
    
    // Engine version detection
    enum class CryEngineVersion {
        UNKNOWN,
        CRYENGINE_3_X,
        CRYENGINE_4_X,
        CRYENGINE_5_0_TO_5_3,
        CRYENGINE_5_4_TO_5_7
    };
    
    CryEngineVersion m_cryEngineVersion;
    std::string m_engineVersion;
    
    // Hook addresses and function pointers
    struct CryEngineOffsets {
        uintptr_t crySystem;
        uintptr_t cryRender;
        uintptr_t cry3DEngine;
        uintptr_t cryAnimation;
        uintptr_t cryInput;
        uintptr_t cryAudio;
        uintptr_t renderPipeline;
        uintptr_t postProcess;
        uintptr_t lightingSystem;
        uintptr_t vegetationSystem;
        uintptr_t terrainSystem;
        uintptr_t waterSystem;
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
        bool advancedLightingEnabled;
        bool vegetationOptimized;
    } m_vrState;
    
    // Performance metrics
    struct PerformanceState {
        uint32_t drawCalls;
        uint32_t triangles;
        uint32_t vertices;
        uint32_t lights;
        uint32_t vegetationInstances;
        uint32_t terrainChunks;
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
        bool supportsVulkan;
        bool supportsDX12;
        bool supportsRayTracing;
        bool supportsAdvancedLighting;
        std::vector<std::string> requiredHooks;
        std::vector<std::string> optionalHooks;
        std::vector<std::string> supportedFeatures;
    } m_gameConfig;
    
    // Hook management
    std::vector<void*> m_activeHooks;
    bool m_hooksInitialized;
    
    // Lighting system state
    struct LightingState {
        bool globalIlluminationEnabled;
        bool realTimeShadowsEnabled;
        bool volumetricFogEnabled;
        bool screenSpaceReflectionsEnabled;
        uint32_t activeLightCount;
        float ambientIntensity;
        float shadowQuality;
    } m_lightingState;
    
    // Vegetation system state
    struct VegetationState {
        bool instancingEnabled;
        bool LODEnabled;
        bool cullingEnabled;
        uint32_t maxInstances;
        uint32_t visibleInstances;
        float cullingDistance;
        bool windEnabled;
    } m_vegetationState;
    
    // Error handling
    void logEngineError(const std::string& error, bool critical = false);
    bool validateEngineState();
    
    // Utility functions
    std::string getCryEngineVersionString() const;
    bool isCryEngine5() const;
    bool supportsAdvancedLighting() const;
    bool supportsVulkan() const;
    bool supportsDX12() const;
    
    // Lighting management
    bool enableGlobalIllumination();
    bool enableRealTimeShadows();
    bool enableVolumetricFog();
    bool enableScreenSpaceReflections();
    bool optimizeLightingForVR();
    
    // Vegetation management
    bool enableVegetationInstancing();
    bool enableVegetationLOD();
    bool enableVegetationCulling();
    bool optimizeVegetationForVR();
    
    // Performance optimization
    bool implementDynamicLODSystem();
    bool implementOcclusionCulling();
    bool implementFrustumCulling();
    bool optimizeRenderingPipeline();
};

} // namespace uevr
