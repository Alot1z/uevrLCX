#pragma once

#include <string>

namespace UEVR {
namespace Core {

// Adapter system management
bool initializeAdapterSystem();
bool loadEngineAdapter(const std::string& engine_name);
bool hotSwapEngineAdapter(const std::string& new_engine);
std::string getCurrentEngineAdapter();
bool isAdapterSystemReady();

// Adapter interface
class IEngineAdapter {
public:
    virtual ~IEngineAdapter() = default;
    virtual bool Initialize() = 0;
    virtual void Cleanup() = 0;
    virtual bool IsCompatible() const = 0;
    virtual std::string GetEngineName() const = 0;
    virtual std::string GetVersion() const = 0;
};

} // namespace Core
} // namespace UEVR


