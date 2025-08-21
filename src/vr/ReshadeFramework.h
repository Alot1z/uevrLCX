#pragma once

#include <string>
#include <memory>
#include <vector>
#include "VRConversionSystem.h"

namespace UEVRLCX {
namespace VR {

class ReshadeFramework {
public:
    ReshadeFramework();
    ~ReshadeFramework();

    bool Initialize();
    bool Configure(const VRConfig& vrConfig);
    bool LoadShaders();
    bool InjectShaders();
    bool EnableShader(const std::string& shaderName);
    bool DisableShader(const std::string& shaderName);
    
    // Reshade-specific features
    bool SetShaderQuality(int quality);
    bool EnablePostProcessing(bool enable);
    bool LoadCustomShaders(const std::string& shaderPath);
    std::vector<std::string> GetAvailableShaders() const;

private:
    bool m_isInitialized;
    bool m_postProcessingEnabled;
    int m_shaderQuality;
    std::vector<std::string> m_enabledShaders;
    std::vector<std::string> m_availableShaders;
    
    // Reshade integration
    void* m_reshadeHandle;
    void* m_shaderManager;
};

} // namespace VR
} // namespace UEVRLCX
