#pragma once

#include "../../common/BaseCrossEngineAdapter.hpp"
#include <memory>
#include <unordered_map>
#include <vector>
#include <string>

namespace uevr {

/**
 * @brief Universal Source Engine VR Adapter
 * 
 * Supports Source Engine/Source 2 for games including:
 * - Half-Life: Alyx, Counter-Strike 2, Dota 2
 * - Legacy compatibility maintenance, modern VR integration
 * - SteamVR integration and advanced VR features
 */
class SourceEngineAdapter : public BaseCrossEngineAdapter {
public:
    SourceEngineAdapter(const std::string& name, const std::string& engineType, const std::string& gameExecutable);
    virtual ~SourceEngineAdapter() = default;

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
    // Source Engine specific functionality
    bool detectSourceVersion();
    bool initializeSourceHooks();
    bool initializeSteamVRHooks();
    bool initializeRenderingHooks();
    bool initializeInputHooks();
    
    // Core system hooks
    bool hookSourceEngine();
    bool hookSource2Engine();
    bool hookSteamVR();
    bool hookSourceRendering();
    bool hookSourceInput();
    bool hookSourceAudio();
    bool hookSourcePhysics();
    
    // Rendering system hooks
    bool hookRenderPipeline();
    bool hookPostProcess();
    bool hookMaterialSystem();
    bool hookTextureSystem();
    bool hookModelSystem();
    bool hookParticleSystem();
    
    // Advanced features
    bool implementLegacyCompatibility();
    bool implementModernVRIntegration();
    bool optimizeRenderingPipeline();
    bool implementAdvancedMaterials();
    bool implementDynamicLOD();
    
    // VR-specific features
    bool setupVRStereoRendering();
    bool implementDynamicFOV();
    bool setupHapticFeedback();
    bool implementMotionControllers();
    bool integrateSteamVR();
    bool optimizeForVR();
    
    // Engine version detection
    enum class SourceVersion {
        UNKNOWN,
        SOURCE_ENGINE,
        SOURCE_2
    };
    
    SourceVersion m_sourceVersion;
    std::string m_engineVersion;
    
    // SteamVR integration
    enum class SteamVRSupport {
        UNKNOWN,
        NATIVE,
        COMPATIBILITY,
        NONE
    };
    
    SteamVRSupport m_steamVRSupport;
    
    // Hook addresses and function pointers
    struct SourceOffsets {
        uintptr_t sourceEngine;
        uintptr_t source2Engine;
        uintptr_t steamVR;
        uintptr_t sourceRendering;
        uintptr_t sourceInput;
        uintptr_t sourceAudio;
        uintptr_t sourcePhysics;
        uintptr_t renderPipeline;
        uintptr_t postProcess;
        uintptr_t materialSystem;
        uintptr_t textureSystem;
        uintptr_t modelSystem;
        uintptr_t particleSystem;
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
        bool steamVRIntegrated;
        bool legacyCompatibilityEnabled;
    } m_vrState;
    
    // Performance metrics
    struct PerformanceState {
        uint32_t drawCalls;
        uint32_t triangles;
        uint32_t vertices;
        uint32_t materials;
        uint32_t textures;
        uint32_t models;
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
        SourceVersion sourceVersion;
        SteamVRSupport steamVRSupport;
        bool supportsVulkan;
        bool supportsDX12;
        bool supportsAdvancedMaterials;
        std::vector<std::string> requiredHooks;
        std::vector<std::string> optionalHooks;
        std::vector<std::string> supportedFeatures;
    } m_gameConfig;
    
    // Hook management
    std::vector<void*> m_activeHooks;
    bool m_hooksInitialized;
    
    // SteamVR integration state
    struct SteamVRState {
        bool connected;
        bool initialized;
        bool hapticsEnabled;
        bool motionControllersEnabled;
        bool advancedFeaturesEnabled;
        std::string version;
        std::vector<std::string> supportedFeatures;
    } m_steamVRState;
    
    // Legacy compatibility state
    struct LegacyCompatibilityState {
        bool enabled;
        bool source1Compatibility;
        bool source2Compatibility;
        bool oldMaterialSupport;
        bool oldTextureSupport;
        bool oldModelSupport;
        bool oldInputSupport;
    } m_legacyState;
    
    // Error handling
    void logEngineError(const std::string& error, bool critical = false);
    bool validateEngineState();
    
    // Utility functions
    std::string getSourceVersionString() const;
    bool isSource2() const;
    bool supportsSteamVR() const;
    bool supportsVulkan() const;
    bool supportsDX12() const;
    
    // SteamVR management
    bool initializeSteamVR();
    bool connectToSteamVR();
    bool enableSteamVRFeatures();
    bool optimizeForSteamVR();
    
    // Legacy compatibility management
    bool enableLegacyCompatibility();
    bool enableSource1Compatibility();
    bool enableSource2Compatibility();
    bool maintainLegacyFeatures();
    
    // Rendering management
    bool enableVulkanRendering();
    bool enableDX12Rendering();
    bool enableAdvancedMaterials();
    bool enableDynamicLOD();
    bool optimizeRenderingForVR();
    
    // Performance optimization
    bool implementAdvancedMaterialSystem();
    bool optimizeTextureRendering();
    bool optimizeModelRendering();
    bool implementDynamicLODSystem();
    bool optimizeRenderingPipeline();
    
    // Input management
    bool enableMotionControllers();
    bool enableHapticFeedback();
    bool enableAdvancedInput();
    bool optimizeInputForVR();
};

} // namespace uevr
