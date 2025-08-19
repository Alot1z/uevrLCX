#include "adapter_loader.h"
#include "engine_detection.h"
#include <Windows.h>
#include <filesystem>
#include <iostream>
#include <json/json.h>
#include <map>
#include <string>
#include <spdlog/spdlog.h>
#include <memory>
#include <vector>
#include <algorithm>

namespace UEVR {
namespace Core {

class AdapterLoader {
private:
    struct AdapterInfo {
        std::string name;
        std::string path;
        std::string version;
        bool loaded;
        HMODULE module;
        EngineType engine_type;
        std::string description;
        std::vector<std::string> supported_games;
        bool is_verified;
        std::chrono::steady_clock::time_point last_loaded;
    };

    std::map<std::string, AdapterInfo> adapters;
    std::string current_adapter;
    bool system_initialized;
    std::vector<std::string> loaded_adapters;

public:
    AdapterLoader() : system_initialized(false) {
        loadAdapterRegistry();
    }

    ~AdapterLoader() {
        unloadAllAdapters();
    }

    bool initializeSystem() {
        try {
            if (system_initialized) {
                spdlog::warn("[AdapterLoader] Adapter system already initialized");
                return true;
            }

            spdlog::info("[AdapterLoader] Initializing cross-engine adapter system...");

            // Load common adapter first
            if (!loadCommonAdapter()) {
                spdlog::warn("[AdapterLoader] Failed to load common adapter, continuing...");
            }

            // Pre-load critical engine adapters
            if (!preloadCriticalAdapters()) {
                spdlog::warn("[AdapterLoader] Failed to preload some critical adapters");
            }

            system_initialized = true;
            spdlog::info("[AdapterLoader] Adapter system initialized successfully");
            return true;
            
        } catch (const std::exception& e) {
            spdlog::error("[AdapterLoader] System initialization failed: {}", e.what());
            return false;
        }
    }

    bool loadAdapterForEngine(const std::string& engine_name) {
        try {
            auto it = adapters.find(engine_name);
            if (it == adapters.end()) {
                spdlog::error("[AdapterLoader] No adapter found for engine: {}", engine_name);
                return false;
            }

            if (it->second.loaded) {
                current_adapter = engine_name;
                spdlog::info("[AdapterLoader] Adapter {} already loaded", engine_name);
                return true;
            }

            if (loadAdapter(it->second)) {
                current_adapter = engine_name;
                it->second.last_loaded = std::chrono::steady_clock::now();
                loaded_adapters.push_back(engine_name);
                return true;
            }

            return false;
            
        } catch (const std::exception& e) {
            spdlog::error("[AdapterLoader] Failed to load adapter for engine {}: {}", engine_name, e.what());
            return false;
        }
    }

    bool hotSwapAdapter(const std::string& new_engine) {
        try {
            if (current_adapter == new_engine) {
                spdlog::info("[AdapterLoader] Adapter {} already active", new_engine);
                return true;
            }

            spdlog::info("[AdapterLoader] Hot-swapping adapter from {} to {}", current_adapter, new_engine);

            // Unload current adapter
            if (!current_adapter.empty()) {
                auto it = adapters.find(current_adapter);
                if (it != adapters.end() && it->second.loaded) {
                    unloadAdapter(it->second);
                }
            }

            // Load new adapter
            bool success = loadAdapterForEngine(new_engine);
            if (success) {
                spdlog::info("[AdapterLoader] Hot-swap completed successfully");
            } else {
                spdlog::error("[AdapterLoader] Hot-swap failed");
            }
            return success;
            
        } catch (const std::exception& e) {
            spdlog::error("[AdapterLoader] Hot-swap exception: {}", e.what());
            return false;
        }
    }

    std::string getCurrentAdapter() const { return current_adapter; }
    bool isSystemInitialized() const { return system_initialized; }

    // Get adapter information
    std::vector<AdapterInfo> getAllAdapters() const {
        std::vector<AdapterInfo> result;
        for (const auto& [name, adapter] : adapters) {
            result.push_back(adapter);
        }
        return result;
    }

    // Get loaded adapters
    std::vector<std::string> getLoadedAdapters() const {
        return loaded_adapters;
    }

