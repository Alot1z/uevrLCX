#include "mt_framework_factory.hpp"
#include <uevr/CrossEngineAdapterRegistry.hpp>
#include <windows.h>
#include <memory>
#include <string>

// DLL entry point
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        // Register MT Framework adapter factory with uevr
        {
            auto factory = std::make_shared<uevr::MTFramework::MTFrameworkAdapterFactory>();
            uevr::CrossEngineAdapterRegistry::registerFactory(factory);
        }
        break;
        
    case DLL_PROCESS_DETACH:
        // Cleanup when DLL is unloaded
        break;
        
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;
    }
    return TRUE;
}

// Export functions for external access
extern "C" {
    
    /**
     * @brief Create Monster Hunter World VR adapter
     * @return Pointer to the created adapter, or nullptr if creation failed
     */
    __declspec(dllexport) uevr::ICrossEngineAdapter* createMonsterHunterWorldAdapter() {
        auto factory = std::make_shared<uevr::MTFramework::MTFrameworkAdapterFactory>();
        auto adapter = factory->createAdapter("MonsterHunterWorld.exe");
        return adapter.get();
    }
    
    /**
     * @brief Check if Monster Hunter World is supported
     * @return true if supported, false otherwise
     */
    __declspec(dllexport) bool isMonsterHunterWorldSupported() {
        auto factory = std::make_shared<uevr::MTFramework::MTFrameworkAdapterFactory>();
        return factory->canCreateAdapter("MonsterHunterWorld.exe");
    }
    
    /**
     * @brief Get MT Framework engine type
     * @return Engine type string
     */
    __declspec(dllexport) const char* getMTFrameworkEngineType() {
        return "MT-Framework";
    }
    
    /**
     * @brief Get Monster Hunter World adapter version
     * @return Version string
     */
    __declspec(dllexport) const char* getMonsterHunterWorldAdapterVersion() {
        return "1.0.0";
    }
    
    /**
     * @brief Initialize Monster Hunter World VR adapter
     * @return true if initialization successful, false otherwise
     */
    __declspec(dllexport) bool initializeMonsterHunterWorldVR() {
        auto factory = std::make_shared<uevr::MTFramework::MTFrameworkAdapterFactory>();
        auto adapter = factory->createAdapter("MonsterHunterWorld.exe");
        
        if (adapter) {
            return adapter->initialize();
        }
        
        return false;
    }
    
    /**
     * @brief Cleanup Monster Hunter World VR adapter
     */
    __declspec(dllexport) void cleanupMonsterHunterWorldVR() {
        // This would typically be called when the game exits
        // The adapter will be automatically cleaned up when the DLL is unloaded
    }
}

// Namespace for C++ access
namespace uevr {
namespace MTFramework {

/**
 * @brief Initialize MT Framework cross-engine integration
 * @return true if initialization successful, false otherwise
 */
bool initializeMTFrameworkIntegration() {
    try {
        // Register the factory with the global registry
        auto factory = std::make_shared<MTFrameworkAdapterFactory>();
        CrossEngineAdapterRegistry::registerFactory(factory);
        
        return true;
    } catch (...) {
        return false;
    }
}

/**
 * @brief Cleanup MT Framework cross-engine integration
 */
void cleanupMTFrameworkIntegration() {
    // Unregister factories and cleanup resources
    // This is typically called during shutdown
}

} // namespace MTFramework
} // namespace uevr
