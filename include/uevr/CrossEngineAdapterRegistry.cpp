#include "ICrossEngineAdapter.hpp"
#include <algorithm>
#include <mutex>

namespace uevr {

// Static member initialization
std::vector<std::shared_ptr<ICrossEngineAdapterFactory>> CrossEngineAdapterRegistry::s_factories;
std::mutex CrossEngineAdapterRegistry::s_factoriesMutex;

void CrossEngineAdapterRegistry::registerFactory(std::shared_ptr<ICrossEngineAdapterFactory> factory) {
    std::lock_guard<std::mutex> lock(s_factoriesMutex);
    
    // Check if factory is already registered
    auto it = std::find_if(s_factories.begin(), s_factories.end(),
        [&factory](const std::shared_ptr<ICrossEngineAdapterFactory>& registered) {
            return registered.get() == factory.get();
        });
    
    if (it == s_factories.end()) {
        s_factories.push_back(factory);
    }
}

void CrossEngineAdapterRegistry::unregisterFactory(std::shared_ptr<ICrossEngineAdapterFactory> factory) {
    std::lock_guard<std::mutex> lock(s_factoriesMutex);
    
    auto it = std::find_if(s_factories.begin(), s_factories.end(),
        [&factory](const std::shared_ptr<ICrossEngineAdapterFactory>& registered) {
            return registered.get() == factory.get();
        });
    
    if (it != s_factories.end()) {
        s_factories.erase(it);
    }
}

std::shared_ptr<ICrossEngineAdapter> CrossEngineAdapterRegistry::createAdapter(const std::string& gameExecutable) {
    std::lock_guard<std::mutex> lock(s_factoriesMutex);
    
    // Try to create an adapter using registered factories
    for (const auto& factory : s_factories) {
        if (factory->canCreateAdapter(gameExecutable)) {
            auto adapter = factory->createAdapter(gameExecutable);
            if (adapter) {
                return adapter;
            }
        }
    }
    
    return nullptr;
}

std::vector<std::shared_ptr<ICrossEngineAdapterFactory>> CrossEngineAdapterRegistry::getRegisteredFactories() {
    std::lock_guard<std::mutex> lock(s_factoriesMutex);
    return s_factories;
}

void CrossEngineAdapterRegistry::clearFactories() {
    std::lock_guard<std::mutex> lock(s_factoriesMutex);
    s_factories.clear();
}

} // namespace uevr
