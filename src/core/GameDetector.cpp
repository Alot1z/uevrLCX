#include "GameDetector.h"
#include "LoggingSystem.h"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <regex>

namespace uevrLCX {
namespace core {

class GameDetectorImpl {
private:
    std::shared_ptr<LoggingSystem> logger;
    bool isInitialized;
    std::vector<std::string> knownGamePaths;
    std::vector<std::string> knownEngineSignatures;

public:
    GameDetectorImpl() : isInitialized(false) {
        logger = std::make_shared<LoggingSystem>();
        initializeKnownSignatures();
        logger->log(LogLevel::INFO, "GameDetector initialized");
    }

    bool detectGame(const std::string& gamePath) {
        try {
            if (!isInitialized) {
                logger->log(LogLevel::ERROR, "GameDetector not initialized");
                return false;
            }

            logger->log(LogLevel::DEBUG, "Detecting game at: " + gamePath);

            if (!std::filesystem::exists(gamePath)) {
                logger->log(LogLevel::WARNING, "Game path does not exist: " + gamePath);
                return false;
            }

            // Check if it's a directory (game folder)
            if (std::filesystem::is_directory(gamePath)) {
                return detectGameFromDirectory(gamePath);
            }

            // Check if it's an executable file
            if (std::filesystem::is_regular_file(gamePath)) {
                return detectGameFromExecutable(gamePath);
            }

            logger->log(LogLevel::WARNING, "Invalid game path: " + gamePath);
            return false;

        } catch (const std::exception& e) {
            logger->log(LogLevel::ERROR, "Error detecting game: " + std::string(e.what()));
            return false;
        }
    }

    std::string detectEngine(const std::string& gamePath) {
        try {
            if (!isInitialized) {
                logger->log(LogLevel::ERROR, "GameDetector not initialized");
                return "";
            }

            logger->log(LogLevel::DEBUG, "Detecting engine for: " + gamePath);

            std::string engine = "";

            // Check for Unreal Engine signatures
            if (hasUnrealEngineSignature(gamePath)) {
                engine = "Unreal Engine";
            }
            // Check for Unity signatures
            else if (hasUnitySignature(gamePath)) {
                engine = "Unity";
            }
            // Check for Godot signatures
            else if (hasGodotSignature(gamePath)) {
                engine = "Godot";
            }
            // Check for custom engine signatures
            else if (hasCustomEngineSignature(gamePath)) {
                engine = "Custom Engine";
            }
            else {
                engine = "Unknown Engine";
            }

            logger->log(LogLevel::INFO, "Detected engine: " + engine + " for " + gamePath);
            return engine;

        } catch (const std::exception& e) {
            logger->log(LogLevel::ERROR, "Error detecting engine: " + std::string(e.what()));
            return "";
        }
    }

    std::vector<std::string> scanForGames() {
        try {
            if (!isInitialized) {
                logger->log(LogLevel::ERROR, "GameDetector not initialized");
                return std::vector<std::string>();
            }

            logger->log(LogLevel::INFO, "Scanning for games");

            std::vector<std::string> foundGames;

            // Scan common game directories
            std::vector<std::string> commonPaths = {
                "C:/Program Files (x86)/Steam/steamapps/common",
                "C:/Program Files/Steam/steamapps/common",
                "C:/Program Files (x86)/Epic Games",
                "C:/Program Files/Epic Games",
                "C:/Games",
                "D:/Games",
                "E:/Games"
            };

            for (const auto& path : commonPaths) {
                if (std::filesystem::exists(path)) {
                    auto gamesInPath = scanDirectoryForGames(path);
                    foundGames.insert(foundGames.end(), gamesInPath.begin(), gamesInPath.end());
                }
            }

            logger->log(LogLevel::INFO, "Found " + std::to_string(foundGames.size()) + " games");
            return foundGames;

        } catch (const std::exception& e) {
            logger->log(LogLevel::ERROR, "Error scanning for games: " + std::string(e.what()));
            return std::vector<std::string>();
        }
    }

    bool isGameRunning(const std::string& gamePath) {
        try {
            if (!isInitialized) {
                logger->log(LogLevel::ERROR, "GameDetector not initialized");
                return false;
            }

            logger->log(LogLevel::DEBUG, "Checking if game is running: " + gamePath);

            // Extract executable name from path
            std::filesystem::path path(gamePath);
            std::string executableName = path.filename().string();

            // Remove extension if present
            if (executableName.find('.') != std::string::npos) {
                executableName = executableName.substr(0, executableName.find('.'));
            }

            // Check if process is running (simplified implementation)
            // In a real implementation, this would use Windows API or similar
            bool isRunning = checkProcessRunning(executableName);

            logger->log(LogLevel::DEBUG, "Game running status: " + std::string(isRunning ? "Yes" : "No"));
            return isRunning;

        } catch (const std::exception& e) {
            logger->log(LogLevel::ERROR, "Error checking if game is running: " + std::string(e.what()));
            return false;
        }
    }

