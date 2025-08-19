/*
 * UEVR Hook Manager Implementation
 * 
 * Centralized hook management system for graphics and input hooks
 * 
 * Copyright (c) 2024 UEVR Project
 * Licensed under MIT License
 */

#include "uevr/hooks/HookManager.hpp"
#include <spdlog/spdlog.h>
#include <Windows.h>
#include <detours.h>
#include <memory>
#include <unordered_map>
#include <vector>
#include <functional>
#include <mutex>
#include <algorithm>

namespace uevr {
namespace Hooks {

// Hook information structure
struct HookInfo {
    std::string name;
    void* original_function;
    void* hook_function;
    bool installed;
    std::string description;
    std::chrono::steady_clock::time_point install_time;
    uint32_t call_count;
    std::function<void()> pre_callback;
    std::function<void()> post_callback;
};

// Hook statistics
struct HookStats {
    uint32_t total_hooks;
    uint32_t installed_hooks;
    uint32_t failed_hooks;
    uint32_t total_calls;
    std::chrono::steady_clock::time_point last_update;
};

class HookManagerImpl {
public:
    HookManagerImpl() : m_initialized(false) {
        spdlog::info("[HookManager] Hook manager implementation created");
    }
    
    ~HookManagerImpl() {
        cleanup();
    }
    
    bool initialize() {
        if (m_initialized) {
            return true;
        }
        
        try {
            spdlog::info("[HookManager] Initializing hook manager...");
            
            // Initialize Detours
            DetourTransactionBegin();
            DetourUpdateThread(GetCurrentThread());
            
            m_initialized = true;
            spdlog::info("[HookManager] Hook manager initialized successfully");
            return true;
            
        } catch (const std::exception& e) {
            spdlog::error("[HookManager] Failed to initialize: {}", e.what());
            return false;
        }
    }
    
    void cleanup() {
        if (!m_initialized) {
            return;
        }
        
        spdlog::info("[HookManager] Cleaning up hook manager...");
        
        // Remove all hooks
        removeAllHooks();
        
        // Commit Detours transaction
        DetourTransactionCommit();
        
        m_initialized = false;
        spdlog::info("[HookManager] Hook manager cleanup completed");
    }
    
    bool installHook(const std::string& name, void* original_function, void* hook_function, 
                    const std::string& description = "") {
        if (!m_initialized) {
            spdlog::error("[HookManager] Cannot install hook - manager not initialized");
            return false;
        }
        
        if (!original_function || !hook_function) {
            spdlog::error("[HookManager] Invalid function pointers for hook: {}", name);
            return false;
        }
        
        // Check if hook already exists
        if (m_hooks.find(name) != m_hooks.end()) {
            spdlog::warn("[HookManager] Hook '{}' already exists", name);
            return false;
        }
        
        try {
            spdlog::info("[HookManager] Installing hook: {} at 0x{:x}", 
                        name, reinterpret_cast<uintptr_t>(original_function));
            
            // Create hook info
            HookInfo hook_info;
            hook_info.name = name;
            hook_info.original_function = original_function;
            hook_info.hook_function = hook_function;
            hook_info.description = description;
            hook_info.installed = false;
            hook_info.install_time = std::chrono::steady_clock::now();
            hook_info.call_count = 0;
            
            // Install hook using Detours
            LONG result = DetourAttach(&hook_info.original_function, hook_info.hook_function);
            if (result != NO_ERROR) {
                spdlog::error("[HookManager] Failed to install hook '{}': {}", name, result);
                return false;
            }
            
            // Mark as installed
            hook_info.installed = true;
            
            // Store hook info
            m_hooks[name] = hook_info;
            
            // Update statistics
            m_stats.total_hooks++;
            m_stats.installed_hooks++;
            
            spdlog::info("[HookManager] Hook '{}' installed successfully", name);
            return true;
            
        } catch (const std::exception& e) {
            spdlog::error("[HookManager] Exception while installing hook '{}': {}", name, e.what());
            m_stats.failed_hooks++;
            return false;
        }
    }
    
    bool removeHook(const std::string& name) {
        auto it = m_hooks.find(name);
        if (it == m_hooks.end()) {
            spdlog::warn("[HookManager] Hook '{}' not found", name);
            return false;
        }
        
        HookInfo& hook_info = it->second;
        if (!hook_info.installed) {
            spdlog::warn("[HookManager] Hook '{}' not installed", name);
            return false;
        }
        
        try {
            spdlog::info("[HookManager] Removing hook: {}", name);
            
            // Remove hook using Detours
            LONG result = DetourDetach(&hook_info.original_function, hook_info.hook_function);
            if (result != NO_ERROR) {
                spdlog::error("[HookManager] Failed to remove hook '{}': {}", name, result);
                return false;
            }
            
            // Mark as not installed
            hook_info.installed = false;
            
            // Update statistics
            m_stats.installed_hooks--;
            
            spdlog::info("[HookManager] Hook '{}' removed successfully", name);
            return true;
            
        } catch (const std::exception& e) {
            spdlog::error("[HookManager] Exception while removing hook '{}': {}", name, e.what());
            return false;
        }
    }
    
