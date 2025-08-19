#pragma once

#include <memory>
#include <string>
#include <vector>
#include <mutex>

namespace uevr {

// Forward declarations
class API;
class Plugin;

/**
 * @brief Cross-Engine VR Adapter Interface
 * 
 * This interface defines the contract for cross-engine VR adapters
 * that integrate with uevr's plugin architecture to provide VR support
 * for non-Unreal Engine games.
 */
class ICrossEngineAdapter {
public:
    virtual ~ICrossEngineAdapter() = default;

    /**
     * @brief Initialize the cross-engine adapter
     * @return true if initialization successful, false otherwise
     */
    virtual bool initialize() = 0;

    /**
     * @brief Clean up resources and cleanup the adapter
     */
    virtual void cleanup() = 0;

    /**
     * @brief Check if VR is enabled and functional
     * @return true if VR is enabled, false otherwise
     */
    virtual bool isVREnabled() const = 0;

    /**
     * @brief Update the adapter state (called each frame)
     */
    virtual void update() = 0;

    /**
     * @brief Render the VR frame
     */
    virtual void render() = 0;

    /**
     * @brief Get the adapter name
     * @return Adapter name string
     */
    virtual std::string getName() const = 0;

    /**
     * @brief Get the engine type this adapter supports
     * @return Engine type string
     */
    virtual std::string getEngineType() const = 0;

    /**
     * @brief Get the game executable name
     * @return Game executable name string
     */
    virtual std::string getGameExecutable() const = 0;

    /**
     * @brief Get the adapter version
     * @return Version string
     */
    virtual std::string getVersion() const = 0;
};

/**
 * @brief Cross-Engine Adapter Factory Interface
 * 
 * Factory interface for creating cross-engine adapters
 */
class ICrossEngineAdapterFactory {
public:
    virtual ~ICrossEngineAdapterFactory() = default;

    /**
     * @brief Create a cross-engine adapter for the specified game
     * @param gameExecutable Game executable name
     * @return Shared pointer to the created adapter, or nullptr if creation failed
     */
    virtual std::shared_ptr<ICrossEngineAdapter> createAdapter(const std::string& gameExecutable) = 0;

    /**
     * @brief Check if this factory can create an adapter for the specified game
     * @param gameExecutable Game executable name
     * @return true if this factory can create an adapter, false otherwise
     */
    virtual bool canCreateAdapter(const std::string& gameExecutable) const = 0;

    /**
     * @brief Get the engine type this factory supports
     * @return Engine type string
     */
    virtual std::string getSupportedEngineType() const = 0;
};

/**
 * @brief Cross-Engine Adapter Registry
 * 
 * Central registry for managing cross-engine adapters
 */
class CrossEngineAdapterRegistry {
public:
    /**
     * @brief Register a cross-engine adapter factory
     * @param factory Shared pointer to the factory to register
     */
    static void registerFactory(std::shared_ptr<ICrossEngineAdapterFactory> factory);

    /**
     * @brief Unregister a cross-engine adapter factory
     * @param factory Shared pointer to the factory to unregister
     */
    static void unregisterFactory(std::shared_ptr<ICrossEngineAdapterFactory> factory);

    /**
     * @brief Create an adapter for the specified game
     * @param gameExecutable Game executable name
     * @return Shared pointer to the created adapter, or nullptr if creation failed
     */
    static std::shared_ptr<ICrossEngineAdapter> createAdapter(const std::string& gameExecutable);

    /**
     * @brief Get all registered factories
     * @return Vector of registered factory pointers
     */
    static std::vector<std::shared_ptr<ICrossEngineAdapterFactory>> getRegisteredFactories();

    /**
     * @brief Clear all registered factories
     */
    static void clearFactories();

private:
    static std::vector<std::shared_ptr<ICrossEngineAdapterFactory>> s_factories;
    static std::mutex s_factoriesMutex;
};

} // namespace uevr
