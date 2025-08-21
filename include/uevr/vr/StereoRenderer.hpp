#pragma once

#include <memory>
#include <utility>
#include <cstdint>

namespace uevr {
namespace vr {

enum class StereoMode {
    GEOMETRY_STEREO = 0,
    DEPTH_STEREO,
    SINGLE_PASS,
    MULTI_PASS
};

enum class GraphicsAPI {
    DX11 = 0,
    DX12,
    VULKAN,
    OPENGL,
    AUTO
};

struct EyeViewport { uint32_t x{0}, y{0}, width{0}, height{0}; };

class IStereoRenderer {
public:
    virtual ~IStereoRenderer() = default;

    virtual bool initialize(GraphicsAPI api, StereoMode mode) = 0;
    virtual void shutdown() = 0;

    virtual void setMode(StereoMode mode) = 0;
    virtual StereoMode getMode() const = 0;

    // Begin/end stereo frame
    virtual bool beginFrame(uint64_t frameIndex) = 0;
    virtual void endFrame() = 0;

    // Render hooks per eye (opaque handles to avoid dependencies)
    virtual bool renderEye(int eyeIndex, void* sceneHandle, void* renderTarget, const EyeViewport& vp) = 0;

    // Optional helpers
    virtual void setNearFar(float nearZ, float farZ) = 0;
    virtual void setIPD(float meters) = 0;
};

std::unique_ptr<IStereoRenderer> CreateStereoRenderer();

} // namespace vr
} // namespace uevr