    // Check if adapter is loaded
    bool isAdapterLoaded(const std::string& adapter_name) const {
        auto it = adapters.find(adapter_name);
        return it != adapters.end() && it->second.loaded;
    }

private:
    void loadAdapterRegistry() {
        try {
            spdlog::info("[AdapterLoader] Loading adapter registry...");
            
            // Load from file if available
            loadAdapterRegistryFromFile();
            
            // Add built-in adapters if file loading failed
            if (adapters.empty()) {
                addBuiltInAdapters();
            }
            
            spdlog::info("[AdapterLoader] Adapter registry loaded: {} adapters", adapters.size());
            
        } catch (const std::exception& e) {
            spdlog::error("[AdapterLoader] Failed to load adapter registry: {}", e.what());
            // Fallback to built-in adapters
            addBuiltInAdapters();
        }
    }

    void loadAdapterRegistryFromFile() {
        try {
            std::ifstream file("adapters/adapter_registry.json");
            if (!file.is_open()) {
                spdlog::warn("[AdapterLoader] No adapter registry file found");
                return;
            }

            Json::Value root;
            Json::Reader reader;
            if (!reader.parse(file, root)) {
                spdlog::error("[AdapterLoader] Failed to parse adapter registry");
                return;
            }

            for (const auto& adapter : root["adapters"]) {
                AdapterInfo info;
                info.name = adapter["name"].asString();
                info.path = adapter["path"].asString();
                info.version = adapter["version"].asString();
                info.engine_type = static_cast<EngineType>(adapter["engine_type"].asInt());
                info.description = adapter.get("description", "").asString();
                info.loaded = false;
                info.module = nullptr;
                info.is_verified = adapter.get("is_verified", false).asBool();

                // Parse supported games
                if (adapter.isMember("supported_games")) {
                    for (const auto& game : adapter["supported_games"]) {
                        info.supported_games.push_back(game.asString());
                    }
                }

                adapters[info.name] = info;
            }
            
        } catch (const std::exception& e) {
            spdlog::error("[AdapterLoader] Failed to load adapter registry from file: {}", e.what());
        }
    }

    void addBuiltInAdapters() {
        try {
            spdlog::info("[AdapterLoader] Adding built-in adapters...");
            
            // RE Engine Adapter
            addBuiltInAdapter({
                "RE Engine",
                "adapters/re-engine/re_engine_adapter.dll",
                "2.0.0",
                false,
                nullptr,
                EngineType::RE_ENGINE,
                "RE Engine VR adapter for Resident Evil 7/8 and other Capcom games",
                {"Resident Evil 7", "Resident Evil 8", "Monster Hunter Rise"},
                true
            });

            // REDengine 4 Adapter
            addBuiltInAdapter({
                "REDengine 4",
                "adapters/redengine4/redengine4_adapter.dll",
                "2.0.0",
                false,
                nullptr,
                EngineType::REDENGINE_4,
                "REDengine 4 VR adapter for Cyberpunk 2077 and CD Projekt Red games",
                {"Cyberpunk 2077", "The Witcher 3"},
                true
            });

            // MT Framework Adapter
            addBuiltInAdapter({
                "MT Framework",
                "adapters/mt-framework/mt_framework_adapter.dll",
                "2.0.0",
                false,
                nullptr,
                EngineType::MT_FRAMEWORK,
                "MT Framework VR adapter for Monster Hunter World and Capcom games",
                {"Monster Hunter World", "Devil May Cry 4", "Devil May Cry 5"},
                true
            });

            // Unreal Engine 4 Adapter
            addBuiltInAdapter({
                "Unreal Engine 4",
                "adapters/unreal-engine4/ue4_adapter.dll",
                "2.0.0",
                false,
                nullptr,
                EngineType::UNREAL_ENGINE_4,
                "Unreal Engine 4 VR adapter for UE4 games",
                {"Generic UE4 Games"},
                true
            });

            // Unreal Engine 5 Adapter
            addBuiltInAdapter({
                "Unreal Engine 5",
                "adapters/unreal-engine5/ue5_adapter.dll",
                "2.0.0",
                false,
                nullptr,
                EngineType::UNREAL_ENGINE_5,
                "Unreal Engine 5 VR adapter for UE5 games",
                {"Generic UE5 Games"},
                true
            });

            // Unity Engine Adapter
            addBuiltInAdapter({
                "Unity Engine",
                "adapters/unity-engine/unity_adapter.dll",
                "2.0.0",
                false,
                nullptr,
                EngineType::UNITY_ENGINE,
                "Unity Engine VR adapter for Unity games",
                {"Generic Unity Games"},
                true
            });

            // Common Adapter
            addBuiltInAdapter({
                "Common",
                "adapters/common/common_adapter.dll",
                "2.0.0",
                false,
                nullptr,
                EngineType::CUSTOM_ENGINE,
                "Common VR functionality for all engines",
                {"All Games"},
                true
            });
            
        } catch (const std::exception& e) {
            spdlog::error("[AdapterLoader] Failed to add built-in adapters: {}", e.what());
        }
    }