    void removeAllHooks() {
        spdlog::info("[HookManager] Removing all hooks...");
        
        std::vector<std::string> hook_names;
        for (const auto& pair : m_hooks) {
            hook_names.push_back(pair.first);
        }
        
        for (const auto& name : hook_names) {
            removeHook(name);
        }
        
        spdlog::info("[HookManager] All hooks removed");
    }
    
    bool isHookInstalled(const std::string& name) const {
        auto it = m_hooks.find(name);
        if (it != m_hooks.end()) {
            return it->second.installed;
        }
        return false;
    }
    
    void* getOriginalFunction(const std::string& name) const {
        auto it = m_hooks.find(name);
        if (it != m_hooks.end()) {
            return it->second.original_function;
        }
        return nullptr;
    }
    
    void* getHookFunction(const std::string& name) const {
        auto it = m_hooks.find(name);
        if (it != m_hooks.end()) {
            return it->second.hook_function;
        }
        return nullptr;
    }
    
    void setPreCallback(const std::string& name, std::function<void()> callback) {
        auto it = m_hooks.find(name);
        if (it != m_hooks.end()) {
            it->second.pre_callback = callback;
        }
    }
    
    void setPostCallback(const std::string& name, std::function<void()> callback) {
        auto it = m_hooks.find(name);
        if (it != m_hooks.end()) {
            it->second.post_callback = callback;
        }
    }
    
    void incrementCallCount(const std::string& name) {
        auto it = m_hooks.find(name);
        if (it != m_hooks.end()) {
            it->second.call_count++;
            m_stats.total_calls++;
        }
    }
    
    HookStats getStats() const {
        return m_stats;
    }
    
    std::vector<std::string> getHookNames() const {
        std::vector<std::string> names;
        for (const auto& pair : m_hooks) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    bool commitTransaction() {
        try {
            spdlog::info("[HookManager] Committing hook transaction...");
            
            LONG result = DetourTransactionCommit();
            if (result != NO_ERROR) {
                spdlog::error("[HookManager] Failed to commit transaction: {}", result);
                return false;
            }
            
            spdlog::info("[HookManager] Hook transaction committed successfully");
            return true;
            
        } catch (const std::exception& e) {
            spdlog::error("[HookManager] Exception while committing transaction: {}", e.what());
            return false;
        }
    }
    
    bool beginTransaction() {
        try {
            spdlog::info("[HookManager] Beginning hook transaction...");
            
            LONG result = DetourTransactionBegin();
            if (result != NO_ERROR) {
                spdlog::error("[HookManager] Failed to begin transaction: {}", result);
                return false;
            }
            
            result = DetourUpdateThread(GetCurrentThread());
            if (result != NO_ERROR) {
                spdlog::error("[HookManager] Failed to update thread: {}", result);
                return false;
            }
            
            spdlog::info("[HookManager] Hook transaction begun successfully");
            return true;
            
        } catch (const std::exception& e) {
            spdlog::error("[HookManager] Exception while beginning transaction: {}", e.what());
            return false;
        }
    }

private:
    bool m_initialized;
    std::unordered_map<std::string, HookInfo> m_hooks;
    HookStats m_stats;
    mutable std::mutex m_mutex;
};

// HookManager implementation
HookManager::HookManager() : m_impl(std::make_unique<HookManagerImpl>()) {
    spdlog::info("[HookManager] Hook manager created");
}

HookManager::~HookManager() {
    spdlog::info("[HookManager] Hook manager destroyed");
}

bool HookManager::initialize() {
    return m_impl->initialize();
}

void HookManager::cleanup() {
    m_impl->cleanup();
}

bool HookManager::installHook(const std::string& name, void* original_function, void* hook_function, 
                            const std::string& description) {
    return m_impl->installHook(name, original_function, hook_function, description);
}

bool HookManager::removeHook(const std::string& name) {
    return m_impl->removeHook(name);
}

void HookManager::removeAllHooks() {
    m_impl->removeAllHooks();
}

bool HookManager::isHookInstalled(const std::string& name) const {
    return m_impl->isHookInstalled(name);
}

void* HookManager::getOriginalFunction(const std::string& name) const {
    return m_impl->getOriginalFunction(name);
}

void* HookManager::getHookFunction(const std::string& name) const {
    return m_impl->getHookFunction(name);
}

void HookManager::setPreCallback(const std::string& name, std::function<void()> callback) {
    m_impl->setPreCallback(name, callback);
}

void HookManager::setPostCallback(const std::string& name, std::function<void()> callback) {
    m_impl->setPostCallback(name, callback);
}

void HookManager::incrementCallCount(const std::string& name) {
    m_impl->incrementCallCount(name);
}

HookManager::HookStats HookManager::getStats() const {
    return m_impl->getStats();
}

std::vector<std::string> HookManager::getHookNames() const {
    return m_impl->getHookNames();
}

bool HookManager::commitTransaction() {
    return m_impl->commitTransaction();
}

bool HookManager::beginTransaction() {
    return m_impl->beginTransaction();
}

// Global hook manager instance
std::unique_ptr<HookManager> g_hook_manager;

} // namespace Hooks
} // namespace uevr
