#include "EngineAdapter.h"
#include "LoggingSystem.h"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

namespace uevrLCX {
namespace core {

class EngineAdapterImpl {
private:
    std::shared_ptr<LoggingSystem> logger;
    bool isInitialized;
    std::map<std::string, VRConfiguration> engineConfigs;

public:
    EngineAdapterImpl() : isInitialized(false) {
        logger = std::make_shared<LoggingSystem>();
        logger->log(LogLevel::INFO, "EngineAdapter initialized");
    }

    bool initialize() {
        try {
            if (isInitialized) {
                logger->log(LogLevel::WARNING, "EngineAdapter already initialized");
                return true;
            }

            logger->log(LogLevel::INFO, "Initializing EngineAdapter");

            // Initialize engine configurations
            initializeEngineConfigs();

            isInitialized = true;
            logger->log(LogLevel::INFO, "EngineAdapter initialized successfully");
            return true;

        } catch (const std::exception& e) {
            logger->log(LogLevel::ERROR, "Error initializing EngineAdapter: " + std::string(e.what()));
            return false;
        }
    }

    bool injectVR(const std::string& gamePath, const VRConfiguration& config) {
        try {
            if (!isInitialized) {
                logger->log(LogLevel::ERROR, "EngineAdapter not initialized");
                return false;
            }

            logger->log(LogLevel::INFO, "Injecting VR into: " + gamePath);

            // Detect engine type
            std::string engineType = detectEngineType(gamePath);
            if (engineType.empty()) {
                logger->log(LogLevel::ERROR, "Could not detect engine type for: " + gamePath);
                return false;
            }

            // Inject VR based on engine type
            bool success = injectVRForEngine(engineType, gamePath, config);
            if (success) {
                logger->log(LogLevel::INFO, "VR injection successful for: " + gamePath);
            } else {
                logger->log(LogLevel::ERROR, "VR injection failed for: " + gamePath);
            }

            return success;

        } catch (const std::exception& e) {
            logger->log(LogLevel::ERROR, "Error injecting VR: " + std::string(e.what()));
            return false;
        }
    }

    bool configureVR(const std::string& gamePath, const VRConfiguration& config) {
        try {
            if (!isInitialized) {
                logger->log(LogLevel::ERROR, "EngineAdapter not initialized");
                return false;
            }

            logger->log(LogLevel::INFO, "Configuring VR for: " + gamePath);

            std::string engineType = detectEngineType(gamePath);
            if (engineType.empty()) {
                logger->log(LogLevel::ERROR, "Could not detect engine type for: " + gamePath);
                return false;
            }

            bool success = configureVRForEngine(engineType, gamePath, config);
            if (success) {
                logger->log(LogLevel::INFO, "VR configuration successful for: " + gamePath);
            } else {
                logger->log(LogLevel::ERROR, "VR configuration failed for: " + gamePath);
            }

            return success;

        } catch (const std::exception& e) {
            logger->log(LogLevel::ERROR, "Error configuring VR: " + std::string(e.what()));
            return false;
        }
    }

    bool removeVR(const std::string& gamePath) {
        try {
            if (!isInitialized) {
                logger->log(LogLevel::ERROR, "EngineAdapter not initialized");
                return false;
            }

            logger->log(LogLevel::INFO, "Removing VR from: " + gamePath);

            std::string engineType = detectEngineType(gamePath);
            if (engineType.empty()) {
                logger->log(LogLevel::ERROR, "Could not detect engine type for: " + gamePath);
                return false;
            }

            bool success = removeVRForEngine(engineType, gamePath);
            if (success) {
                logger->log(LogLevel::INFO, "VR removal successful for: " + gamePath);
            } else {
                logger->log(LogLevel::ERROR, "VR removal failed for: " + gamePath);
            }

            return success;

        } catch (const std::exception& e) {
            logger->log(LogLevel::ERROR, "Error removing VR: " + std::string(e.what()));
            return false;
        }
    }

