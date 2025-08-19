#include "mt_framework_factory.hpp"
#include "mt_framework_adapter.hpp"
#include "mt_framework_types.hpp"
#include <vector>
#include <algorithm>

namespace uevr {
namespace MTFramework {

std::unique_ptr<ICrossEngineAdapter> MTFrameworkFactory::createAdapter() {
    return createAdapterWithConfig("");
}

std::unique_ptr<ICrossEngineAdapter> MTFrameworkFactory::createAdapterWithConfig(const std::string& configPath) {
    if (!isSupported()) {
        return nullptr;
    }

    try {
        auto adapter = std::make_unique<MTFrameworkAdapter>();
        
        if (configPath.empty()) {
            // Use default configuration
            if (!adapter->loadConfiguration("")) {
                return nullptr;
            }
        } else {
            // Load from specified configuration file
            if (!adapter->loadConfiguration(configPath)) {
                return nullptr;
            }
        }

        return adapter;
    } catch (const std::exception&) {
        return nullptr;
    }
}

std::unique_ptr<ICrossEngineAdapter> MTFrameworkFactory::createAdapterForGame(const std::string& gameName) {
    if (!isGameSupported(gameName)) {
        return nullptr;
    }

    // Create adapter with game-specific configuration
    std::string configPath = "config/" + gameName + ".cfg";
    return createAdapterWithConfig(configPath);
}

bool MTFrameworkFactory::isSupported() {
    return validateSystemRequirements();
}

std::vector<std::string> MTFrameworkFactory::getSupportedGames() {
    return {
        Games::MONSTER_HUNTER_WORLD,
        Games::DEVIL_MAY_CRY_5,
        Games::RESIDENT_EVIL_6,
        Games::DRAGONS_DOGMA
    };
}

std::string MTFrameworkFactory::getVersion() {
    return "1.0.0";
}

std::string MTFrameworkFactory::getDescription() {
    return "MT Framework VR Adapter Factory - Provides VR support for Capcom MT Framework games";
}

// Private helper methods

bool MTFrameworkFactory::validateSystemRequirements() {
    // Check if DirectX is available
    // Check if MT Framework runtime is available
    // Check system capabilities
    
    // For now, assume supported
    return true;
}

std::string MTFrameworkFactory::detectGameType() {
    // Implementation would detect the running game type
    // For now, return Monster Hunter World as default
    return Games::MONSTER_HUNTER_WORLD;
}

bool MTFrameworkFactory::loadGameConfiguration(const std::string& gameName) {
    // Implementation would load game-specific configuration
    // For now, return true as placeholder
    return true;
}

} // namespace MTFramework
} // namespace uevr
