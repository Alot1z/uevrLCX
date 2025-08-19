#include "mt_framework_factory.hpp"
#include <windows.h>
#include <string>
#include <algorithm>

namespace uevr {
namespace MTFramework {

MTFrameworkAdapterFactory::MTFrameworkAdapterFactory() {
    // Factory initialization
}

MTFrameworkAdapterFactory::~MTFrameworkAdapterFactory() {
    // Factory cleanup
}

std::shared_ptr<ICrossEngineAdapter> MTFrameworkAdapterFactory::createAdapter(const std::string& gameExecutable) {
    // Check if we can create an adapter for this game
    if (!canCreateAdapter(gameExecutable)) {
        return nullptr;
    }
    
    // Create Monster Hunter World adapter
    if (isMonsterHunterWorld(gameExecutable)) {
        return std::make_shared<MonsterHunterWorldAdapter>();
    }
    
    // Future: Add support for other MT Framework games
    return nullptr;
}

bool MTFrameworkAdapterFactory::canCreateAdapter(const std::string& gameExecutable) const {
    // Check if this is an MT Framework game
    return isMTFrameworkGame(gameExecutable);
}

std::string MTFrameworkAdapterFactory::getSupportedEngineType() const {
    return "MT-Framework";
}

bool MTFrameworkAdapterFactory::isMonsterHunterWorld(const std::string& gameExecutable) const {
    // Check for Monster Hunter World executable
    std::string lowerExe = gameExecutable;
    std::transform(lowerExe.begin(), lowerExe.end(), lowerExe.begin(), ::tolower);
    
    return lowerExe.find("monsterhunterworld.exe") != std::string::npos ||
           lowerExe.find("monsterhunterworld") != std::string::npos;
}

bool MTFrameworkAdapterFactory::isMTFrameworkGame(const std::string& gameExecutable) const {
    // Check if this is an MT Framework game
    if (isMonsterHunterWorld(gameExecutable)) {
        return true;
    }
    
    // Future: Add detection for other MT Framework games
    // Examples: Resident Evil 6, Dragon's Dogma, etc.
    
    return false;
}

bool MTFrameworkAdapterFactory::detectMTFramework() const {
    // Check for MT Framework DLL or other indicators
    HMODULE mtFrameworkModule = GetModuleHandleA("MTFramework.dll");
    if (mtFrameworkModule) {
        return true;
    }
    
    // Check for other MT Framework indicators
    // This could include checking for specific memory patterns,
    // registry entries, or other engine-specific signatures
    
    return false;
}

} // namespace MTFramework
} // namespace uevr
