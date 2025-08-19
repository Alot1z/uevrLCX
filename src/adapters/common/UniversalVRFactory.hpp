#pragma once

#include "BaseCrossEngineAdapter.hpp"
#include "UniversalEngineRegistry.hpp"
#include <memory>
#include <unordered_map>
#include <functional>

namespace uevr {

/**
 * @brief Universal VR Factory for Mass Deployment
 * 
 * This factory automatically creates the appropriate VR adapter for any
 * detected game/engine combination, enabling support for 500+ games
 * without manual configuration.
 * 
 * The factory uses the UniversalEngineRegistry to detect games and engines,
 * then creates the optimal adapter with pre-configured VR settings.
 */
class UniversalVRFactory {
public:
    // Adapter creation function type
    using AdapterCreator = std::function<std::shared_ptr<ICrossEngineAdapter>(const std::string&, EngineType, const std::string&)>;
    
    // Factory configuration
    struct FactoryConfig {
        bool enableAutoDetection;
        bool enableFallbackAdapters;
        bool enablePerformanceOptimization;
        bool enableQualityAdaptation;
        bool enableCrossEngineCompatibility;
        std::string defaultAdapterType;
        std::vector<std::string> preferredEngines;
    };

    /**
     * @brief Initialize the universal VR factory
     * @param config Factory configuration
     * @return true if initialization successful
     */
    static bool initialize(const FactoryConfig& config = FactoryConfig{});
    
    /**
     * @brief Cleanup the universal VR factory
     */
    static void cleanup();
    
    /**
     * @brief Register an adapter creator for a specific engine type
     * @param engineType Engine type to register for
     * @param creator Function to create the adapter
     */
    static void registerAdapterCreator(EngineType engineType, AdapterCreator creator);
    
    /**
     * @brief Create a universal VR adapter for the detected game/engine
     * @return Shared pointer to the created adapter, or nullptr if creation failed
     */
    static std::shared_ptr<ICrossEngineAdapter> createUniversalAdapter();
    
    /**
     * @brief Create a VR adapter for a specific game/engine combination
     * @param gameName Name of the game
     * @param engineType Type of engine
     * @return Shared pointer to the created adapter, or nullptr if creation failed
     */
    static std::shared_ptr<ICrossEngineAdapter> createAdapter(const std::string& gameName, EngineType engineType);
    
    /**
     * @brief Auto-detect and create the optimal adapter
     * @return Shared pointer to the created adapter, or nullptr if creation failed
     */
    static std::shared_ptr<ICrossEngineAdapter> autoDetectAndCreate();
    
    /**
     * @brief Get the optimal VR configuration for a game
     * @param gameName Name of the game
     * @return VR configuration optimized for the game
     */
    static UniversalEngineRegistry::UniversalVRConfig getOptimalVRConfig(const std::string& gameName);
    
    /**
     * @brief Check if an engine type is supported
     * @param engineType Engine type to check
     * @return true if the engine type is supported
     */
    static bool isEngineSupported(EngineType engineType);
    
    /**
     * @brief Get all supported engine types
     * @return Vector of supported engine types
     */
    static std::vector<EngineType> getSupportedEngineTypes();
    
    /**
     * @brief Get factory statistics
     * @return String containing factory statistics
     */
    static std::string getFactoryStatistics();

private:
    static std::unordered_map<EngineType, AdapterCreator> s_adapterCreators;
    static FactoryConfig s_config;
    static bool s_initialized;
    static size_t s_adaptersCreated;
    static size_t s_successfulCreations;
    static size_t s_failedCreations;
    
    // Internal methods
    static bool initializeDefaultAdapterCreators();
    static std::shared_ptr<ICrossEngineAdapter> createFallbackAdapter(EngineType engineType);
    static bool validateAdapterCreation(const std::shared_ptr<ICrossEngineAdapter>& adapter);
    static std::string getEngineTypeString(EngineType engineType);
    static void logFactoryEvent(const std::string& event, bool success = true);
};

} // namespace uevr


