#pragma once

#include <string>
#include <memory>
#include <vector>
#include "VRConversionSystem.h"

namespace UEVRLCX {
namespace VR {

class Depth3DExtractor {
public:
    Depth3DExtractor();
    ~Depth3DExtractor();

    bool Initialize();
    bool Setup(const GameInfo& gameInfo);
    bool Enable(bool enable);
    bool ExtractDepthBuffer();
    bool ProcessDepthData();
    
    // Depth3D-specific features
    bool SetDepthAccuracy(double accuracy);
    bool EnableRealTimeExtraction(bool enable);
    bool SetExtractionMethod(const std::string& method);
    double GetDepthAccuracy() const;

private:
    bool m_isInitialized;
    bool m_isEnabled;
    bool m_realTimeExtraction;
    double m_depthAccuracy;
    std::string m_extractionMethod;
    
    // Depth3D integration
    void* m_depth3DHandle;
    void* m_depthBuffer;
    size_t m_bufferSize;
};

} // namespace VR
} // namespace UEVRLCX