    std::vector<std::string> getSupportedEngines() {
        try {
            std::vector<std::string> engines;
            for (const auto& pair : engineConfigs) {
                engines.push_back(pair.first);
            }
            return engines;

        } catch (const std::exception& e) {
            logger->log(LogLevel::ERROR, "Error getting supported engines: " + std::string(e.what()));
            return std::vector<std::string>();
        }
    }

    bool isEngineSupported(const std::string& engineType) {
        return engineConfigs.find(engineType) != engineConfigs.end();
    }

    VRConfiguration getDefaultConfig(const std::string& engineType) {
        try {
            auto it = engineConfigs.find(engineType);
            if (it != engineConfigs.end()) {
                return it->second;
            }

            // Return default configuration
            VRConfiguration defaultConfig;
            defaultConfig.resolution = {1920, 1080};
            defaultConfig.refreshRate = 90;
            defaultConfig.fov = 110.0f;
            defaultConfig.enablePhysics = true;
            defaultConfig.enableHaptics = true;
            return defaultConfig;

        } catch (const std::exception& e) {
            logger->log(LogLevel::ERROR, "Error getting default config: " + std::string(e.what()));
            return VRConfiguration{};
        }
    }

    bool shutdown() {
        try {
            if (!isInitialized) {
                logger->log(LogLevel::WARNING, "EngineAdapter not initialized");
                return true;
            }

            logger->log(LogLevel::INFO, "Shutting down EngineAdapter");

            isInitialized = false;
            logger->log(LogLevel::INFO, "EngineAdapter shut down successfully");
            return true;

        } catch (const std::exception& e) {
            logger->log(LogLevel::ERROR, "Error shutting down EngineAdapter: " + std::string(e.what()));
            return false;
        }
    }

private:
    void initializeEngineConfigs() {
        try {
            logger->log(LogLevel::INFO, "Initializing engine configurations");

            // Unreal Engine configuration
            VRConfiguration unrealConfig;
            unrealConfig.resolution = {2560, 1440};
            unrealConfig.refreshRate = 90;
            unrealConfig.fov = 120.0f;
            unrealConfig.enablePhysics = true;
            unrealConfig.enableHaptics = true;
            engineConfigs["Unreal Engine"] = unrealConfig;

            // Unity configuration
            VRConfiguration unityConfig;
            unityConfig.resolution = {1920, 1080};
            unityConfig.refreshRate = 90;
            unityConfig.fov = 110.0f;
            unityConfig.enablePhysics = true;
            unityConfig.enableHaptics = true;
            engineConfigs["Unity"] = unityConfig;

            // Godot configuration
            VRConfiguration godotConfig;
            godotConfig.resolution = {1920, 1080};
            godotConfig.refreshRate = 90;
            godotConfig.fov = 110.0f;
            godotConfig.enablePhysics = true;
            godotConfig.enableHaptics = true;
            engineConfigs["Godot"] = godotConfig;

            // Custom engine configuration
            VRConfiguration customConfig;
            customConfig.resolution = {1920, 1080};
            customConfig.refreshRate = 90;
            customConfig.fov = 110.0f;
            customConfig.enablePhysics = true;
            customConfig.enableHaptics = true;
            engineConfigs["Custom Engine"] = customConfig;

            logger->log(LogLevel::INFO, "Engine configurations initialized");

        } catch (const std::exception& e) {
            logger->log(LogLevel::ERROR, "Error initializing engine configs: " + std::string(e.what()));
        }
    }

    std::string detectEngineType(const std::string& gamePath) {
        try {
            // Check for Unreal Engine
            if (hasUnrealEngineFiles(gamePath)) {
                return "Unreal Engine";
            }

            // Check for Unity
            if (hasUnityFiles(gamePath)) {
                return "Unity";
            }

            // Check for Godot
            if (hasGodotFiles(gamePath)) {
                return "Godot";
            }

            // Check for custom engine
            if (hasCustomEngineFiles(gamePath)) {
                return "Custom Engine";
            }

            return "";

        } catch (const std::exception& e) {
            logger->log(LogLevel::ERROR, "Error detecting engine type: " + std::string(e.what()));
            return "";
        }
    }