    bool initialize() {
        try {
            if (isInitialized) {
                logger->log(LogLevel::WARNING, "GameDetector already initialized");
                return true;
            }

            logger->log(LogLevel::INFO, "Initializing GameDetector");

            // Initialize detection systems
            initializeKnownSignatures();
            loadKnownGamePaths();

            isInitialized = true;
            logger->log(LogLevel::INFO, "GameDetector initialized successfully");
            return true;

        } catch (const std::exception& e) {
            logger->log(LogLevel::ERROR, "Error initializing GameDetector: " + std::string(e.what()));
            return false;
        }
    }

    bool shutdown() {
        try {
            if (!isInitialized) {
                logger->log(LogLevel::WARNING, "GameDetector not initialized");
                return true;
            }

            logger->log(LogLevel::INFO, "Shutting down GameDetector");

            isInitialized = false;
            logger->log(LogLevel::INFO, "GameDetector shut down successfully");
            return true;

        } catch (const std::exception& e) {
            logger->log(LogLevel::ERROR, "Error shutting down GameDetector: " + std::string(e.what()));
            return false;
        }
    }

private:
    void initializeKnownSignatures() {
        // Unreal Engine signatures
        knownEngineSignatures = {
            "UE4", "UE5", "UnrealEngine", "Engine", "Unreal",
            "Unity", "UnityEngine", "MonoBehaviour",
            "Godot", "GodotEngine",
            "CustomEngine", "GameEngine"
        };
    }

    void loadKnownGamePaths() {
        // Load known game paths from configuration or registry
        // For now, use common paths
        knownGamePaths = {
            "C:/Program Files (x86)/Steam/steamapps/common",
            "C:/Program Files/Steam/steamapps/common",
            "C:/Program Files (x86)/Epic Games",
            "C:/Program Files/Epic Games"
        };
    }

    bool detectGameFromDirectory(const std::string& gamePath) {
        try {
            std::filesystem::path path(gamePath);
            
            // Look for common game files
            std::vector<std::string> gameFiles = {
                "game.exe", "Game.exe", "GAME.EXE",
                "launcher.exe", "Launcher.exe",
                "start.exe", "Start.exe",
                "main.exe", "Main.exe"
            };

            for (const auto& file : gameFiles) {
                std::filesystem::path exePath = path / file;
                if (std::filesystem::exists(exePath)) {
                    logger->log(LogLevel::INFO, "Found game executable: " + exePath.string());
                    return true;
                }
            }

            // Look for engine-specific files
            if (hasEngineFiles(gamePath)) {
                logger->log(LogLevel::INFO, "Found engine files in: " + gamePath);
                return true;
            }

            return false;

        } catch (const std::exception& e) {
            logger->log(LogLevel::ERROR, "Error detecting game from directory: " + std::string(e.what()));
            return false;
        }
    }

    bool detectGameFromExecutable(const std::string& gamePath) {
        try {
            // Check file extension
            std::filesystem::path path(gamePath);
            std::string extension = path.extension().string();
            
            if (extension != ".exe" && extension != ".EXE") {
                logger->log(LogLevel::WARNING, "Not an executable file: " + gamePath);
                return false;
            }

            // Check file size (games are typically large)
            std::filesystem::path filePath(gamePath);
            if (std::filesystem::exists(filePath)) {
                auto fileSize = std::filesystem::file_size(filePath);
                if (fileSize > 1024 * 1024) { // Larger than 1MB
                    logger->log(LogLevel::INFO, "Found potential game executable: " + gamePath);
                    return true;
                }
            }

            return false;

        } catch (const std::exception& e) {
            logger->log(LogLevel::ERROR, "Error detecting game from executable: " + std::string(e.what()));
            return false;
        }
    }

    bool hasUnrealEngineSignature(const std::string& gamePath) {
        try {
            std::vector<std::string> unrealSignatures = {
                "Engine", "UnrealEngine", "UE4", "UE5", "Unreal",
                "Engine.ini", "Game.ini", "DefaultEngine.ini"
            };

            return hasAnySignature(gamePath, unrealSignatures);

        } catch (const std::exception& e) {
            logger->log(LogLevel::ERROR, "Error checking Unreal Engine signature: " + std::string(e.what()));
            return false;
        }
    }

