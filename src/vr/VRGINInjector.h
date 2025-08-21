#pragma once

#include <string>
#include <memory>
#include <vector>

namespace UEVRLCX {
namespace VR {

class VRGINInjector {
public:
    VRGINInjector();
    ~VRGINInjector();

    bool Initialize();
    bool Inject(const std::string& gamePath);
    bool Uninject();
    bool IsInjected() const;
    
    // VRGIN-specific features
    bool EnableStereoRendering(bool enable);
    bool SetIPD(float ipd);
    bool SetFOV(float fov);
    bool EnableMotionControllers(bool enable);
    bool EnableRoomScale(bool enable);

private:
    bool m_isInitialized;
    bool m_isInjected;
    std::string m_currentGamePath;
    
    // VRGIN integration
    void* m_vrginHandle;
    void* m_injectionHandle;
};

} // namespace VR
} // namespace UEVRLCX