    bool hasUnrealEngineFiles(const std::string& gamePath) {
        std::vector<std::string> unrealFiles = {
            "Engine.ini", "Game.ini", "DefaultEngine.ini",
            "UE4Editor.exe", "UE5Editor.exe"
        };

        return hasAnyFiles(gamePath, unrealFiles);
    }

    bool hasUnityFiles(const std::string& gamePath) {
        std::vector<std::string> unityFiles = {
            "UnityPlayer.dll", "Assembly-CSharp.dll",
            "UnityCrashHandler64.exe"
        };

        return hasAnyFiles(gamePath, unityFiles);
    }

    bool hasGodotFiles(const std::string& gamePath) {
        std::vector<std::string> godotFiles = {
            "project.godot", "export_presets.cfg",
            "godot.exe", "Godot.exe"
        };

        return hasAnyFiles(gamePath, godotFiles);
    }

    bool hasCustomEngineFiles(const std::string& gamePath) {
        std::vector<std::string> customFiles = {
            "engine.dll", "game_engine.dll",
            "custom_engine.dll"
        };

        return hasAnyFiles(gamePath, customFiles);
    }

    bool hasAnyFiles(const std::string& gamePath, const std::vector<std::string>& files) {
        try {
            std::filesystem::path path(gamePath);
            for (const auto& file : files) {
                std::filesystem::path filePath = path / file;
                if (std::filesystem::exists(filePath)) {
                    return true;
                }
            }
            return false;

        } catch (const std::exception& e) {
            logger->log(LogLevel::ERROR, "Error checking files: " + std::string(e.what()));
            return false;
        }
    }

    bool injectVRForEngine(const std::string& engineType, const std::string& gamePath, const VRConfiguration& config) {
        try {
            logger->log(LogLevel::INFO, "Injecting VR for " + engineType + " engine");

            if (engineType == "Unreal Engine") {
                return injectVRForUnreal(gamePath, config);
            } else if (engineType == "Unity") {
                return injectVRForUnity(gamePath, config);
            } else if (engineType == "Godot") {
                return injectVRForGodot(gamePath, config);
            } else if (engineType == "Custom Engine") {
                return injectVRForCustom(gamePath, config);
            }

            logger->log(LogLevel::ERROR, "Unsupported engine type: " + engineType);
            return false;

        } catch (const std::exception& e) {
            logger->log(LogLevel::ERROR, "Error injecting VR for engine: " + std::string(e.what()));
            return false;
        }
    }

    bool configureVRForEngine(const std::string& engineType, const std::string& gamePath, const VRConfiguration& config) {
        try {
            logger->log(LogLevel::INFO, "Configuring VR for " + engineType + " engine");

            if (engineType == "Unreal Engine") {
                return configureVRForUnreal(gamePath, config);
            } else if (engineType == "Unity") {
                return configureVRForUnity(gamePath, config);
            } else if (engineType == "Godot") {
                return configureVRForGodot(gamePath, config);
            } else if (engineType == "Custom Engine") {
                return configureVRForCustom(gamePath, config);
            }

            logger->log(LogLevel::ERROR, "Unsupported engine type: " + engineType);
            return false;

        } catch (const std::exception& e) {
            logger->log(LogLevel::ERROR, "Error configuring VR for engine: " + std::string(e.what()));
            return false;
        }
    }

    bool removeVRForEngine(const std::string& engineType, const std::string& gamePath) {
        try {
            logger->log(LogLevel::INFO, "Removing VR for " + engineType + " engine");

            if (engineType == "Unreal Engine") {
                return removeVRForUnreal(gamePath);
            } else if (engineType == "Unity") {
                return removeVRForUnity(gamePath);
            } else if (engineType == "Godot") {
                return removeVRForGodot(gamePath);
            } else if (engineType == "Custom Engine") {
                return removeVRForCustom(gamePath);
            }

            logger->log(LogLevel::ERROR, "Unsupported engine type: " + engineType);
            return false;

        } catch (const std::exception& e) {
            logger->log(LogLevel::ERROR, "Error removing VR for engine: " + std::string(e.what()));
            return false;
        }
    }

