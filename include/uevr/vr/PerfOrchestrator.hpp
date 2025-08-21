#pragma once

#include <memory>
#include <string>
#include <unordered_map>

namespace uevr {
namespace vr {

struct PerfTargets {
    int targetFPS{90};
    float maxLatencyMs{11.0f};
    float minRenderScale{0.7f};
    float maxRenderScale{1.5f};
    bool enableVRS{true};
    bool enableFoveated{true};
    bool enableAsyncReprojection{true};
};

struct PerfSnapshot {
    float currentFPS{0.f};
    float frameTimeCPU{0.f};
    float frameTimeGPU{0.f};
    float latencyMs{0.f};
    float renderScale{1.f};
    std::unordered_map<std::string, float> extra; // extensible metrics
};

class IPerfOrchestrator {
public:
    virtual ~IPerfOrchestrator() = default;

    virtual bool initialize(const PerfTargets& targets) = 0;
    virtual void shutdown() = 0;

    virtual void setTargets(const PerfTargets& targets) = 0;
    virtual PerfTargets getTargets() const = 0;

    // Feed per-frame metrics and retrieve adjustment hints
    virtual void submitSnapshot(const PerfSnapshot& snapshot) = 0;

    // Returns new recommended render scale in [min, max]
    virtual float recommendRenderScale() const = 0;

    // Returns whether to enable/disable features dynamically
    virtual bool shouldEnableFoveated() const = 0;
    virtual bool shouldEnableVRS() const = 0;
    virtual bool shouldDropFrame() const = 0;
};

std::unique_ptr<IPerfOrchestrator> CreatePerfOrchestrator();

} // namespace vr
} // namespace uevr
