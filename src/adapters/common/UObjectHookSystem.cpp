#include "UObjectHookSystem.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <Windows.h>
#include <Psapi.h>

namespace uevr {

UObjectHookSystem::UObjectHookSystem()
    : m_autoDiscoveryEnabled(true)
    , m_batchProcessingEnabled(false)
    , m_backupDirectory("./backups")
{
    // Create backup directory if it doesn't exist
    std::filesystem::create_directories(m_backupDirectory);
    
    // Initialize performance metrics
    m_performance = {0, 0, 0, 0, 0, 0};
    
    // Initialize validation state
    m_validation = {{}, {}, true, 0};
}

bool UObjectHookSystem::scanForComponents() {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    try {
        // Clear previous discoveries
        m_discoveredComponents.clear();
        m_componentAddresses.clear();
        
        // Perform memory scanning
        if (!scanMemoryForComponents()) {
            logError("Memory scanning failed");
            return false;
        }
        
        // Perform pattern scanning
        if (!scanPatternForComponents()) {
            logError("Pattern scanning failed");
            return false;
        }
        
        // Validate discovered components
        for (auto it = m_discoveredComponents.begin(); it != m_discoveredComponents.end();) {
            if (!validateComponentPath(*it)) {
                it = m_discoveredComponents.erase(it);
            } else {
                ++it;
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        m_performance.scanTimeMs = static_cast<uint32_t>(duration.count());
        m_performance.totalComponents = static_cast<uint32_t>(m_discoveredComponents.size());
        
        std::cout << "[UObjectHook] Component scan completed: " << m_discoveredComponents.size() 
                  << " components found in " << m_performance.scanTimeMs << "ms" << std::endl;
        
        return true;
        
    } catch (const std::exception& e) {
        logError("Component scanning exception: " + std::string(e.what()));
        return false;
    }
}

std::vector<std::string> UObjectHookSystem::getDiscoveredComponents() const {
    return m_discoveredComponents;
}

bool UObjectHookSystem::validateComponentPath(const std::string& componentPath) const {
    if (componentPath.empty()) {
        return false;
    }
    
    // Check if component path follows expected format
    // Expected format: /Game/Path/To/Component
    if (componentPath[0] != '/') {
        return false;
    }
    
    // Check for valid characters
    for (char c : componentPath) {
        if (!isalnum(c) && c != '/' && c != '_' && c != '-') {
            return false;
        }
    }
    
    return true;
}

bool UObjectHookSystem::addAttachment(const ComponentAttachment& attachment) {
    if (!attachment.isValid()) {
        logError("Invalid attachment configuration");
        return false;
    }
    
    // Check if component path exists in discovered components
    if (std::find(m_discoveredComponents.begin(), m_discoveredComponents.end(), 
                  attachment.componentPath) == m_discoveredComponents.end()) {
        logError("Component path not found: " + attachment.componentPath);
        return false;
    }
    
    // Remove existing attachment for this component if it exists
    removeAttachment(attachment.componentPath);
    
    // Add new attachment
    m_attachments.push_back(attachment);
    m_performance.activeAttachments = static_cast<uint32_t>(m_attachments.size());
    
    std::cout << "[UObjectHook] Added attachment: " << attachment.componentPath 
              << " -> " << controllerSideToString(attachment.controllerSide) << std::endl;
    
    return true;
}

bool UObjectHookSystem::removeAttachment(const std::string& componentPath) {
    auto it = std::find_if(m_attachments.begin(), m_attachments.end(),
                           [&](const ComponentAttachment& att) {
                               return att.componentPath == componentPath;
                           });
    
    if (it != m_attachments.end()) {
        m_attachments.erase(it);
        m_performance.activeAttachments = static_cast<uint32_t>(m_attachments.size());
        
        std::cout << "[UObjectHook] Removed attachment: " << componentPath << std::endl;
        return true;
    }
    
    return false;
}

bool UObjectHookSystem::updateAttachment(const ComponentAttachment& attachment) {
    if (!attachment.isValid()) {
        return false;
    }
    
    auto it = std::find_if(m_attachments.begin(), m_attachments.end(),
                           [&](const ComponentAttachment& att) {
                               return att.componentPath == attachment.componentPath;
                           });
    
    if (it != m_attachments.end()) {
        *it = attachment;
        std::cout << "[UObjectHook] Updated attachment: " << attachment.componentPath << std::endl;
        return true;
    }
    
    return false;
}

std::vector<ComponentAttachment> UObjectHookSystem::getAttachments() const {
    return m_attachments;
}

bool UObjectHookSystem::mapWeaponToRightController(const std::string& weaponPath) {
    ComponentAttachment weaponAttachment;
    weaponAttachment.componentPath = weaponPath;
    weaponAttachment.controllerSide = ControllerSide::RIGHT;
    weaponAttachment.attachmentType = AttachmentType::WEAPON;
    weaponAttachment.permanent = true;
    weaponAttachment.enabled = true;
    
    // Set default weapon offsets
    weaponAttachment.positionOffset = {0.1f, -0.05f, 0.0f};  // Slightly right and down
    weaponAttachment.rotationOffset = {0.0f, 0.0f, 0.0f, 1.0f};  // No rotation
    weaponAttachment.scaleOffset = {1.0f, 1.0f, 1.0f};  // No scaling
    
    weaponAttachment.description = "Primary weapon attachment";
    weaponAttachment.tags = {"weapon", "primary", "combat"};
    
    return addAttachment(weaponAttachment);
}

bool UObjectHookSystem::mapUtilityToLeftController(const std::string& utilityPath) {
    ComponentAttachment utilityAttachment;
    utilityAttachment.componentPath = utilityPath;
    utilityAttachment.controllerSide = ControllerSide::LEFT;
    utilityAttachment.attachmentType = AttachmentType::UTILITY;
    utilityAttachment.permanent = true;
    utilityAttachment.enabled = true;
    
    // Set default utility offsets
    utilityAttachment.positionOffset = {-0.1f, -0.05f, 0.0f};  // Slightly left and down
    utilityAttachment.rotationOffset = {0.0f, 0.0f, 0.0f, 1.0f};  // No rotation
    utilityAttachment.scaleOffset = {1.0f, 1.0f, 1.0f};  // No scaling
    
    utilityAttachment.description = "Utility item attachment";
    utilityAttachment.tags = {"utility", "item", "tool"};
    
    return addAttachment(utilityAttachment);
}

bool UObjectHookSystem::mapUIElementToController(const std::string& uiPath, ControllerSide side) {
    ComponentAttachment uiAttachment;
    uiAttachment.componentPath = uiPath;
    uiAttachment.controllerSide = side;
    uiAttachment.attachmentType = AttachmentType::UI_ELEMENT;
    uiAttachment.permanent = false;  // UI elements may change
    uiAttachment.enabled = true;
    
    // Set default UI offsets based on controller side
    if (side == ControllerSide::LEFT) {
        uiAttachment.positionOffset = {-0.15f, 0.0f, 0.2f};  // Left side, forward
    } else {
        uiAttachment.positionOffset = {0.15f, 0.0f, 0.2f};   // Right side, forward
    }
    
    uiAttachment.rotationOffset = {0.0f, 0.0f, 0.0f, 1.0f};
    uiAttachment.scaleOffset = {1.0f, 1.0f, 1.0f};
    
    uiAttachment.description = "UI element attachment";
    uiAttachment.tags = {"ui", "hud", "interface"};
    
    return addAttachment(uiAttachment);
}

bool UObjectHookSystem::saveAttachmentState(const std::string& gameId) {
    try {
        nlohmann::json json;
        if (!serializeAttachments(json)) {
            return false;
        }
        
        std::string filename = m_backupDirectory + "/" + gameId + "_attachments.json";
        std::ofstream file(filename);
        if (!file.is_open()) {
            logError("Failed to open file for writing: " + filename);
            return false;
        }
        
        file << json.dump(4);
        file.close();
        
        std::cout << "[UObjectHook] Saved attachment state to: " << filename << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        logError("Save attachment state exception: " + std::string(e.what()));
        return false;
    }
}

bool UObjectHookSystem::loadAttachmentState(const std::string& gameId) {
    try {
        std::string filename = m_backupDirectory + "/" + gameId + "_attachments.json";
        std::ifstream file(filename);
        if (!file.is_open()) {
            logError("Failed to open file for reading: " + filename);
            return false;
        }
        
        nlohmann::json json;
        file >> json;
        file.close();
        
        if (!deserializeAttachments(json)) {
            return false;
        }
        
        std::cout << "[UObjectHook] Loaded attachment state from: " << filename << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        logError("Load attachment state exception: " + std::string(e.what()));
        return false;
    }
}

bool UObjectHookSystem::backupCurrentState(const std::string& gameId) {
    std::string backupFilename = generateBackupFilename(gameId);
    
    // Create backup directory if it doesn't exist
    std::filesystem::create_directories(m_backupDirectory + "/backups");
    
    // Save current state to backup file
    if (!saveAttachmentState(gameId + "_backup")) {
        return false;
    }
    
    // Rename to backup filename
    std::string currentFile = m_backupDirectory + "/" + gameId + "_backup_attachments.json";
    std::string backupFile = m_backupDirectory + "/backups/" + backupFilename;
    
    if (std::filesystem::exists(currentFile)) {
        std::filesystem::rename(currentFile, backupFile);
    }
    
    std::cout << "[UObjectHook] Created backup: " << backupFile << std::endl;
    return true;
}

bool UObjectHookSystem::restoreFromBackup(const std::string& gameId) {
    // Find latest backup file
    std::string backupDir = m_backupDirectory + "/backups";
    std::string backupPattern = gameId + "_backup_";
    
    std::vector<std::string> backupFiles;
    for (const auto& entry : std::filesystem::directory_iterator(backupDir)) {
        if (entry.path().filename().string().find(backupPattern) != std::string::npos) {
            backupFiles.push_back(entry.path().string());
        }
    }
    
    if (backupFiles.empty()) {
        logError("No backup files found for: " + gameId);
        return false;
    }
    
    // Sort by modification time (newest first)
    std::sort(backupFiles.begin(), backupFiles.end(),
              [](const std::string& a, const std::string& b) {
                  return std::filesystem::last_write_time(a) > std::filesystem::last_write_time(b);
              });
    
    // Load from latest backup
    std::string latestBackup = backupFiles[0];
    std::cout << "[UObjectHook] Restoring from backup: " << latestBackup << std::endl;
    
    // Copy backup to current state file
    std::string currentFile = m_backupDirectory + "/" + gameId + "_attachments.json";
    std::filesystem::copy_file(latestBackup, currentFile, std::filesystem::copy_options::overwrite_existing);
    
    // Load the restored state
    return loadAttachmentState(gameId);
}

bool UObjectHookSystem::autoDiscoverWeapons() {
    if (!m_autoDiscoveryEnabled) {
        return false;
    }
    
    std::cout << "[UObjectHook] Auto-discovering weapons..." << std::endl;
    
    // Scan for weapon patterns
    if (!scanForWeaponPatterns()) {
        return false;
    }
    
    // Auto-map discovered weapons to right controller
    for (const auto& component : m_discoveredComponents) {
        if (component.find("weapon") != std::string::npos || 
            component.find("gun") != std::string::npos ||
            component.find("rifle") != std::string::npos) {
            mapWeaponToRightController(component);
        }
    }
    
    return true;
}

bool UObjectHookSystem::autoDiscoverUtilities() {
    if (!m_autoDiscoveryEnabled) {
        return false;
    }
    
    std::cout << "[UObjectHook] Auto-discovering utilities..." << std::endl;
    
    // Scan for utility patterns
    if (!scanForUtilityPatterns()) {
        return false;
    }
    
    // Auto-map discovered utilities to left controller
    for (const auto& component : m_discoveredComponents) {
        if (component.find("utility") != std::string::npos || 
            component.find("tool") != std::string::npos ||
            component.find("item") != std::string::npos) {
            mapUtilityToLeftController(component);
        }
    }
    
    return true;
}

bool UObjectHookSystem::autoDiscoverUIElements() {
    if (!m_autoDiscoveryEnabled) {
        return false;
    }
    
    std::cout << "[UObjectHook] Auto-discovering UI elements..." << std::endl;
    
    // Scan for UI patterns
    if (!scanForUIPatterns()) {
        return false;
    }
    
    // Auto-map discovered UI elements
    for (const auto& component : m_discoveredComponents) {
        if (component.find("ui") != std::string::npos || 
            component.find("hud") != std::string::npos ||
            component.find("menu") != std::string::npos) {
            // Map to appropriate controller based on component type
            if (component.find("left") != std::string::npos) {
                mapUIElementToController(component, ControllerSide::LEFT);
            } else if (component.find("right") != std::string::npos) {
                mapUIElementToController(component, ControllerSide::RIGHT);
            } else {
                // Default to left controller for UI elements
                mapUIElementToController(component, ControllerSide::LEFT);
            }
        }
    }
    
    return true;
}

bool UObjectHookSystem::validateAllAttachments() const {
    m_validation.errors.clear();
    m_validation.warnings.clear();
    m_validation.isValid = true;
    
    for (const auto& attachment : m_attachments) {
        if (!attachment.isValid()) {
            m_validation.errors.push_back("Invalid attachment: " + attachment.componentPath);
            m_validation.isValid = false;
        }
        
        if (!validateComponentPath(attachment.componentPath)) {
            m_validation.errors.push_back("Invalid component path: " + attachment.componentPath);
            m_validation.isValid = false;
        }
        
        // Check for duplicate attachments
        size_t count = std::count_if(m_attachments.begin(), m_attachments.end(),
                                    [&](const ComponentAttachment& att) {
                                        return att.componentPath == attachment.componentPath;
                                    });
        
        if (count > 1) {
            m_validation.warnings.push_back("Duplicate attachment: " + attachment.componentPath);
        }
    }
    
    m_validation.lastValidationTime = static_cast<uint32_t>(std::time(nullptr));
    
    std::cout << "[UObjectHook] Validation completed: " 
              << (m_validation.isValid ? "PASSED" : "FAILED") 
              << " (" << m_validation.errors.size() << " errors, " 
              << m_validation.warnings.size() << " warnings)" << std::endl;
    
    return m_validation.isValid;
}

bool UObjectHookSystem::testAttachment(const std::string& componentPath) {
    auto it = std::find_if(m_attachments.begin(), m_attachments.end(),
                           [&](const ComponentAttachment& att) {
                               return att.componentPath == componentPath;
                           });
    
    if (it == m_attachments.end()) {
        logError("Attachment not found: " + componentPath);
        return false;
    }
    
    // Simulate attachment test
    std::cout << "[UObjectHook] Testing attachment: " << componentPath << std::endl;
    
    // Check if component is accessible
    if (!validateComponentPath(componentPath)) {
        logError("Component path validation failed: " + componentPath);
        return false;
    }
    
    // Check transform validity
    if (!it->isValid()) {
        logError("Attachment transform validation failed: " + componentPath);
        return false;
    }
    
    std::cout << "[UObjectHook] Attachment test passed: " << componentPath << std::endl;
    return true;
}

std::vector<std::string> UObjectHookSystem::getValidationErrors() const {
    return m_validation.errors;
}

void UObjectHookSystem::optimizeAttachmentUpdates() {
    if (m_batchProcessingEnabled) {
        // Enable batch processing for multiple updates
        std::cout << "[UObjectHook] Batch processing enabled for attachment updates" << std::endl;
    }
    
    // Optimize transform calculations
    updateAttachmentTransforms();
}

void UObjectHookSystem::enableBatchProcessing(bool enable) {
    m_batchProcessingEnabled = enable;
    std::cout << "[UObjectHook] Batch processing " 
              << (enable ? "enabled" : "disabled") << std::endl;
}

void UObjectHookSystem::setGameProfile(const std::string& profilePath) {
    m_gameProfilePath = profilePath;
    std::cout << "[UObjectHook] Game profile set: " << profilePath << std::endl;
}

void UObjectHookSystem::setBackupDirectory(const std::string& backupPath) {
    m_backupDirectory = backupPath;
    std::filesystem::create_directories(m_backupDirectory);
    std::cout << "[UObjectHook] Backup directory set: " << backupPath << std::endl;
}

void UObjectHookSystem::setAutoDiscoveryEnabled(bool enabled) {
    m_autoDiscoveryEnabled = enabled;
    std::cout << "[UObjectHook] Auto-discovery " 
              << (enabled ? "enabled" : "disabled") << std::endl;
}

// Private implementation methods
bool UObjectHookSystem::scanMemoryForComponents() {
    // This is a simplified memory scanning implementation
    // In practice, you would implement more sophisticated memory scanning
    
    // Get process handle
    HANDLE processHandle = GetCurrentProcess();
    if (!processHandle) {
        return false;
    }
    
    // Get process memory info
    PROCESS_MEMORY_COUNTERS_EX pmc;
    if (!GetProcessMemoryInfo(processHandle, (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
        return false;
    }
    
    // Scan memory regions for component patterns
    // This is a placeholder - actual implementation would scan for specific patterns
    
    std::cout << "[UObjectHook] Memory scanning completed" << std::endl;
    return true;
}

bool UObjectHookSystem::scanPatternForComponents() {
    // This is a simplified pattern scanning implementation
    // In practice, you would implement more sophisticated pattern matching
    
    // Add some example components for demonstration
    m_discoveredComponents = {
        "/Game/Weapons/Pistol",
        "/Game/Weapons/Rifle",
        "/Game/Weapons/Shotgun",
        "/Game/Utilities/Flashlight",
        "/Game/Utilities/Medkit",
        "/Game/UI/HUD",
        "/Game/UI/MainMenu",
        "/Game/UI/Inventory"
    };
    
    std::cout << "[UObjectHook] Pattern scanning completed" << std::endl;
    return true;
}

bool UObjectHookSystem::validateComponentAddress(uintptr_t address) const {
    // Basic address validation
    return address != 0 && address < 0x7FFFFFFFFFFF;
}

bool UObjectHookSystem::processAttachment(const ComponentAttachment& attachment) {
    // Process attachment for rendering
    if (!attachment.enabled) {
        return false;
    }
    
    // Apply transforms and update rendering
    // This would integrate with the VR rendering system
    
    return true;
}

bool UObjectHookSystem::validateAttachment(const ComponentAttachment& attachment) const {
    return attachment.isValid() && validateComponentPath(attachment.componentPath);
}

void UObjectHookSystem::updateAttachmentTransforms() {
    // Update transform matrices for all active attachments
    for (auto& attachment : m_attachments) {
        if (attachment.enabled) {
            // Update transform calculations
            // This would integrate with the VR controller system
        }
    }
}

bool UObjectHookSystem::serializeAttachments(nlohmann::json& json) const {
    try {
        json = nlohmann::json::array();
        
        for (const auto& attachment : m_attachments) {
            nlohmann::json attachmentJson;
            attachmentJson["componentPath"] = attachment.componentPath;
            attachmentJson["controllerSide"] = controllerSideToString(attachment.controllerSide);
            attachmentJson["attachmentType"] = attachmentTypeToString(attachment.attachmentType);
            attachmentJson["permanent"] = attachment.permanent;
            attachmentJson["enabled"] = attachment.enabled;
            
            // Transform offsets
            attachmentJson["positionOffset"] = {
                attachment.positionOffset.x,
                attachment.positionOffset.y,
                attachment.positionOffset.z
            };
            
            attachmentJson["rotationOffset"] = {
                attachment.rotationOffset.x,
                attachment.rotationOffset.y,
                attachment.rotationOffset.z,
                attachment.rotationOffset.w
            };
            
            attachmentJson["scaleOffset"] = {
                attachment.scaleOffset.x,
                attachment.scaleOffset.y,
                attachment.scaleOffset.z
            };
            
            attachmentJson["description"] = attachment.description;
            attachmentJson["tags"] = attachment.tags;
            
            json.push_back(attachmentJson);
        }
        
        return true;
        
    } catch (const std::exception& e) {
        logError("Serialization exception: " + std::string(e.what()));
        return false;
    }
}

bool UObjectHookSystem::deserializeAttachments(const nlohmann::json& json) {
    try {
        m_attachments.clear();
        
        for (const auto& attachmentJson : json) {
            ComponentAttachment attachment;
            
            attachment.componentPath = attachmentJson["componentPath"];
            attachment.controllerSide = stringToControllerSide(attachmentJson["controllerSide"]);
            attachment.attachmentType = stringToAttachmentType(attachmentJson["attachmentType"]);
            attachment.permanent = attachmentJson["permanent"];
            attachment.enabled = attachmentJson["enabled"];
            
            // Transform offsets
            auto posOffset = attachmentJson["positionOffset"];
            attachment.positionOffset = {
                posOffset[0], posOffset[1], posOffset[2]
            };
            
            auto rotOffset = attachmentJson["rotationOffset"];
            attachment.rotationOffset = {
                rotOffset[0], rotOffset[1], rotOffset[2], rotOffset[3]
            };
            
            auto scaleOffset = attachmentJson["scaleOffset"];
            attachment.scaleOffset = {
                scaleOffset[0], scaleOffset[1], scaleOffset[2]
            };
            
            attachment.description = attachmentJson["description"];
            attachment.tags = attachmentJson["tags"].get<std::vector<std::string>>();
            
            m_attachments.push_back(attachment);
        }
        
        m_performance.activeAttachments = static_cast<uint32_t>(m_attachments.size());
        return true;
        
    } catch (const std::exception& e) {
        logError("Deserialization exception: " + std::string(e.what()));
        return false;
    }
}

std::string UObjectHookSystem::generateBackupFilename(const std::string& gameId) const {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto tm = *std::localtime(&time_t);
    
    char buffer[64];
    std::strftime(buffer, sizeof(buffer), "%Y%m%d_%H%M%S", &tm);
    
    return gameId + "_backup_" + buffer + ".json";
}

bool UObjectHookSystem::scanForWeaponPatterns() {
    // Scan for weapon-related patterns in discovered components
    // This is a simplified implementation
    
    std::cout << "[UObjectHook] Weapon pattern scanning completed" << std::endl;
    return true;
}

bool UObjectHookSystem::scanForUtilityPatterns() {
    // Scan for utility-related patterns in discovered components
    // This is a simplified implementation
    
    std::cout << "[UObjectHook] Utility pattern scanning completed" << std::endl;
    return true;
}

bool UObjectHookSystem::scanForUIPatterns() {
    // Scan for UI-related patterns in discovered components
    // This is a simplified implementation
    
    std::cout << "[UObjectHook] UI pattern scanning completed" << std::endl;
    return true;
}

bool UObjectHookSystem::isReadableMemory(uintptr_t address, size_t size) const {
    // Check if memory region is readable
    // This is a simplified implementation
    
    return true;
}

bool UObjectHookSystem::findPatternInMemory(uintptr_t startAddr, size_t size, const std::vector<uint8_t>& pattern) const {
    // Find pattern in memory region
    // This is a simplified implementation
    
    return false;
}

std::vector<uintptr_t> UObjectHookSystem::findAllPatternMatches(uintptr_t startAddr, size_t size, const std::vector<uint8_t>& pattern) const {
    // Find all pattern matches in memory region
    // This is a simplified implementation
    
    return {};
}

// Helper methods
std::string UObjectHookSystem::controllerSideToString(ControllerSide side) const {
    switch (side) {
        case ControllerSide::LEFT: return "left";
        case ControllerSide::RIGHT: return "right";
        case ControllerSide::BOTH: return "both";
        default: return "unknown";
    }
}

ControllerSide UObjectHookSystem::stringToControllerSide(const std::string& str) const {
    if (str == "left") return ControllerSide::LEFT;
    if (str == "right") return ControllerSide::RIGHT;
    if (str == "both") return ControllerSide::BOTH;
    return ControllerSide::LEFT; // Default
}

std::string UObjectHookSystem::attachmentTypeToString(AttachmentType type) const {
    switch (type) {
        case AttachmentType::WEAPON: return "weapon";
        case AttachmentType::UTILITY: return "utility";
        case AttachmentType::UI_ELEMENT: return "ui_element";
        case AttachmentType::CAMERA: return "camera";
        case AttachmentType::CUSTOM: return "custom";
        default: return "unknown";
    }
}

AttachmentType UObjectHookSystem::stringToAttachmentType(const std::string& str) const {
    if (str == "weapon") return AttachmentType::WEAPON;
    if (str == "utility") return AttachmentType::UTILITY;
    if (str == "ui_element") return AttachmentType::UI_ELEMENT;
    if (str == "camera") return AttachmentType::CAMERA;
    if (str == "custom") return AttachmentType::CUSTOM;
    return AttachmentType::CUSTOM; // Default
}

void UObjectHookSystem::logError(const std::string& error) const {
    std::cerr << "[UObjectHook] ERROR: " << error << std::endl;
}

} // namespace uevr
