#pragma once

#include <string>

namespace UEVR {
namespace Core {

// Engine detection functions
bool detectEngineForProcess(DWORD processId);
std::string getDetectedEngine();
std::string getDetectedAdapter();

// Engine types
enum class EngineType {
    UNKNOWN = 0,
    RE_ENGINE,
    REDENGINE4,
    REDENGINE3,
    MT_FRAMEWORK,
    UNREAL_ENGINE_4,
    UNREAL_ENGINE_5,
    UNITY_ENGINE,
    CRYENGINE,
    IDTECH6,
    IDTECH7,
    SOURCE_ENGINE,
    SOURCE2,
    CUSTOM_ENGINE
};

// Engine detection result
struct EngineDetectionResult {
    EngineType type;
    std::string version;
    std::string adapter_path;
    bool success;
};

} // namespace Core
} // namespace UEVR


