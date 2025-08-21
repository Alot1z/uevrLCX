#pragma once

#include <string>
#include <memory>
#include <vector>
#include "VRConversionSystem.h"

namespace UEVRLCX {
namespace VR {

class FoveatedRenderer {
public:
    FoveatedRenderer();
    ~FoveatedRenderer();

    bool Initialize();
    bool Enable(bool enable);
    bool SetFoveationLevel(int level);
    bool EnableEyeTracking(bool enable);
    bool SetFoveationRadius(float radius);
    bool SetPeripheralQuality(float quality);
    
    // Foveated rendering features
    bool EnableDynamicFoveation(bool enable);
    bool SetFoveationProfile(const std::string& profile);
    bool CalibrateEyeTracking();
    float GetFoveationRadius() const;
    float GetPeripheralQuality() const;
    bool IsEyeTrackingEnabled() const;

private:
    bool m_isInitialized;
    bool m_enabled;
    bool m_eyeTrackingEnabled;
    bool m_dynamicFoveationEnabled;
    int m_foveationLevel;
    float m_foveationRadius;
    float m_peripheralQuality;
    std::string m_foveationProfile;
    
    // Foveated rendering system
    void* m_foveationEngine;
    void* m_eyeTracker;
    void* m_qualityMapper;
};

} // namespace VR
} // namespace UEVRLCX
