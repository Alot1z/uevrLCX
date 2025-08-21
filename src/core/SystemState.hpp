#pragma once

namespace UEVRLCX {

enum class SystemState {
    CREATED,
    INITIALIZING,
    RUNNING,
    STOPPED,
    ERROR,
    SHUTDOWN
};

enum class SecurityLevel {
    LOW,
    MEDIUM,
    HIGH,
    CRITICAL
};

enum class PerformanceLevel {
    LOW,
    MEDIUM,
    HIGH,
    ULTRA
};

} // namespace UEVRLCX
