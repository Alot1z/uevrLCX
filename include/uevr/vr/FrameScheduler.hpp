#pragma once

#include <cstdint>
#include <chrono>
#include <memory>
#include <string>

namespace uevr {
namespace vr {

struct FrameTimings {
    float deltaSeconds{0.f};
    float cpuTimeMs{0.f};
    float gpuTimeMs{0.f};
    float presentTimeMs{0.f};
    uint64_t frameIndex{0};
    std::chrono::steady_clock::time_point timestamp{};
};

class IFrameScheduler {
public:
    virtual ~IFrameScheduler() = default;

    virtual bool initialize(int targetFps = 90) = 0;
    virtual void shutdown() = 0;

    virtual bool beginFrame() = 0; // returns false if we should skip this frame
    virtual void endFrame() = 0;

    virtual bool shouldSkipFrame() const = 0;

    virtual void setTargetFramerate(int fps) = 0;
    virtual int getTargetFramerate() const = 0;

    virtual void setFramePacingEnabled(bool enable) = 0;
    virtual bool isFramePacingEnabled() const = 0;

    virtual FrameTimings getLastTimings() const = 0;
    virtual std::string getStatsString() const = 0;
};

std::unique_ptr<IFrameScheduler> CreateFrameScheduler();

} // namespace vr
} // namespace uevr
