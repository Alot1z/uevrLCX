#pragma once

#include "../../common/BaseCrossEngineAdapter.hpp"
#include <memory>
#include <unordered_map>
#include <vector>
#include <string>

namespace uevr {

/**
 * @brief Universal id Tech Engine VR Adapter
 * 
 * Supports id Tech 6/7 for games including:
 * - DOOM (2016), DOOM Eternal, Wolfenstein series
 * - High-performance rendering optimization, advanced culling systems
 * - Vulkan and OpenGL support with VR optimization
 */
class IdTechAdapter : public BaseCrossEngineAdapter {
public:
    IdTechAdapter(const std::string& name, const std::string& engineType, const std::string& gameExecutable);
    virtual ~IdTechAdapter() = default;

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
    // id Tech specific functionality
    bool detectIdTechVersion();
    bool initializeIdTechHooks();
    bool initializeVulkanHooks();
    bool initializeOpenGLHooks();
    bool initializeRenderingHooks();
    
    // Core system hooks
    bool hookIdTech();
    bool hookIdRenderSystem();
    bool hookIdGame();
    bool hookIdInput();
    bool hookIdAudio();
    bool hookIdPhysics();
    
    // Rendering system hooks
    bool hookRenderPipeline();
    bool hookPostProcess();
    bool hookShadowSystem();
    bool hookParticleSystem();
    bool hookDecalSystem();
    bool hookLightingSystem();
    
    // Advanced features
    bool implementAdvancedCulling();
    bool optimizeRenderingPipeline();
    bool implementDynamicLOD();
    bool optimizeParticleSystems();
    bool implementAdvancedShadows();
    
    // VR-specific features
    bool setupVRStereoRendering();
    bool implementDynamicFOV();
    bool setupHapticFeedback();
    bool implementMotionControllers();
    bool optimizeForVR();
    
    // Engine version detection
    enum class IdTechVersion {
        UNKNOWN,
        ID_TECH_6,
        ID_TECH_7
    };
    
    IdTechVersion m_idTechVersion;
    std::string m_engineVersion;
    
    // Graphics API detection
    enum class GraphicsAPI {
        UNKNOWN,
        VULKAN,
        OPENGL,
        DIRECTX
    };
    
    GraphicsAPI m_graphicsAPI;
    
    // Hook addresses and function pointers
    struct IdTechOffsets {
        uintptr_t idTech;
        uintptr_t idRenderSystem;
        uintptr_t idGame;
        uintptr_t idInput;
        uintptr_t idAudio;
        uintptr_t idPhysics;
        uintptr_t renderPipeline;
        uintptr_t postProcess;
        uintptr_t shadowSystem;
        uintptr_t particleSystem;
        uintptr_t decalSystem;
        uintptr_t lightingSystem;
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
        bool advancedCullingEnabled;
        bool optimizedRenderingEnabled;
    } m_vrState;
    
    // Performance metrics
    struct PerformanceState {
        uint32_t drawCalls;
        uint32_t triangles;
        uint32_t vertices;
        uint32_t particles;
        uint32_t lights;
        uint32_t shadows;
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
        GraphicsAPI graphicsAPI;
        bool supportsVulkan;
        bool supportsOpenGL;
        bool supportsRayTracing;
        bool supportsAdvancedCulling;
        std::vector<std::string> requiredHooks;
        std::vector<std::string> optionalHooks;
        std::vector<std::string> supportedFeatures;
    } m_gameConfig;
    
    // Hook management
    std::vector<void*> m_activeHooks;
    bool m_hooksInitialized;
    
    // Rendering system state
    struct RenderingState {
        bool vulkanEnabled;
        bool openGLEnabled;
        bool advancedCullingEnabled;
        bool dynamicLODEnabled;
        bool particleOptimizationEnabled;
        bool shadowOptimizationEnabled;
        uint32_t maxDrawCalls;
        uint32_t maxTriangles;
        bool vsyncEnabled;
    } m_renderingState;
    
    // Culling system state
    struct CullingState {
        bool frustumCullingEnabled;
        bool occlusionCullingEnabled;
        bool portalCullingEnabled;
        bool LODCullingEnabled;
        uint32_t visibleObjects;
        uint32_t culledObjects;
        float cullingDistance;
        bool dynamicCullingEnabled;
    } m_cullingState;
    
    // Error handling
    void logEngineError(const std::string& error, bool critical = false);
    bool validateEngineState();
    
    // Utility functions
    std::string getIdTechVersionString() const;
    bool isIdTech7() const;
    bool supportsVulkan() const;
    bool supportsOpenGL() const;
    bool supportsAdvancedCulling() const;
    
    // Rendering management
    bool enableVulkanRendering();
    bool enableOpenGLRendering();
    bool enableAdvancedCulling();
    bool enableDynamicLOD();
    bool optimizeRenderingForVR();
    
    // Culling management
    bool enableFrustumCulling();
    bool enableOcclusionCulling();
    bool enablePortalCulling();
    bool enableLODCulling();
    bool optimizeCullingForVR();
    
    // Performance optimization
    bool implementAdvancedCullingSystem();
    bool optimizeParticleRendering();
    bool optimizeShadowRendering();
    bool implementDynamicLODSystem();
    bool optimizeRenderingPipeline();
    
    // Graphics API management
    bool detectGraphicsAPI();
    bool switchToVulkan();
    bool switchToOpenGL();
    bool optimizeForGraphicsAPI();
};

} // namespace uevr
