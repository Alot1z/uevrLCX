#pragma once

#include "BaseCrossEngineAdapter.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <functional>
#include <vector>
#include <chrono>
#include <iostream>
#include <stdexcept>
#include <algorithm>

namespace uevr {

/**
 * @brief Factory for creating cross-engine VR adapters
 * 
 * Provides dynamic creation of game-specific adapters based on
 * engine detection and configuration.
 */
class CrossEngineAdapterFactory {
public:
    using AdapterCreator = std::function<std::unique_ptr<BaseCrossEngineAdapter>()>;
    
    CrossEngineAdapterFactory();
    ~CrossEngineAdapterFactory() = default;
    
    // Register adapter creators
    void registerAdapter(const std::string& engineName, AdapterCreator creator);
    
    // Create adapter by engine name
    std::unique_ptr<BaseCrossEngineAdapter> createAdapter(const std::string& engineName);
    
    // Auto-detect engine and create appropriate adapter
    std::unique_ptr<BaseCrossEngineAdapter> autoDetectAndCreate();
    
    // Get list of supported engines
    std::vector<std::string> getSupportedEngines() const;
    
    // Check if engine is supported
    bool isEngineSupported(const std::string& engineName) const;
    
    // Get adapter information
    struct AdapterInfo {
        std::string engineName;
        std::string description;
        std::string version;
        std::vector<std::string> supportedGames;
        bool isActive;
    };
    
    AdapterInfo getAdapterInfo(const std::string& engineName) const;

private:
    // Registered adapter creators
    std::unordered_map<std::string, AdapterCreator> m_adapters;
    
    // Adapter information
    std::unordered_map<std::string, AdapterInfo> m_adapterInfo;
    
    // Engine detection methods
    bool detectREEngine() const;
    bool detectREDengine4() const;
    bool detectMTFramework() const;
    
    // Initialize default adapters
    void initializeDefaultAdapters();
};

} // namespace uevr
