/*
 * Cross-Engine Memory Management Protocols for uevr Adapters
 * 
 * This header provides safe memory management and hooking safety patterns
 * for all cross-engine VR adapters, ensuring stability and preventing crashes.
 * 
 * Copyright (c) 2024 uevr Cross-Engine Adapter Project
 * Licensed under MIT License
 */

#pragma once

#include <windows.h>
#include <d3d11.h>
#include <d3d12.h>
#include <memory>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <atomic>

namespace uevr_common {

/**
 * Memory safety manager for cross-engine adapters
 * 
 * Provides safe memory access patterns and prevents crashes
 * from invalid memory operations during hooking.
 */
class MemorySafetyManager {
public:
    // Memory safety configuration
    struct SafetyConfig {
        bool enable_memory_validation = true;
        bool enable_bounds_checking = true;
        bool enable_null_pointer_protection = true;
        size_t max_buffer_size = 1024 * 1024; // 1MB default
        size_t max_string_length = 1024;
        bool enable_memory_tracking = true;
    };

    // Memory region information
    struct MemoryRegion {
        void* address = nullptr;
        size_t size = 0;
        bool readable = false;
        bool writable = false;
        bool executable = false;
        std::string description;
    };

    // Hook safety information
    struct HookSafetyInfo {
        void* original_function = nullptr;
        void* hook_function = nullptr;
        bool hook_active = false;
        std::string function_name;
        std::string module_name;
        DWORD thread_id = 0;
    };

public:
    MemorySafetyManager(const SafetyConfig& config = SafetyConfig{});
    ~MemorySafetyManager();

    // Memory validation
    bool is_safe_to_read(const void* ptr, size_t size) const;
    bool is_safe_to_write(void* ptr, size_t size) const;
    bool is_safe_to_execute(const void* ptr) const;
    
    // Memory region management
    bool register_memory_region(void* address, size_t size, const std::string& description = "");
    bool unregister_memory_region(void* address);
    const MemoryRegion* get_memory_region(const void* ptr) const;
    
    // Hook safety management
    bool register_hook(const HookSafetyInfo& hook_info);
    bool unregister_hook(void* original_function);
    bool is_hook_safe(const HookSafetyInfo& hook_info) const;
    
    // Memory tracking
    void track_memory_allocation(void* ptr, size_t size, const std::string& source = "");
    void track_memory_deallocation(void* ptr);
    size_t get_total_allocated_memory() const;
    
    // Safety checks
    template<typename T>
    T safe_read(const void* ptr, size_t offset = 0) const {
        if (!is_safe_to_read(ptr, offset + sizeof(T))) {
            return T{};
        }
        return *reinterpret_cast<const T*>(static_cast<const uint8_t*>(ptr) + offset);
    }
    
    template<typename T>
    bool safe_write(void* ptr, const T& value, size_t offset = 0) const {
        if (!is_safe_to_write(ptr, offset + sizeof(T))) {
            return false;
        }
        *reinterpret_cast<T*>(static_cast<uint8_t*>(ptr) + offset) = value;
        return true;
    }

private:
    SafetyConfig m_config;
    std::vector<MemoryRegion> m_memory_regions;
    std::unordered_map<void*, HookSafetyInfo> m_hooks;
    std::unordered_map<void*, size_t> m_memory_tracking;
    mutable std::mutex m_mutex;
    std::atomic<size_t> m_total_allocated;
    
    // Internal validation helpers
    bool validate_memory_region(const void* ptr, size_t size) const;
    bool check_page_protection(const void* ptr, size_t size, DWORD required_protection) const;
    void log_memory_violation(const void* ptr, size_t size, const std::string& operation) const;
};

/**
 * Constant buffer safety manager for D3D11/D3D12
 * 
 * Provides safe access to constant buffers during matrix extraction.
 */
class ConstantBufferSafetyManager {
public:
    // Constant buffer information
    struct ConstantBufferInfo {
        void* data = nullptr;
        size_t size = 0;
        bool mapped = false;
        std::string description;
        D3D11_MAPPED_SUBRESOURCE d3d11_mapped;
        D3D12_RANGE d3d12_range;
    };

public:
    ConstantBufferSafetyManager();
    ~ConstantBufferSafetyManager();

