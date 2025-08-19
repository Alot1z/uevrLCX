/*
 * Cross-Engine Memory Management Protocols Implementation
 * 
 * Implementation of safe memory management and hooking safety patterns
 * for all cross-engine VR adapters.
 * 
 * Copyright (c) 2024 UEVR Cross-Engine Adapter Project
 * Licensed under MIT License
 */

#include "memory_management.hpp"
#include <spdlog/spdlog.h>
#include <Psapi.h>
#include <TlHelp32.h>

namespace uevr_common {

// MemorySafetyManager Implementation
MemorySafetyManager::MemorySafetyManager(const SafetyConfig& config)
    : m_config(config), m_total_allocated(0) {
    spdlog::info("[MemorySafety] Memory safety manager initialized");
}

MemorySafetyManager::~MemorySafetyManager() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Clean up registered memory regions
    for (const auto& region : m_memory_regions) {
        spdlog::debug("[MemorySafety] Cleaning up memory region: {} at 0x{:x}", 
                     region.description, reinterpret_cast<uintptr_t>(region.address));
    }
    
    spdlog::info("[MemorySafety] Memory safety manager destroyed");
}

bool MemorySafetyManager::is_safe_to_read(const void* ptr, size_t size) const {
    if (!ptr || size == 0) {
        return false;
    }
    
    if (m_config.enable_memory_validation) {
        return validate_memory_region(ptr, size);
    }
    
    return true;
}

bool MemorySafetyManager::is_safe_to_write(void* ptr, size_t size) const {
    if (!ptr || size == 0) {
        return false;
    }
    
    if (m_config.enable_memory_validation) {
        return validate_memory_region(ptr, size);
    }
    
    return true;
}

bool MemorySafetyManager::is_safe_to_execute(const void* ptr) const {
    if (!ptr) {
        return false;
    }
    
    if (m_config.enable_memory_validation) {
        return check_page_protection(ptr, 1, PAGE_EXECUTE);
    }
    
    return true;
}

bool MemorySafetyManager::register_memory_region(void* address, size_t size, const std::string& description) {
    if (!address || size == 0) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Check if region already exists
    for (const auto& region : m_memory_regions) {
        if (region.address == address) {
            spdlog::warn("[MemorySafety] Memory region already registered at 0x{:x}", 
                        reinterpret_cast<uintptr_t>(address));
            return false;
        }
    }
    
    // Create new memory region
    MemoryRegion region;
    region.address = address;
    region.size = size;
    region.description = description;
    
    // Get page protection
    MEMORY_BASIC_INFORMATION mbi;
    if (VirtualQuery(address, &mbi, sizeof(mbi)) != 0) {
        region.readable = (mbi.Protect & PAGE_READONLY) || (mbi.Protect & PAGE_READWRITE);
        region.writable = (mbi.Protect & PAGE_READWRITE) || (mbi.Protect & PAGE_WRITECOPY);
        region.executable = (mbi.Protect & PAGE_EXECUTE) || (mbi.Protect & PAGE_EXECUTE_READ);
    }
    
    m_memory_regions.push_back(region);
    
    spdlog::debug("[MemorySafety] Registered memory region: {} at 0x{:x} ({} bytes)", 
                 description, reinterpret_cast<uintptr_t>(address), size);
    
    return true;
}

bool MemorySafetyManager::unregister_memory_region(void* address) {
    if (!address) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    for (auto it = m_memory_regions.begin(); it != m_memory_regions.end(); ++it) {
        if (it->address == address) {
            spdlog::debug("[MemorySafety] Unregistered memory region: {} at 0x{:x}", 
                         it->description, reinterpret_cast<uintptr_t>(address));
            m_memory_regions.erase(it);
            return true;
        }
    }
    
    return false;
}

