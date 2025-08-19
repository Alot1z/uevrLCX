#pragma once

#include <uevr/ICrossEngineAdapter.hpp>
#include "mt_framework_adapter.hpp"
#include <memory>
#include <string>

namespace uevr {
namespace MTFramework {

/**
 * @brief MT Framework Cross-Engine Adapter Factory
 * 
 * This factory creates Monster Hunter World VR adapters that integrate
 * with the MT Framework engine using uevr's cross-engine framework.
 */
class MTFrameworkAdapterFactory : public ICrossEngineAdapterFactory {
public:
    MTFrameworkAdapterFactory();
    virtual ~MTFrameworkAdapterFactory();
    
    // ICrossEngineAdapterFactory implementation
    std::shared_ptr<ICrossEngineAdapter> createAdapter(const std::string& gameExecutable) override;
    bool canCreateAdapter(const std::string& gameExecutable) const override;
    std::string getSupportedEngineType() const override;
    
private:
    // Game detection methods
    bool isMonsterHunterWorld(const std::string& gameExecutable) const;
    bool isMTFrameworkGame(const std::string& gameExecutable) const;
    
    // Engine detection
    bool detectMTFramework() const;
};

} // namespace MTFramework
} // namespace uevr
