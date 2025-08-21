#include "unity_adapter.hpp"

namespace uevr { namespace adapters { namespace unity {

bool UnityAdapter::initialize() {
    m_renderer = vr::CreateStereoRenderer();
    m_scheduler = vr::CreateFrameScheduler();
    if (!m_renderer || !m_scheduler) return false;
    if (!m_renderer->initialize(vr::GraphicsAPI::AUTO, vr::StereoMode::MULTI_PASS)) return false;
    if (!m_scheduler->initialize(90)) return false;
    return true;
}

void UnityAdapter::shutdown() {
    if (m_renderer) m_renderer->shutdown();
    if (m_scheduler) m_scheduler->shutdown();
    m_renderer.reset();
    m_scheduler.reset();
}

void UnityAdapter::onPreCull(void* /*unityCamera*/) {
    if (!m_scheduler || !m_renderer) return;
    if (!m_scheduler->beginFrame()) return;
    m_renderer->beginFrame(++m_frameIndex);
}

void UnityAdapter::onPostRender(void* /*unityCamera*/) {
    if (!m_renderer || !m_scheduler) return;
    m_renderer->endFrame();
    m_scheduler->endFrame();
}

void UnityAdapter::beginCameraRendering(void* unityCamera) {
    onPreCull(unityCamera);
}

void UnityAdapter::endCameraRendering(void* unityCamera) {
    onPostRender(unityCamera);
}

void UnityAdapter::setXRDisabled(bool disabled) { m_xrDisabled = disabled; }

}}} // namespace