    void addBuiltInAdapter(const AdapterInfo& info) {
        adapters[info.name] = info;
        spdlog::debug("[AdapterLoader] Added built-in adapter: {} ({})", info.name, info.description);
    }

    bool loadCommonAdapter() {
        try {
            auto it = adapters.find("Common");
            if (it == adapters.end()) {
                spdlog::warn("[AdapterLoader] Common adapter not found in registry");
                return false;
            }

            bool success = loadAdapter(it->second);
            if (success) {
                spdlog::info("[AdapterLoader] Common adapter loaded successfully");
            }
            return success;
            
        } catch (const std::exception& e) {
            spdlog::error("[AdapterLoader] Common adapter loading failed: {}", e.what());
            return false;
        }
    }

    bool preloadCriticalAdapters() {
        try {
            spdlog::info("[AdapterLoader] Preloading critical engine adapters...");
            
            std::vector<std::string> critical_engines = {
                "RE Engine", "REDengine 4", "MT Framework"
            };

            int success_count = 0;
            for (const auto& engine : critical_engines) {
                auto it = adapters.find(engine);
                if (it != adapters.end()) {
                    if (loadAdapter(it->second)) {
                        success_count++;
                        spdlog::info("[AdapterLoader] Critical adapter {} loaded", engine);
                    } else {
                        spdlog::warn("[AdapterLoader] Failed to load critical adapter {}", engine);
                    }
                }
            }

            spdlog::info("[AdapterLoader] Preloaded {}/{} critical adapters", success_count, critical_engines.size());
            return success_count > 0;
            
        } catch (const std::exception& e) {
            spdlog::error("[AdapterLoader] Critical adapter preloading failed: {}", e.what());
            return false;
        }
    }

    bool loadAdapter(AdapterInfo& info) {
        try {
            std::filesystem::path adapterPath = std::filesystem::current_path() / info.path;
            
            if (!std::filesystem::exists(adapterPath)) {
                spdlog::warn("[AdapterLoader] Adapter path does not exist: {}", adapterPath.string());
                return false;
            }

            spdlog::info("[AdapterLoader] Loading adapter: {} from {}", info.name, adapterPath.string());

            // Load the DLL
            info.module = LoadLibraryW(adapterPath.wstring().c_str());
            if (!info.module) {
                DWORD error = GetLastError();
                spdlog::error("[AdapterLoader] Failed to load adapter {} (error: {})", info.name, error);
                return false;
            }

            // Get initialization function
            typedef bool (*InitFunc)();
            InitFunc initFunc = (InitFunc)GetProcAddress(info.module, "InitializeAdapter");
            if (!initFunc) {
                spdlog::error("[AdapterLoader] Failed to get InitializeAdapter function for {}", info.name);
                FreeLibrary(info.module);
                return false;
            }

            // Initialize the adapter
            if (!initFunc()) {
                spdlog::error("[AdapterLoader] Failed to initialize adapter {}", info.name);
                FreeLibrary(info.module);
                return false;
            }

            info.loaded = true;
            info.last_loaded = std::chrono::steady_clock::now();
            
            spdlog::info("[AdapterLoader] Successfully loaded adapter: {} (version: {})", 
                        info.name, info.version);
            return true;
            
        } catch (const std::exception& e) {
            spdlog::error("[AdapterLoader] Adapter loading exception for {}: {}", info.name, e.what());
            return false;
        }
    }

    void unloadAdapter(AdapterInfo& info) {
        try {
            if (!info.loaded || !info.module) {
                return;
            }

            spdlog::info("[AdapterLoader] Unloading adapter: {}", info.name);

            // Get cleanup function
            typedef void (*CleanupFunc)();
            CleanupFunc cleanupFunc = (CleanupFunc)GetProcAddress(info.module, "CleanupAdapter");
            if (cleanupFunc) {
                cleanupFunc();
            }

            FreeLibrary(info.module);
            info.module = nullptr;
            info.loaded = false;
            
            // Remove from loaded adapters list
            auto it = std::find(loaded_adapters.begin(), loaded_adapters.end(), info.name);
            if (it != loaded_adapters.end()) {
                loaded_adapters.erase(it);
            }
            
            spdlog::info("[AdapterLoader] Unloaded adapter: {}", info.name);
            
        } catch (const std::exception& e) {
            spdlog::error("[AdapterLoader] Adapter unloading exception for {}: {}", info.name, e.what());
        }
    }