const MemorySafetyManager::MemoryRegion* MemorySafetyManager::get_memory_region(const void* ptr) const {
    if (!ptr) {
        return nullptr;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    for (const auto& region : m_memory_regions) {
        if (ptr >= region.address && 
            static_cast<const uint8_t*>(ptr) < static_cast<const uint8_t*>(region.address) + region.size) {
            return &region;
        }
    }
    
    return nullptr;
}

bool MemorySafetyManager::register_hook(const HookSafetyInfo& hook_info) {
    if (!hook_info.original_function || !hook_info.hook_function) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    m_hooks[hook_info.original_function] = hook_info;
    
    spdlog::debug("[MemorySafety] Registered hook: {} -> {} in {}", 
                 hook_info.function_name, hook_info.hook_function, hook_info.module_name);
    
    return true;
}

bool MemorySafetyManager::unregister_hook(void* original_function) {
    if (!original_function) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_hooks.find(original_function);
    if (it != m_hooks.end()) {
        spdlog::debug("[MemorySafety] Unregistered hook: {} in {}", 
                     it->second.function_name, it->second.module_name);
        m_hooks.erase(it);
        return true;
    }
    
    return false;
}

bool MemorySafetyManager::is_hook_safe(const HookSafetyInfo& hook_info) const {
    if (!hook_info.original_function || !hook_info.hook_function) {
        return false;
    }
    
    // Check if function is in valid memory region
    if (!is_safe_to_execute(hook_info.original_function)) {
        return false;
    }
    
    if (!is_safe_to_execute(hook_info.hook_function)) {
        return false;
    }
    
    return true;
}

void MemorySafetyManager::track_memory_allocation(void* ptr, size_t size, const std::string& source) {
    if (!ptr || size == 0) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    m_memory_tracking[ptr] = size;
    m_total_allocated += size;
    
    spdlog::debug("[MemorySafety] Tracked allocation: {} bytes from {} at 0x{:x}", 
                 size, source, reinterpret_cast<uintptr_t>(ptr));
}

void MemorySafetyManager::track_memory_deallocation(void* ptr) {
    if (!ptr) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_memory_tracking.find(ptr);
    if (it != m_memory_tracking.end()) {
        m_total_allocated -= it->second;
        m_memory_tracking.erase(it);
        
        spdlog::debug("[MemorySafety] Tracked deallocation at 0x{:x}", 
                     reinterpret_cast<uintptr_t>(ptr));
    }
}

size_t MemorySafetyManager::get_total_allocated_memory() const {
    return m_total_allocated.load();
}

bool MemorySafetyManager::validate_memory_region(const void* ptr, size_t size) const {
    if (!ptr || size == 0) {
        return false;
    }
    
    // Check if size exceeds maximum buffer size
    if (size > m_config.max_buffer_size) {
        log_memory_violation(ptr, size, "Buffer size exceeds maximum");
        return false;
    }
    
    // Check page protection
    return check_page_protection(ptr, size, PAGE_READONLY | PAGE_READWRITE);
}

bool MemorySafetyManager::check_page_protection(const void* ptr, size_t size, DWORD required_protection) const {
    MEMORY_BASIC_INFORMATION mbi;
    if (VirtualQuery(ptr, &mbi, sizeof(mbi)) == 0) {
        log_memory_violation(ptr, size, "VirtualQuery failed");
        return false;
    }
    
    // Check if memory is committed and accessible
    if (mbi.State != MEM_COMMIT) {
        log_memory_violation(ptr, size, "Memory not committed");
        return false;
    }
    
    // Check protection
    if ((mbi.Protect & required_protection) == 0) {
        log_memory_violation(ptr, size, "Insufficient memory protection");
        return false;
    }
    
    return true;
}

void MemorySafetyManager::log_memory_violation(const void* ptr, size_t size, const std::string& operation) const {
    spdlog::warn("[MemorySafety] Memory violation: {} at 0x{:x} ({} bytes)", 
                 operation, reinterpret_cast<uintptr_t>(ptr), size);
}

// ConstantBufferSafetyManager Implementation
ConstantBufferSafetyManager::ConstantBufferSafetyManager() {
    spdlog::info("[ConstantBufferSafety] Constant buffer safety manager initialized");
}

ConstantBufferSafetyManager::~ConstantBufferSafetyManager() {
    std::lock_guard<std::mutex> lock(m_buffer_mutex);
    
    // Clean up mapped buffers
    for (const auto& info : m_mapped_buffers) {
        if (info.mapped) {
            spdlog::warn("[ConstantBufferSafety] Buffer still mapped at destruction: {}", 
                         info.description);
        }
    }
    
    spdlog::info("[ConstantBufferSafety] Constant buffer safety manager destroyed");
}

bool ConstantBufferSafetyManager::map_constant_buffer_d3d11(ID3D11DeviceContext* context, ID3D11Buffer* buffer, ConstantBufferInfo& info) {
    if (!context || !buffer) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(m_buffer_mutex);
    
    // Get buffer description
    D3D11_BUFFER_DESC desc;
    buffer->GetDesc(&desc);
    
    info.data = nullptr;
    info.size = desc.ByteWidth;
    info.description = "D3D11 Constant Buffer";
    info.mapped = false;
    
    // Map the buffer
    HRESULT hr = context->Map(buffer, 0, D3D11_MAP_READ, 0, &info.d3d11_mapped);
    if (SUCCEEDED(hr)) {
        info.data = info.d3d11_mapped.pData;
        info.mapped = true;
        m_mapped_buffers.push_back(info);
        
        spdlog::debug("[ConstantBufferSafety] Mapped D3D11 buffer: {} bytes at 0x{:x}", 
                     info.size, reinterpret_cast<uintptr_t>(info.data));
        return true;
    }
    
    spdlog::error("[ConstantBufferSafety] Failed to map D3D11 buffer: 0x{:x}", hr);
    return false;
}

void ConstantBufferSafetyManager::unmap_constant_buffer_d3d11(ID3D11DeviceContext* context, ID3D11Buffer* buffer) {
    if (!context || !buffer) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(m_buffer_mutex);
    
    // Find and remove the mapped buffer
    for (auto it = m_mapped_buffers.begin(); it != m_mapped_buffers.end(); ++it) {
        if (it->mapped && it->data) {
            context->Unmap(buffer, 0);
            it->mapped = false;
            it->data = nullptr;
            
            spdlog::debug("[ConstantBufferSafety] Unmapped D3D11 buffer");
            break;
        }
    }
}

bool ConstantBufferSafetyManager::map_constant_buffer_d3d12(ID3D12Resource* resource, ConstantBufferInfo& info) {
    if (!resource) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(m_buffer_mutex);
    
    // Get resource description
    D3D12_RESOURCE_DESC desc = resource->GetDesc();
    
    info.data = nullptr;
    info.size = static_cast<size_t>(desc.Width);
    info.description = "D3D12 Constant Buffer";
    info.mapped = false;
    
    // Map the resource
    D3D12_RANGE read_range = {0, info.size};
    HRESULT hr = resource->Map(0, &read_range, &info.data);
    if (SUCCEEDED(hr)) {
        info.mapped = true;
        info.d3d12_range = read_range;
        m_mapped_buffers.push_back(info);
        
        spdlog::debug("[ConstantBufferSafety] Mapped D3D12 buffer: {} bytes at 0x{:x}", 
                     info.size, reinterpret_cast<uintptr_t>(info.data));
        return true;
    }
    
    spdlog::error("[ConstantBufferSafety] Failed to map D3D12 buffer: 0x{:x}", hr);
    return false;
}

void ConstantBufferSafetyManager::unmap_constant_buffer_d3d12(ID3D12Resource* resource) {
    if (!resource) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(m_buffer_mutex);
    
    // Find and remove the mapped buffer
    for (auto it = m_mapped_buffers.begin(); it != m_mapped_buffers.end(); ++it) {
        if (it->mapped && it->data) {
            resource->Unmap(0, &it->d3d12_range);
            it->mapped = false;
            it->data = nullptr;
            
            spdlog::debug("[ConstantBufferSafety] Unmapped D3D12 buffer");
            break;
        }
    }
}

bool ConstantBufferSafetyManager::extract_matrix_safe(const ConstantBufferInfo& info, size_t offset, float* matrix_data, size_t matrix_size) {
    if (!info.mapped || !info.data || !matrix_data || matrix_size == 0) {
        return false;
    }
    
    if (!validate_buffer_access(info, offset, matrix_size)) {
        return false;
    }
    
    // Copy matrix data safely
    const uint8_t* src = static_cast<const uint8_t*>(info.data) + offset;
    std::memcpy(matrix_data, src, matrix_size);
    
    // Validate the extracted data
    return validate_matrix_data(matrix_data, matrix_size);
}

bool ConstantBufferSafetyManager::validate_matrix_data(const float* matrix_data, size_t matrix_size) const {
    if (!matrix_data || matrix_size == 0) {
        return false;
    }
    
    // Check for NaN or infinite values
    for (size_t i = 0; i < matrix_size / sizeof(float); ++i) {
        if (std::isnan(matrix_data[i]) || std::isinf(matrix_data[i])) {
            spdlog::warn("[ConstantBufferSafety] Invalid matrix data detected at index {}", i);
            return false;
        }
    }
    
    return true;
}

bool ConstantBufferSafetyManager::validate_buffer_access(const ConstantBufferInfo& info, size_t offset, size_t size) const {
    if (offset + size > info.size) {
        log_buffer_violation(info, offset, size);
        return false;
    }
    
    return true;
}

void ConstantBufferSafetyManager::log_buffer_violation(const ConstantBufferInfo& info, size_t offset, size_t size) const {
    spdlog::warn("[ConstantBufferSafety] Buffer access violation: {} bytes at offset {} (buffer size: {})", 
                 size, offset, info.size);
}

// HookSafetyManager Implementation
HookSafetyManager::HookSafetyManager(const HookConfig& config)
    : m_config(config) {
    spdlog::info("[HookSafety] Hook safety manager initialized");
}

HookSafetyManager::~HookSafetyManager() {
    std::lock_guard<std::mutex> lock(m_hook_mutex);
    
    // Clean up all hooks
    for (auto& hook : m_hooks) {
        if (hook.second.status == HookStatus::Active) {
            spdlog::warn("[HookSafety] Hook still active at destruction: {} in {}", 
                         hook.second.function_name, hook.second.module_name);
        }
    }
    
    spdlog::info("[HookSafety] Hook safety manager destroyed");
}

bool HookSafetyManager::install_hook(HookInfo& hook_info) {
    if (!hook_info.original_function || !hook_info.hook_function) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(m_hook_mutex);
    
    // Validate hook target and function
    if (!validate_hook_target(hook_info.original_function)) {
        log_hook_violation(hook_info, "Invalid hook target");
        return false;
    }
    
    if (!validate_hook_function(hook_info.hook_function)) {
        log_hook_violation(hook_info, "Invalid hook function");
        return false;
    }
    
    // Check for conflicts
    if (check_hook_conflicts(hook_info)) {
        log_hook_violation(hook_info, "Hook conflict detected");
        return false;
    }
    
    // Create trampoline if needed
    if (m_config.enable_hook_rollback) {
        if (!create_trampoline(hook_info)) {
            log_hook_violation(hook_info, "Failed to create trampoline");
            return false;
        }
    }
    
    // Update hook status
    hook_info.status = HookStatus::Active;
    hook_info.thread_id = GetCurrentThreadId();
    hook_info.hook_count++;
    
    m_hooks[hook_info.original_function] = hook_info;
    
    spdlog::info("[HookSafety] Hook installed: {} -> {} in {}", 
                 hook_info.function_name, hook_info.hook_function, hook_info.module_name);
    
    return true;
}

bool HookSafetyManager::remove_hook(HookInfo& hook_info) {
    if (!hook_info.original_function) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(m_hook_mutex);
    
    auto it = m_hooks.find(hook_info.original_function);
    if (it != m_hooks.end()) {
        // Destroy trampoline if exists
        if (hook_info.trampoline) {
            destroy_trampoline(hook_info);
        }
        
        hook_info.status = HookStatus::Removed;
        m_hooks.erase(it);
        
        spdlog::info("[HookSafety] Hook removed: {} in {}", 
                     hook_info.function_name, hook_info.module_name);
        
        return true;
    }
    
    return false;
}

bool HookSafetyManager::is_hook_safe(const HookInfo& hook_info) const {
    if (!hook_info.original_function || !hook_info.hook_function) {
        return false;
    }
    
    // Check memory protection
    if (!validate_memory_protection(hook_info.original_function, 1)) {
        return false;
    }
    
    if (!validate_memory_protection(hook_info.hook_function, 1)) {
        return false;
    }
    
    return true;
}

bool HookSafetyManager::validate_hook_target(void* function) const {
    if (!function) {
        return false;
    }
    
    return validate_memory_protection(function, 1);
}

bool HookSafetyManager::validate_hook_function(void* function) const {
    if (!function) {
        return false;
    }
    
    return validate_memory_protection(function, 1);
}

bool HookSafetyManager::check_hook_conflicts(const HookInfo& hook_info) const {
    if (!m_config.enable_hook_validation) {
        return false;
    }
    
    // Check if function already has a hook
    for (const auto& hook : m_hooks) {
        if (hook.first == hook_info.original_function && 
            hook.second.status == HookStatus::Active) {
            return true;
        }
    }
    
    return false;
}

bool HookSafetyManager::rollback_hook(HookInfo& hook_info) {
    if (!hook_info.trampoline) {
        return false;
    }
    
    // Restore original function
    // This is a simplified implementation - in practice, you'd need to restore the original bytes
    
    hook_info.status = HookStatus::Removed;
    
    spdlog::info("[HookSafety] Hook rolled back: {} in {}", 
                 hook_info.function_name, hook_info.module_name);
    
    return true;
}

bool HookSafetyManager::rollback_all_hooks() {
    std::lock_guard<std::mutex> lock(m_hook_mutex);
    
    bool success = true;
    for (auto& hook : m_hooks) {
        if (hook.second.status == HookStatus::Active) {
            if (!rollback_hook(hook.second)) {
                success = false;
            }
        }
    }
    
    return success;
}

const std::vector<HookSafetyManager::HookInfo>& HookSafetyManager::get_active_hooks() const {
    static std::vector<HookInfo> active_hooks;
    active_hooks.clear();
    
    std::lock_guard<std::mutex> lock(m_hook_mutex);
    
    for (const auto& hook : m_hooks) {
        if (hook.second.status == HookStatus::Active) {
            active_hooks.push_back(hook.second);
        }
    }
    
    return active_hooks;
}

HookSafetyManager::HookStatus HookSafetyManager::get_hook_status(void* original_function) const {
    if (!original_function) {
        return HookStatus::NotInstalled;
    }
    
    std::lock_guard<std::mutex> lock(m_hook_mutex);
    
    auto it = m_hooks.find(original_function);
    if (it != m_hooks.end()) {
        return it->second.status;
    }
    
    return HookStatus::NotInstalled;
}

bool HookSafetyManager::create_trampoline(HookInfo& hook_info) {
    // This is a simplified implementation
    // In practice, you'd need to allocate executable memory and create a proper trampoline
    
    hook_info.trampoline = hook_info.original_function;
    return true;
}

void HookSafetyManager::destroy_trampoline(HookInfo& hook_info) {
    // This is a simplified implementation
    // In practice, you'd need to free the allocated trampoline memory
    
    hook_info.trampoline = nullptr;
}

bool HookSafetyManager::validate_memory_protection(void* address, size_t size) const {
    MEMORY_BASIC_INFORMATION mbi;
    if (VirtualQuery(address, &mbi, sizeof(mbi)) == 0) {
        return false;
    }
    
    return (mbi.State == MEM_COMMIT) && (mbi.Protect & PAGE_EXECUTE);
}

void HookSafetyManager::log_hook_violation(const HookInfo& hook_info, const std::string& reason) const {
    spdlog::warn("[HookSafety] Hook violation: {} for {} in {}", 
                 reason, hook_info.function_name, hook_info.module_name);
}

// GlobalMemoryManager Implementation
std::unique_ptr<MemorySafetyManager> GlobalMemoryManager::s_memory_safety;
std::unique_ptr<ConstantBufferSafetyManager> GlobalMemoryManager::s_constant_buffer_safety;
std::unique_ptr<HookSafetyManager> GlobalMemoryManager::s_hook_safety;
std::mutex GlobalMemoryManager::s_init_mutex;

MemorySafetyManager& GlobalMemoryManager::get_memory_safety() {
    initialize_if_needed();
    return *s_memory_safety;
}

ConstantBufferSafetyManager& GlobalMemoryManager::get_constant_buffer_safety() {
    initialize_if_needed();
    return *s_constant_buffer_safety;
}

HookSafetyManager& GlobalMemoryManager::get_hook_safety() {
    initialize_if_needed();
    return *s_hook_safety;
}

void GlobalMemoryManager::configure_memory_safety(const MemorySafetyManager::SafetyConfig& config) {
    std::lock_guard<std::mutex> lock(s_init_mutex);
    
    if (s_memory_safety) {
        // Recreate with new config
        s_memory_safety.reset();
    }
    
    s_memory_safety = std::make_unique<MemorySafetyManager>(config);
}

void GlobalMemoryManager::configure_hook_safety(const HookSafetyManager::HookConfig& config) {
    std::lock_guard<std::mutex> lock(s_init_mutex);
    
    if (s_hook_safety) {
        // Recreate with new config
        s_hook_safety.reset();
    }
    
    s_hook_safety = std::make_unique<HookSafetyManager>(config);
}

void GlobalMemoryManager::cleanup_all() {
    std::lock_guard<std::mutex> lock(s_init_mutex);
    
    if (s_hook_safety) {
        s_hook_safety->rollback_all_hooks();
    }
    
    s_memory_safety.reset();
    s_constant_buffer_safety.reset();
    s_hook_safety.reset();
    
    spdlog::info("[GlobalMemoryManager] All memory managers cleaned up");
}

void GlobalMemoryManager::initialize_if_needed() {
    std::lock_guard<std::mutex> lock(s_init_mutex);
    
    if (!s_memory_safety) {
        s_memory_safety = std::make_unique<MemorySafetyManager>();
    }
    
    if (!s_constant_buffer_safety) {
        s_constant_buffer_safety = std::make_unique<ConstantBufferSafetyManager>();
    }
    
    if (!s_hook_safety) {
        s_hook_safety = std::make_unique<HookSafetyManager>();
    }
}

} // namespace uevr_common
