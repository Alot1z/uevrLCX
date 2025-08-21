#pragma once

#include <memory>
#include <cstdint>
#include "uevr/vr/StereoRenderer.hpp"
#include "uevr/vr/FrameScheduler.hpp"

namespace uevr { namespace adapters { namespace unity {

class UnityAdapter {
public:
    UnityAdapter() = default;
    ~UnityAdapter() = default;

    bool initialize();
    void shutdown();

    // Built-in pipeline
    void onPreCull(void* unityCamera);
    void onPostRender(void* unityCamera);

    // SRP pipeline
    void beginCameraRendering(void* unityCamera);
    void endCameraRendering(void* unityCamera);

    void setXRDisabled(bool disabled);

private:
    std::unique_ptr<vr::IStereoRenderer> m_renderer;
    std::unique_ptr<vr::IFrameScheduler> m_scheduler;
    bool m_xrDisabled{true};
    uint64_t m_frameIndex{0};
};

}}} // namespace uevr::adapters::unity
