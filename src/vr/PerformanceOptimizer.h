#pragma once

#include <string>
#include <memory>
#include <vector>
#include "VRConversionSystem.h"

namespace UEVRLCX {
namespace VR {

class PerformanceOptimizer {
public:
    PerformanceOptimizer();
    ~PerformanceOptimizer();

    bool Initialize();
    bool Optimize();
    bool EnableFoveatedRendering(bool enable);
    bool EnableAsyncTimewarp(bool enable);
    bool EnableMultiResolutionShading(bool enable);
    bool SetTargetFrameRate(double frameRate);
    bool SetMaxLatency(double latency);
    
    // Performance optimization features
    bool EnableDynamicResolution(bool enable);
    bool EnableAdaptiveQuality(bool enable);
    bool SetPerformanceProfile(const std::string& profile);
    double GetCurrentFrameRate() const;
    double GetCurrentLatency() const;
    double GetPerformanceOverhead() const;

private:
    bool m_isInitialized;
    bool m_foveatedRenderingEnabled;
    bool m_asyncTimewarpEnabled;
    bool m_multiResolutionShadingEnabled;
    bool m_dynamicResolutionEnabled;
    bool m_adaptiveQualityEnabled;
    double m_targetFrameRate;
    double m_maxLatency;
    std::string m_performanceProfile;
    
    // Performance monitoring
    double m_currentFrameRate;
    double m_currentLatency;
    double m_performanceOverhead;
    
    // Performance optimization systems
    void* m_foveatedRenderer;
    void* m_timewarpProcessor;
    void* m_resolutionScaler;
    void* m_qualityManager;
};

} // namespace VR
} // namespace UEVRLCX
