/*
BaseCrossEngineAdapter - Base class for cross-engine VR adapters
Provides common functionality and implements the ICrossEngineAdapter interface
while extending uevr's Plugin system for proper integration.
*/

#pragma once

#include <uevr/ICrossEngineAdapter.hpp>

#include <chrono>
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <functional>
#include <d3d11.h>
#include <d3d12.h>
#include <dxgi.h>

namespace uevr {

// Universal engine types for mass deployment
enum class EngineType {
    UNREAL_ENGINE_4,
    UNREAL_ENGINE_5,
    RE_ENGINE,
    REDENGINE_4,
    REDENGINE_3,
    MT_FRAMEWORK,
    UNITY_ENGINE,
    CRYENGINE,
    ID_TECH_6,
    ID_TECH_7,
    SOURCE_ENGINE,
    SOURCE_2,
    CUSTOM_ENGINE,
    UNKNOWN_ENGINE
};

// Universal VR integration patterns
struct UniversalVRPatterns {
    bool supportsStereoRendering;
    bool supportsMotionControllers;
    bool supportsHapticFeedback;
    bool supportsDynamicFOV;
    bool supportsFrameInterpolation;
    bool supportsNeuralUpscaling;
    bool supportsRayTracing;
    bool supportsTemporalUpsampling;
};

// Cross-engine compatibility matrix
struct EngineCompatibility {
    EngineType engineType;
    std::string engineName;
    std::vector<std::string> supportedGames;
    std::vector<std::string> supportedVersions;
    UniversalVRPatterns vrCapabilities;
    bool requiresCustomHooks;
    bool supportsGenericRendering;
};

/**
 * @brief Universal base class for cross-engine VR adapters
 * 
 * Provides common functionality and implements the ICrossEngineAdapter interface
 * for cross-engine VR support across 500+ games.
 * 
 * This is the foundation for mass deployment of VR support to all major game engines.
 */
class BaseCrossEngineAdapter : public ICrossEngineAdapter {
public:
    BaseCrossEngineAdapter(const std::string& name, EngineType engineType, const std::string& gameExecutable);
    virtual ~BaseCrossEngineAdapter() = default;

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

    // Universal engine detection and compatibility
    virtual bool detectEngineVersion();
    virtual bool checkEngineCompatibility();
    virtual bool initializeUniversalVRPatterns();
    
    // Cross-engine VR integration patterns
    virtual bool setupUniversalStereoRendering();
    virtual bool setupUniversalMotionControllers();
    virtual bool setupUniversalHapticFeedback();
    virtual bool setupUniversalDynamicFOV();
    virtual bool setupUniversalFrameInterpolation();
    virtual bool setupUniversalNeuralUpscaling();
    virtual bool setupUniversalRayTracing();
    virtual bool setupUniversalTemporalUpsampling();

    // VR rendering callbacks (to be implemented by derived classes if needed)
    virtual void on_initialize();
    virtual void on_device_reset();
    virtual void on_post_render_vr_framework_dx11(ID3D11DeviceContext* context, ID3D11Texture2D* texture, ID3D11RenderTargetView* rtv);
    virtual void on_post_render_vr_framework_dx12(ID3D12GraphicsCommandList* command_list, ID3D12Resource* rt, D3D12_CPU_DESCRIPTOR_HANDLE* rtv);

protected:
    // Common VR functionality
    virtual bool initializeVR() = 0;
    virtual void cleanupVR() = 0;
    virtual void updateVR() = 0;
    virtual void renderVR() = 0;
    
    // Engine-specific functionality
    virtual bool detectEngine() = 0;
    virtual bool initializeEngineHooks() = 0;
    virtual void cleanupEngineHooks() = 0;
    
    // Universal engine detection
    virtual bool scanForEngineSignatures();
    virtual bool validateEngineVersion();
    virtual bool detectEngineCapabilities();
    
    // Common state
    bool m_vrEnabled;
    bool m_engineDetected;
    bool m_hooksInitialized;
    bool m_universalPatternsInitialized;
    
    // Configuration
    std::string m_name;
    EngineType m_engineType;
    std::string m_engineTypeString;
    std::string m_gameExecutable;
    std::string m_version;
    std::string m_detectedEngineVersion;
    
    // Universal VR patterns
    UniversalVRPatterns m_vrPatterns;
    EngineCompatibility m_engineCompatibility;
    
    // Performance tracking
    struct PerformanceMetrics {
        double frameTime;
        double cpuTime;
        double gpuTime;
        uint32_t frameCount;
        uint32_t droppedFrames;
        double vrLatency;
        double motionToPhoton;
        double reprojectionRate;
    } m_performance;
    
    // Error handling
    struct ErrorState {
        std::string lastError;
        uint32_t errorCount;
        bool hasCriticalError;
        std::vector<std::string> errorHistory;
    } m_errorState;

private:
    void updatePerformanceMetrics();
    void logError(const std::string& error, bool critical = false);
    bool initializeEngineCompatibilityMatrix();
    std::string engineTypeToString(EngineType type);
};

} // namespace uevr