    bool hasUnitySignature(const std::string& gamePath) {
        try {
            std::vector<std::string> unitySignatures = {
                "Unity", "UnityEngine", "MonoBehaviour", "Assembly-CSharp.dll",
                "UnityPlayer.dll", "UnityCrashHandler64.exe"
            };

            return hasAnySignature(gamePath, unitySignatures);

        } catch (const std::exception& e) {
            logger->log(LogLevel::ERROR, "Error checking Unity signature: " + std::string(e.what()));
            return false;
        }
    }

    bool hasGodotSignature(const std::string& gamePath) {
        try {
            std::vector<std::string> godotSignatures = {
                "Godot", "GodotEngine", "godot", "libgodot",
                "project.godot", "export_presets.cfg"
            };

            return hasAnySignature(gamePath, godotSignatures);

        } catch (const std::exception& e) {
            logger->log(LogLevel::ERROR, "Error checking Godot signature: " + std::string(e.what()));
            return false;
        }
    }

    bool hasCustomEngineSignature(const std::string& gamePath) {
        try {
            std::vector<std::string> customSignatures = {
                "CustomEngine", "GameEngine", "Engine.dll", "Game.dll"
            };

            return hasAnySignature(gamePath, customSignatures);

        } catch (const std::exception& e) {
            logger->log(LogLevel::ERROR, "Error checking custom engine signature: " + std::string(e.what()));
            return false;
        }
    }

    bool hasAnySignature(const std::string& gamePath, const std::vector<std::string>& signatures) {
        try {
            std::filesystem::path path(gamePath);
            
            // Check if it's a directory
            if (std::filesystem::is_directory(path)) {
                for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
                    std::string fileName = entry.path().filename().string();
                    for (const auto& signature : signatures) {
                        if (fileName.find(signature) != std::string::npos) {
                            return true;
                        }
                    }
                }
            }
            // Check if it's a file
            else if (std::filesystem::is_regular_file(path)) {
                std::string fileName = path.filename().string();
                for (const auto& signature : signatures) {
                    if (fileName.find(signature) != std::string::npos) {
                        return true;
                    }
                }
            }

            return false;

        } catch (const std::exception& e) {
            logger->log(LogLevel::ERROR, "Error checking signatures: " + std::string(e.what()));
            return false;
        }
    }

    bool hasEngineFiles(const std::string& gamePath) {
        try {
            std::vector<std::string> engineFiles = {
                "Engine.ini", "Game.ini", "DefaultEngine.ini",
                "project.godot", "export_presets.cfg",
                "Assembly-CSharp.dll", "UnityPlayer.dll"
            };

            std::filesystem::path path(gamePath);
            for (const auto& file : engineFiles) {
                std::filesystem::path filePath = path / file;
                if (std::filesystem::exists(filePath)) {
                    return true;
                }
            }

            return false;

        } catch (const std::exception& e) {
            logger->log(LogLevel::ERROR, "Error checking engine files: " + std::string(e.what()));
            return false;
        }
    }

    std::vector<std::string> scanDirectoryForGames(const std::string& directoryPath) {
        try {
            std::vector<std::string> foundGames;
            std::filesystem::path path(directoryPath);

            if (!std::filesystem::exists(path)) {
                return foundGames;
            }

            for (const auto& entry : std::filesystem::directory_iterator(path)) {
                if (entry.is_directory()) {
                    std::string entryPath = entry.path().string();
                    if (detectGame(entryPath)) {
                        foundGames.push_back(entryPath);
                    }
                }
            }

            return foundGames;

        } catch (const std::exception& e) {
            logger->log(LogLevel::ERROR, "Error scanning directory for games: " + std::string(e.what()));
            return std::vector<std::string>();
        }
    }

    bool checkProcessRunning(const std::string& processName) {
        try {
            // Simplified process checking
            // In a real implementation, this would use Windows API (EnumProcesses, etc.)
            
            // For now, just return false as a placeholder
            // This would need to be implemented with proper Windows API calls
            logger->log(LogLevel::DEBUG, "Process checking not fully implemented for: " + processName);
            return false;

        } catch (const std::exception& e) {
            logger->log(LogLevel::ERROR, "Error checking process: " + std::string(e.what()));
            return false;
        }
    }
};

// GameDetector implementation
GameDetector::GameDetector() : impl(std::make_unique<GameDetectorImpl>()) {}

GameDetector::~GameDetector() = default;

bool GameDetector::detectGame(const std::string& gamePath) {
    return impl->detectGame(gamePath);
}

std::string GameDetector::detectEngine(const std::string& gamePath) {
    return impl->detectEngine(gamePath);
}

std::vector<std::string> GameDetector::scanForGames() {
    return impl->scanForGames();
}

bool GameDetector::isGameRunning(const std::string& gamePath) {
    return impl->isGameRunning(gamePath);
}

} // namespace core
} // namespace uevrLCX
