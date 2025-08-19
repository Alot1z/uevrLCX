#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <DirectXMath.h>
#include <nlohmann/json.hpp>

namespace uevr {

/**
 * @brief Controller side enumeration
 */
enum class ControllerSide {
    LEFT,
    RIGHT,
    BOTH
};

/**
 * @brief Attachment type enumeration
 */
enum class AttachmentType {
    WEAPON,         // Primary weapon attachment
    UTILITY,        // Utility item attachment
    UI_ELEMENT,     // UI element attachment
    CAMERA,         // Camera attachment
    CUSTOM          // Custom attachment
};

/**
 * @brief Component attachment configuration
 */
struct ComponentAttachment {
    std::string componentPath;           // Path to the component in the game object hierarchy
    ControllerSide controllerSide;       // Which controller to attach to
    AttachmentType attachmentType;       // Type of attachment
    bool permanent;                      // Whether attachment persists across game states
    
    // Transform offsets
    DirectX::XMFLOAT3 positionOffset;   // Position offset from controller
    DirectX::XMFLOAT4 rotationOffset;   // Rotation offset (quaternion)
    DirectX::XMFLOAT3 scaleOffset;      // Scale offset
    
    // Additional properties
    std::string description;             // Human-readable description
    std::vector<std::string> tags;      // Searchable tags
    bool enabled;                        // Whether this attachment is active
    
    // Validation
    bool isValid() const {
        return !componentPath.empty() && 
               (positionOffset.x != 0.0f || positionOffset.y != 0.0f || positionOffset.z != 0.0f ||
                rotationOffset.x != 0.0f || rotationOffset.y != 0.0f || rotationOffset.z != 0.0f || rotationOffset.w != 0.0f);
    }
};

/**
 * @brief UObjectHook system for component discovery and management
 */
class UObjectHookSystem {
public:
    UObjectHookSystem();
    ~UObjectHookSystem() = default;
    
    // Component discovery
    bool scanForComponents();
    std::vector<std::string> getDiscoveredComponents() const;
    bool validateComponentPath(const std::string& componentPath) const;
    
    // Attachment management
    bool addAttachment(const ComponentAttachment& attachment);
    bool removeAttachment(const std::string& componentPath);
    bool updateAttachment(const ComponentAttachment& attachment);
    std::vector<ComponentAttachment> getAttachments() const;
    
    // Controller mapping
    bool mapWeaponToRightController(const std::string& weaponPath);
    bool mapUtilityToLeftController(const std::string& utilityPath);
    bool mapUIElementToController(const std::string& uiPath, ControllerSide side);
    
    // State persistence
    bool saveAttachmentState(const std::string& gameId);
    bool loadAttachmentState(const std::string& gameId);
    bool backupCurrentState(const std::string& gameId);
    bool restoreFromBackup(const std::string& gameId);
    
    // Auto-discovery
    bool autoDiscoverWeapons();
    bool autoDiscoverUtilities();
    bool autoDiscoverUIElements();
    
    // Validation and testing
    bool validateAllAttachments() const;
    bool testAttachment(const std::string& componentPath);
    std::vector<std::string> getValidationErrors() const;
    
    // Performance optimization
    void optimizeAttachmentUpdates();
    void enableBatchProcessing(bool enable);
    
    // Configuration
    void setGameProfile(const std::string& profilePath);
    void setBackupDirectory(const std::string& backupPath);
    void setAutoDiscoveryEnabled(bool enabled);

private:
    // Component discovery
    bool scanMemoryForComponents();
    bool scanPatternForComponents();
    bool validateComponentAddress(uintptr_t address) const;
    
    // Attachment processing
    bool processAttachment(const ComponentAttachment& attachment);
    bool validateAttachment(const ComponentAttachment& attachment) const;
    void updateAttachmentTransforms();
    
    // State management
    bool serializeAttachments(nlohmann::json& json) const;
    bool deserializeAttachments(const nlohmann::json& json);
    std::string generateBackupFilename(const std::string& gameId) const;
    
    // Auto-discovery patterns
    bool scanForWeaponPatterns();
    bool scanForUtilityPatterns();
    bool scanForUIPatterns();
    
    // Internal state
    std::vector<std::string> m_discoveredComponents;
    std::vector<ComponentAttachment> m_attachments;
    std::unordered_map<std::string, uintptr_t> m_componentAddresses;
    
    // Configuration
    std::string m_gameProfilePath;
    std::string m_backupDirectory;
    bool m_autoDiscoveryEnabled;
    bool m_batchProcessingEnabled;
    
    // Performance
    struct PerformanceMetrics {
        uint32_t scanTimeMs;
        uint32_t updateTimeMs;
        uint32_t validationTimeMs;
        uint32_t totalComponents;
        uint32_t activeAttachments;
    } m_performance;
    
    // Validation state
    struct ValidationState {
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        bool isValid;
        uint32_t lastValidationTime;
    } m_validation;
    
    // Helper methods
    std::string controllerSideToString(ControllerSide side) const;
    ControllerSide stringToControllerSide(const std::string& str) const;
    std::string attachmentTypeToString(AttachmentType type) const;
    AttachmentType stringToAttachmentType(const std::string& str) const;
    
    // Memory scanning helpers
    bool isReadableMemory(uintptr_t address, size_t size) const;
    bool findPatternInMemory(uintptr_t startAddr, size_t size, const std::vector<uint8_t>& pattern) const;
    std::vector<uintptr_t> findAllPatternMatches(uintptr_t startAddr, size_t size, const std::vector<uint8_t>& pattern) const;
};

/**
 * @brief Component discovery result
 */
struct ComponentDiscoveryResult {
    std::string componentPath;
    uintptr_t memoryAddress;
    std::string componentType;
    bool isValid;
    float confidence;                    // Confidence score (0.0 - 1.0)
    std::vector<std::string> tags;
    std::string description;
};

/**
 * @brief Auto-discovery configuration
 */
struct AutoDiscoveryConfig {
    bool enableWeaponDiscovery;
    bool enableUtilityDiscovery;
    bool enableUIDiscovery;
    bool enableCameraDiscovery;
    
    // Pattern matching
    std::vector<std::string> weaponPatterns;
    std::vector<std::string> utilityPatterns;
    std::vector<std::string> uiPatterns;
    std::vector<std::string> cameraPatterns;
    
    // Confidence thresholds
    float minConfidenceThreshold;
    float maxScanTimeMs;
    uint32_t maxComponentsPerType;
};

/**
 * @brief Attachment validation result
 */
struct AttachmentValidationResult {
    std::string componentPath;
    bool isValid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    float performanceImpact;
    bool recommended;
};

} // namespace uevr
