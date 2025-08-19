#pragma once

#include "../../common/BaseCrossEngineAdapter.hpp"
#include <memory>
#include <unordered_map>
#include <vector>
#include <string>

namespace uevr {

/**
 * @brief Universal Unity Engine VR Adapter
 * 
 * Supports Unity 2019.4 LTS through Unity 6 for thousands of indie titles including:
 * - Ori series, Cuphead, Cities: Skylines
 * - IL2CPP and Mono runtime hooking
 * - Component-based VR adaptation, universal shader replacement
 * - Cross-platform compatibility
 */
class UnityEngineAdapter : public BaseCrossEngineAdapter {
public:
    UnityEngineAdapter(const std::string& name, const std::string& engineType, const std::string& gameExecutable);
    virtual ~UnityEngineAdapter() = default;

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
    // Unity Engine specific functionality
    bool detectUnityVersion();
    bool initializeIL2CPPHooks();
    bool initializeMonoHooks();
    bool initializeComponentHooks();
    bool initializeShaderHooks();
    
    // Runtime hooking
    bool hookIL2CPPRuntime();
    bool hookMonoRuntime();
    bool hookUnityPlayer();
    bool hookGameAssembly();
    
    // Component system hooks
    bool hookComponentSystem();
    bool hookTransformSystem();
    bool hookRenderingSystem();
    bool hookInputSystem();
    
    // Shader and material system
    bool hookShaderSystem();
    bool hookMaterialSystem();
    bool hookTextureSystem();
    bool implementUniversalShaders();
    
    // VR-specific features
    bool setupVRStereoRendering();
    bool implementDynamicFOV();
    bool setupHapticFeedback();
    bool implementMotionControllers();
    bool setupComponentBasedVR();
    
    // Engine version detection
    enum class UnityVersion {
        UNKNOWN,
        UNITY_2019_4_LTS,
        UNITY_2020_LTS,
        UNITY_2021_LTS,
        UNITY_2022_LTS,
        UNITY_6_0_PLUS
    };
    
    UnityVersion m_unityVersion;
    std::string m_engineVersion;
    
    // Runtime type detection
    enum class RuntimeType {
        UNKNOWN,
        MONO,
        IL2CPP
    };
    
    RuntimeType m_runtimeType;
    
    // Hook addresses and function pointers
    struct UnityOffsets {
        uintptr_t unityPlayer;
        uintptr_t gameAssembly;
        uintptr_t il2cppRuntime;
        uintptr_t monoRuntime;
        uintptr_t componentSystem;
        uintptr_t transformSystem;
        uintptr_t renderingSystem;
        uintptr_t inputSystem;
        uintptr_t shaderSystem;
        uintptr_t materialSystem;
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
        bool componentBasedVREnabled;
    } m_vrState;
    
    // Performance metrics
    struct PerformanceState {
        uint32_t drawCalls;
        uint32_t triangles;
        uint32_t vertices;
        uint32_t gameObjects;
        uint32_t components;
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
        RuntimeType runtimeType;
        bool supportsURP;
        bool supportsHDRP;
        bool supportsBuiltInRP;
        std::vector<std::string> requiredHooks;
        std::vector<std::string> optionalHooks;
        std::vector<std::string> supportedShaders;
    } m_gameConfig;
    
    // Hook management
    std::vector<void*> m_activeHooks;
    bool m_hooksInitialized;
    
    // Component tracking
    struct ComponentInfo {
        std::string componentName;
        std::string gameObjectName;
        bool isVREnabled;
        bool hasCustomVRBehavior;
    };
    
    std::vector<ComponentInfo> m_trackedComponents;
    
    // Error handling
    void logEngineError(const std::string& error, bool critical = false);
    bool validateEngineState();
    
    // Utility functions
    std::string getUnityVersionString() const;
    bool isUnity2022Plus() const;
    bool supportsURP() const;
    bool supportsHDRP() const;
    bool isIL2CPP() const;
    bool isMono() const;
    
    // Component management
    bool registerVRComponent(const std::string& componentName);
    bool enableVRForComponent(const std::string& componentName);
    bool disableVRForComponent(const std::string& componentName);
    
    // Shader management
    bool replaceShader(const std::string& originalShader, const std::string& vrShader);
    bool injectVRShaders();
    bool optimizeShadersForVR();
};

} // namespace uevr
