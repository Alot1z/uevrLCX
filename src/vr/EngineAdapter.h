#pragma once

#include <string>
#include <memory>
#include <vector>
#include "VRConversionSystem.h"

namespace UEVRLCX {
namespace VR {

class EngineAdapter {
public:
    virtual ~EngineAdapter() = default;

    virtual bool Initialize() = 0;
    virtual bool Setup(const GameInfo& gameInfo) = 0;
    virtual bool InjectVRComponents() = 0;
    virtual bool ConfigureVRRendering() = 0;
    virtual bool SetupInputMapping() = 0;
    virtual bool OptimizeForEngine() = 0;
    virtual void Shutdown() = 0;
    
    // Engine-specific features
    virtual bool LoadEngineSpecificShaders() = 0;
    virtual bool ConfigureEnginePhysics() = 0;
    virtual bool SetupEngineCollision() = 0;
    virtual bool EnableEngineOptimizations() = 0;
    
    // Engine information
    virtual GameEngine GetEngineType() const = 0;
    virtual std::string GetEngineVersion() const = 0;
    virtual bool IsEngineSupported() const = 0;
    virtual std::vector<std::string> GetSupportedFeatures() const = 0;
};

// ============================================================================
// Unreal Engine Adapter
// ============================================================================

class UnrealEngineAdapter : public EngineAdapter {
public:
    UnrealEngineAdapter();
    ~UnrealEngineAdapter() override;

    bool Initialize() override;
    bool Setup(const GameInfo& gameInfo) override;
    bool InjectVRComponents() override;
    bool ConfigureVRRendering() override;
    bool SetupInputMapping() override;
    bool OptimizeForEngine() override;
    void Shutdown() override;
    
    bool LoadEngineSpecificShaders() override;
    bool ConfigureEnginePhysics() override;
    bool SetupEngineCollision() override;
    bool EnableEngineOptimizations() override;
    
    GameEngine GetEngineType() const override { return GameEngine::UNREAL_ENGINE; }
    std::string GetEngineVersion() const override;
    bool IsEngineSupported() const override;
    std::vector<std::string> GetSupportedFeatures() const override;

private:
    bool m_isInitialized;
    std::string m_engineVersion;
    
    // Unreal Engine specific
    bool InjectUEModules();
    bool ConfigureUERendering();
    bool SetupUEInputSystem();
    bool OptimizeUEPerformance();
};

// ============================================================================
// Unity Engine Adapter
// ============================================================================

class UnityEngineAdapter : public EngineAdapter {
public:
    UnityEngineAdapter();
    ~UnityEngineAdapter() override;

    bool Initialize() override;
    bool Setup(const GameInfo& gameInfo) override;
    bool InjectVRComponents() override;
    bool ConfigureVRRendering() override;
    bool SetupInputMapping() override;
    bool OptimizeForEngine() override;
    void Shutdown() override;
    
    bool LoadEngineSpecificShaders() override;
    bool ConfigureEnginePhysics() override;
    bool SetupEngineCollision() override;
    bool EnableEngineOptimizations() override;
    
    GameEngine GetEngineType() const override { return GameEngine::UNITY; }
    std::string GetEngineVersion() const override;
    bool IsEngineSupported() const override;
    std::vector<std::string> GetSupportedFeatures() const override;

private:
    bool m_isInitialized;
    std::string m_engineVersion;
    
    // Unity specific
    bool InjectUnityModules();
    bool ConfigureUnityRendering();
    bool SetupUnityInputSystem();
    bool OptimizeUnityPerformance();
};

// ============================================================================
// Godot Engine Adapter
// ============================================================================

class GodotEngineAdapter : public EngineAdapter {
public:
    GodotEngineAdapter();
    ~GodotEngineAdapter() override;

    bool Initialize() override;
    bool Setup(const GameInfo& gameInfo) override;
    bool InjectVRComponents() override;
    bool ConfigureVRRendering() override;
    bool SetupInputMapping() override;
    bool OptimizeForEngine() override;
    void Shutdown() override;
    
    bool LoadEngineSpecificShaders() override;
    bool ConfigureEnginePhysics() override;
    bool SetupEngineCollision() override;
    bool EnableEngineOptimizations() override;
    
    GameEngine GetEngineType() const override { return GameEngine::GODOT; }
    std::string GetEngineVersion() const override;
    bool IsEngineSupported() const override;
    std::vector<std::string> GetSupportedFeatures() const override;

private:
    bool m_isInitialized;
    std::string m_engineVersion;
    
    // Godot specific
    bool InjectGodotModules();
    bool ConfigureGodotRendering();
    bool SetupGodotInputSystem();
    bool OptimizeGodotPerformance();
};

// ============================================================================
// Custom Engine Adapter
// ============================================================================

class CustomEngineAdapter : public EngineAdapter {
public:
    CustomEngineAdapter();
    ~CustomEngineAdapter() override;

    bool Initialize() override;
    bool Setup(const GameInfo& gameInfo) override;
    bool InjectVRComponents() override;
    bool ConfigureVRRendering() override;
    bool SetupInputMapping() override;
    bool OptimizeForEngine() override;
    void Shutdown() override;
    
    bool LoadEngineSpecificShaders() override;
    bool ConfigureEnginePhysics() override;
    bool SetupEngineCollision() override;
    bool EnableEngineOptimizations() override;
    
    GameEngine GetEngineType() const override { return GameEngine::CUSTOM; }
    std::string GetEngineVersion() const override;
    bool IsEngineSupported() const override;
    std::vector<std::string> GetSupportedFeatures() const override;

private:
    bool m_isInitialized;
    std::string m_engineVersion;
    
    // Custom engine specific
    bool AnalyzeCustomEngine();
    bool InjectCustomModules();
    bool ConfigureCustomRendering();
    bool SetupCustomInputSystem();
    bool OptimizeCustomPerformance();
};

// ============================================================================
// Legacy Engine Adapter
// ============================================================================

class LegacyEngineAdapter : public EngineAdapter {
public:
    LegacyEngineAdapter();
    ~LegacyEngineAdapter() override;

    bool Initialize() override;
    bool Setup(const GameInfo& gameInfo) override;
    bool InjectVRComponents() override;
    bool ConfigureVRRendering() override;
    bool SetupInputMapping() override;
    bool OptimizeForEngine() override;
    void Shutdown() override;
    
    bool LoadEngineSpecificShaders() override;
    bool ConfigureEnginePhysics() override;
    bool SetupEngineCollision() override;
    bool EnableEngineOptimizations() override;
    
    GameEngine GetEngineType() const override { return GameEngine::LEGACY; }
    std::string GetEngineVersion() const override;
    bool IsEngineSupported() const override;
    std::vector<std::string> GetSupportedFeatures() const override;

private:
    bool m_isInitialized;
    std::string m_engineVersion;
    
    // Legacy engine specific
    bool AnalyzeLegacyEngine();
    bool InjectLegacyModules();
    bool ConfigureLegacyRendering();
    bool SetupLegacyInputSystem();
    bool OptimizeLegacyPerformance();
};

// ============================================================================
// Factory Functions
// ============================================================================

std::unique_ptr<EngineAdapter> CreateEngineAdapter(GameEngine engine);

} // namespace VR
} // namespace UEVRLCX
