#include "adapter_loader.h"
#include "engine_detection.h"
#include <Windows.h>
#include <filesystem>
#include <iostream>
#include <json/json.h>
#include <map>
#include <string>

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
    };

    std::map<std::string, AdapterInfo> adapters;
    std::string current_adapter;
    bool system_initialized;

public:
    AdapterLoader() : system_initialized(false) {
        loadAdapterRegistry();
    }

    ~AdapterLoader() {
        unloadAllAdapters();
    }

    bool initializeSystem() {
        if (system_initialized) {
            return true;
        }

        // Load common adapter
        if (!loadCommonAdapter()) {
            std::cerr << "Failed to load common adapter" << std::endl;
            return false;
        }

        system_initialized = true;
        return true;
    }

    bool loadAdapterForEngine(const std::string& engine_name) {
        auto it = adapters.find(engine_name);
        if (it == adapters.end()) {
            std::cerr << "No adapter found for engine: " << engine_name << std::endl;
            return false;
        }

        if (it->second.loaded) {
            current_adapter = engine_name;
            return true;
        }

        if (loadAdapter(it->second)) {
            current_adapter = engine_name;
            return true;
        }

        return false;
    }

    bool hotSwapAdapter(const std::string& new_engine) {
        if (current_adapter == new_engine) {
            return true;
        }

        // Unload current adapter
        if (!current_adapter.empty()) {
            auto it = adapters.find(current_adapter);
            if (it != adapters.end() && it->second.loaded) {
                unloadAdapter(it->second);
            }
        }

        // Load new adapter
        return loadAdapterForEngine(new_engine);
    }

    std::string getCurrentAdapter() const { return current_adapter; }
    bool isSystemInitialized() const { return system_initialized; }

private:
    void loadAdapterRegistry() {
        std::ifstream file("adapters/adapter_registry.json");
        if (!file.is_open()) {
            std::cerr << "Failed to open adapter registry" << std::endl;
            return;
        }

        Json::Value root;
        Json::Reader reader;
        if (!reader.parse(file, root)) {
            std::cerr << "Failed to parse adapter registry" << std::endl;
            return;
        }

        for (const auto& adapter : root["adapters"]) {
            AdapterInfo info;
            info.name = adapter["name"].asString();
            info.path = adapter["path"].asString();
            info.version = adapter["version"].asString();
            info.loaded = false;
            info.module = nullptr;

            adapters[info.name] = info;
        }
    }

    bool loadCommonAdapter() {
        auto it = adapters.find("common");
        if (it == adapters.end()) {
            std::cerr << "Common adapter not found in registry" << std::endl;
            return false;
        }

        return loadAdapter(it->second);
    }

    bool loadAdapter(AdapterInfo& info) {
        std::filesystem::path adapterPath = std::filesystem::current_path() / info.path;
        
        if (!std::filesystem::exists(adapterPath)) {
            std::cerr << "Adapter path does not exist: " << adapterPath << std::endl;
            return false;
        }

        // Load the DLL
        info.module = LoadLibraryW(adapterPath.wstring().c_str());
        if (!info.module) {
            DWORD error = GetLastError();
            std::cerr << "Failed to load adapter " << info.name << " (error: " << error << ")" << std::endl;
            return false;
        }

        // Get initialization function
        typedef bool (*InitFunc)();
        InitFunc initFunc = (InitFunc)GetProcAddress(info.module, "InitializeAdapter");
        if (!initFunc) {
            std::cerr << "Failed to get InitializeAdapter function for " << info.name << std::endl;
            FreeLibrary(info.module);
            return false;
        }

        // Initialize the adapter
        if (!initFunc()) {
            std::cerr << "Failed to initialize adapter " << info.name << std::endl;
            FreeLibrary(info.module);
            return false;
        }

        info.loaded = true;
        std::cout << "Successfully loaded adapter: " << info.name << " (version: " << info.version << ")" << std::endl;
        return true;
    }

    void unloadAdapter(AdapterInfo& info) {
        if (!info.loaded || !info.module) {
            return;
        }

        // Get cleanup function
        typedef void (*CleanupFunc)();
        CleanupFunc cleanupFunc = (CleanupFunc)GetProcAddress(info.module, "CleanupAdapter");
        if (cleanupFunc) {
            cleanupFunc();
        }

        FreeLibrary(info.module);
        info.module = nullptr;
        info.loaded = false;
        std::cout << "Unloaded adapter: " << info.name << std::endl;
    }

    void unloadAllAdapters() {
        for (auto& [name, adapter] : adapters) {
            if (adapter.loaded) {
                unloadAdapter(adapter);
            }
        }
    }
};

// Global adapter loader instance
static AdapterLoader g_adapterLoader;

bool initializeAdapterSystem() {
    return g_adapterLoader.initializeSystem();
}

bool loadEngineAdapter(const std::string& engine_name) {
    return g_adapterLoader.loadAdapterForEngine(engine_name);
}

bool hotSwapEngineAdapter(const std::string& new_engine) {
    return g_adapterLoader.hotSwapAdapter(new_engine);
}

std::string getCurrentEngineAdapter() {
    return g_adapterLoader.getCurrentAdapter();
}

bool isAdapterSystemReady() {
    return g_adapterLoader.isSystemInitialized();
}

} // namespace Core
} // namespace UEVR