    // D3D11 constant buffer safety
    bool map_constant_buffer_d3d11(ID3D11DeviceContext* context, ID3D11Buffer* buffer, ConstantBufferInfo& info);
    void unmap_constant_buffer_d3d11(ID3D11DeviceContext* context, ID3D11Buffer* buffer);
    
    // D3D12 constant buffer safety
    bool map_constant_buffer_d3d12(ID3D12Resource* resource, ConstantBufferInfo& info);
    void unmap_constant_buffer_d3d12(ID3D12Resource* resource);
    
    // Safe matrix extraction
    bool extract_matrix_safe(const ConstantBufferInfo& info, size_t offset, float* matrix_data, size_t matrix_size);
    bool validate_matrix_data(const float* matrix_data, size_t matrix_size) const;

private:
    std::vector<ConstantBufferInfo> m_mapped_buffers;
    std::mutex m_buffer_mutex;
    
    bool validate_buffer_access(const ConstantBufferInfo& info, size_t offset, size_t size) const;
    void log_buffer_violation(const ConstantBufferInfo& info, size_t offset, size_t size) const;
};

/**
 * Hook safety manager for cross-engine adapters
 * 
 * Ensures safe hook installation and removal across different engines.
 */
class HookSafetyManager {
public:
    // Hook configuration
    struct HookConfig {
        bool enable_hook_validation = true;
        bool enable_thread_safety = true;
        bool enable_hook_rollback = true;
        size_t max_hooks_per_function = 1;
        bool enable_hook_chaining = false;
    };

    // Hook status
    enum class HookStatus {
        NotInstalled,
        Installing,
        Active,
        Failed,
        Removed
    };

    // Hook information
    struct HookInfo {
        void* original_function = nullptr;
        void* hook_function = nullptr;
        void* trampoline = nullptr;
        HookStatus status = HookStatus::NotInstalled;
        std::string function_name;
        std::string module_name;
        DWORD thread_id = 0;
        size_t hook_count = 0;
    };

public:
    HookSafetyManager(const HookConfig& config = HookConfig{});
    ~HookSafetyManager();

    // Hook management
    bool install_hook(HookInfo& hook_info);
    bool remove_hook(HookInfo& hook_info);
    bool is_hook_safe(const HookInfo& hook_info) const;
    
    // Hook validation
    bool validate_hook_target(void* function) const;
    bool validate_hook_function(void* function) const;
    bool check_hook_conflicts(const HookInfo& hook_info) const;
    
    // Hook rollback
    bool rollback_hook(HookInfo& hook_info);
    bool rollback_all_hooks();
    
    // Hook monitoring
    const std::vector<HookInfo>& get_active_hooks() const;
    HookStatus get_hook_status(void* original_function) const;

private:
    HookConfig m_config;
    std::vector<HookInfo> m_hooks;
    std::mutex m_hook_mutex;
    
    bool create_trampoline(HookInfo& hook_info);
    void destroy_trampoline(HookInfo& hook_info);
    bool validate_memory_protection(void* address, size_t size) const;
    void log_hook_violation(const HookInfo& hook_info, const std::string& reason) const;
};

/**
 * Global memory management instance
 * 
 * Provides centralized access to memory safety features.
 */
class GlobalMemoryManager {
public:
    static MemorySafetyManager& get_memory_safety();
    static ConstantBufferSafetyManager& get_constant_buffer_safety();
    static HookSafetyManager& get_hook_safety();
    
    // Global configuration
    static void configure_memory_safety(const MemorySafetyManager::SafetyConfig& config);
    static void configure_hook_safety(const HookSafetyManager::HookConfig& config);
    
    // Global cleanup
    static void cleanup_all();

private:
    static std::unique_ptr<MemorySafetyManager> s_memory_safety;
    static std::unique_ptr<ConstantBufferSafetyManager> s_constant_buffer_safety;
    static std::unique_ptr<HookSafetyManager> s_hook_safety;
    static std::mutex s_init_mutex;
    
    static void initialize_if_needed();
};

} // namespace uevr_common