    void unloadAllAdapters() {
        try {
            spdlog::info("[AdapterLoader] Unloading all adapters...");
            
            for (auto& [name, adapter] : adapters) {
                if (adapter.loaded) {
                    unloadAdapter(adapter);
                }
            }
            
            loaded_adapters.clear();
            current_adapter.clear();
            
        } catch (const std::exception& e) {
            spdlog::error("[AdapterLoader] Failed to unload all adapters: {}", e.what());
        }
    }
};

// Global adapter loader instance
static std::unique_ptr<AdapterLoader> g_adapterLoader;

bool initializeAdapterSystem() {
    try {
        if (!g_adapterLoader) {
            g_adapterLoader = std::make_unique<AdapterLoader>();
        }
        return g_adapterLoader->initializeSystem();
        
    } catch (const std::exception& e) {
        spdlog::error("[AdapterLoader] Failed to initialize adapter system: {}", e.what());
        return false;
    }
}

bool loadEngineAdapter(const std::string& engine_name) {
    try {
        if (!g_adapterLoader) {
            spdlog::error("[AdapterLoader] Adapter system not initialized");
            return false;
        }
        return g_adapterLoader->loadAdapterForEngine(engine_name);
        
    } catch (const std::exception& e) {
        spdlog::error("[AdapterLoader] Failed to load engine adapter: {}", e.what());
        return false;
    }
}

bool hotSwapEngineAdapter(const std::string& new_engine) {
    try {
        if (!g_adapterLoader) {
            spdlog::error("[AdapterLoader] Adapter system not initialized");
            return false;
        }
        return g_adapterLoader->hotSwapAdapter(new_engine);
        
    } catch (const std::exception& e) {
        spdlog::error("[AdapterLoader] Failed to hot-swap engine adapter: {}", e.what());
        return false;
    }
}

std::string getCurrentEngineAdapter() {
    try {
        return g_adapterLoader ? g_adapterLoader->getCurrentAdapter() : "";
    } catch (const std::exception& e) {
        spdlog::error("[AdapterLoader] Failed to get current engine adapter: {}", e.what());
        return "";
    }
}

bool isAdapterSystemReady() {
    try {
        return g_adapterLoader && g_adapterLoader->isSystemInitialized();
    } catch (const std::exception& e) {
        spdlog::error("[AdapterLoader] Failed to check adapter system readiness: {}", e.what());
        return false;
    }
}

// Additional utility functions
std::vector<std::string> getLoadedAdapters() {
    try {
        return g_adapterLoader ? g_adapterLoader->getLoadedAdapters() : std::vector<std::string>();
    } catch (const std::exception& e) {
        spdlog::error("[AdapterLoader] Failed to get loaded adapters: {}", e.what());
        return std::vector<std::string>();
    }
}

bool isAdapterLoaded(const std::string& adapter_name) {
    try {
        return g_adapterLoader ? g_adapterLoader->isAdapterLoaded(adapter_name) : false;
    } catch (const std::exception& e) {
        spdlog::error("[AdapterLoader] Failed to check adapter loaded status: {}", e.what());
        return false;
    }
}

// Initialize global adapter loader
void initializeGlobalAdapterLoader() {
    try {
        if (!g_adapterLoader) {
            g_adapterLoader = std::make_unique<AdapterLoader>();
            spdlog::info("[AdapterLoader] Global adapter loader initialized");
        }
    } catch (const std::exception& e) {
        spdlog::error("[AdapterLoader] Failed to initialize global loader: {}", e.what());
    }
}

// Cleanup global adapter loader
void cleanupGlobalAdapterLoader() {
    try {
        g_adapterLoader.reset();
        spdlog::info("[AdapterLoader] Global adapter loader cleaned up");
    } catch (const std::exception& e) {
        spdlog::error("[AdapterLoader] Failed to cleanup global loader: {}", e.what());
    }
}

} // namespace Core
} // namespace UEVR


