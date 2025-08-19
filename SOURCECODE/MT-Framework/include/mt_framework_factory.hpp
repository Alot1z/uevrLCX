#pragma once

#include <uevr/ICrossEngineAdapter.hpp>
#include <memory>

namespace uevr {
namespace MTFramework {

/**
 * @brief Factory for creating MT Framework VR adapters
 * 
 * This factory provides a standardized way to create MT Framework VR adapters
 * for different games. It implements the factory pattern and provides automatic
 * adapter instantiation with proper configuration.
 * 
 * @version 1.0.0
 * @author uevr Cross-Engine Adapter Project
 */
class MTFrameworkFactory {
public:
    /**
     * @brief Create a new MT Framework adapter instance
     * @return Unique pointer to the created adapter
     */
    static std::unique_ptr<ICrossEngineAdapter> createAdapter();

    /**
     * @brief Create a new MT Framework adapter with specific configuration
     * @param configPath Path to configuration file
     * @return Unique pointer to the created adapter, or nullptr if failed
     */
    static std::unique_ptr<ICrossEngineAdapter> createAdapterWithConfig(const std::string& configPath);

    /**
     * @brief Create a new MT Framework adapter for a specific game
     * @param gameName Name of the target game
     * @return Unique pointer to the created adapter, or nullptr if failed
     */
    static std::unique_ptr<ICrossEngineAdapter> createAdapterForGame(const std::string& gameName);

    /**
     * @brief Check if MT Framework is supported on this system
     * @return true if MT Framework is supported
     */
    static bool isSupported();

    /**
     * @brief Get supported game list
     * @return Vector of supported game names
     */
    static std::vector<std::string> getSupportedGames();

    /**
     * @brief Get factory version
     * @return Factory version string
     */
    static std::string getVersion();

    /**
     * @brief Get factory description
     * @return Factory description string
     */
    static std::string getDescription();

private:
    MTFrameworkFactory() = delete;
    ~MTFrameworkFactory() = delete;
    MTFrameworkFactory(const MTFrameworkFactory&) = delete;
    MTFrameworkFactory& operator=(const MTFrameworkFactory&) = delete;

    // Internal helper methods
    static bool validateSystemRequirements();
    static std::string detectGameType();
    static bool loadGameConfiguration(const std::string& gameName);
};

} // namespace MTFramework
} // namespace uevr