    // Engine-specific VR injection methods
    bool injectVRForUnreal(const std::string& gamePath, const VRConfiguration& config) {
        logger->log(LogLevel::INFO, "Injecting VR into Unreal Engine game: " + gamePath);
        // Implementation would inject VR hooks into Unreal Engine
        return true;
    }

    bool injectVRForUnity(const std::string& gamePath, const VRConfiguration& config) {
        logger->log(LogLevel::INFO, "Injecting VR into Unity game: " + gamePath);
        // Implementation would inject VR hooks into Unity
        return true;
    }

    bool injectVRForGodot(const std::string& gamePath, const VRConfiguration& config) {
        logger->log(LogLevel::INFO, "Injecting VR into Godot game: " + gamePath);
        // Implementation would inject VR hooks into Godot
        return true;
    }

    bool injectVRForCustom(const std::string& gamePath, const VRConfiguration& config) {
        logger->log(LogLevel::INFO, "Injecting VR into Custom Engine game: " + gamePath);
        // Implementation would inject VR hooks into custom engine
        return true;
    }

    // Engine-specific VR configuration methods
    bool configureVRForUnreal(const std::string& gamePath, const VRConfiguration& config) {
        logger->log(LogLevel::INFO, "Configuring VR for Unreal Engine game: " + gamePath);
        // Implementation would configure VR settings
        return true;
    }

    bool configureVRForUnity(const std::string& gamePath, const VRConfiguration& config) {
        logger->log(LogLevel::INFO, "Configuring VR for Unity game: " + gamePath);
        // Implementation would configure VR settings
        return true;
    }

    bool configureVRForGodot(const std::string& gamePath, const VRConfiguration& config) {
        logger->log(LogLevel::INFO, "Configuring VR for Godot game: " + gamePath);
        // Implementation would configure VR settings
        return true;
    }

    bool configureVRForCustom(const std::string& gamePath, const VRConfiguration& config) {
        logger->log(LogLevel::INFO, "Configuring VR for Custom Engine game: " + gamePath);
        // Implementation would configure VR settings
        return true;
    }

    // Engine-specific VR removal methods
    bool removeVRForUnreal(const std::string& gamePath) {
        logger->log(LogLevel::INFO, "Removing VR from Unreal Engine game: " + gamePath);
        // Implementation would remove VR hooks
        return true;
    }

    bool removeVRForUnity(const std::string& gamePath) {
        logger->log(LogLevel::INFO, "Removing VR from Unity game: " + gamePath);
        // Implementation would remove VR hooks
        return true;
    }

    bool removeVRForGodot(const std::string& gamePath) {
        logger->log(LogLevel::INFO, "Removing VR from Godot game: " + gamePath);
        // Implementation would remove VR hooks
        return true;
    }

    bool removeVRForCustom(const std::string& gamePath) {
        logger->log(LogLevel::INFO, "Removing VR from Custom Engine game: " + gamePath);
        // Implementation would remove VR hooks
        return true;
    }
};

// EngineAdapter implementation
EngineAdapter::EngineAdapter() : impl(std::make_unique<EngineAdapterImpl>()) {}

EngineAdapter::~EngineAdapter() = default;

bool EngineAdapter::initialize() {
    return impl->initialize();
}

bool EngineAdapter::injectVR(const std::string& gamePath, const VRConfiguration& config) {
    return impl->injectVR(gamePath, config);
}

bool EngineAdapter::configureVR(const std::string& gamePath, const VRConfiguration& config) {
    return impl->configureVR(gamePath, config);
}

bool EngineAdapter::removeVR(const std::string& gamePath) {
    return impl->removeVR(gamePath);
}

std::vector<std::string> EngineAdapter::getSupportedEngines() {
    return impl->getSupportedEngines();
}

bool EngineAdapter::isEngineSupported(const std::string& engineType) {
    return impl->isEngineSupported(engineType);
}

VRConfiguration EngineAdapter::getDefaultConfig(const std::string& engineType) {
    return impl->getDefaultConfig(engineType);
}

bool EngineAdapter::shutdown() {
    return impl->shutdown();
}

} // namespace core
} // namespace uevrLCX
