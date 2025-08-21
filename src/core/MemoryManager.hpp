#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <atomic>
#include <chrono>
#include <functional>

namespace UEVR {

// Forward declarations
class Logger;
class ConfigurationManager;

/**
 * @brief Memory allocation types
 */
enum class AllocationType {
    STANDARD,       // Standard heap allocation
    EXECUTABLE,     // Executable memory allocation
    SHARED,         // Shared memory allocation
    VIRTUAL,        // Virtual memory allocation
    POOL,           // Memory pool allocation
    STACK,          // Stack allocation
    ALIGNED         // Aligned allocation
};

/**
 * @brief Memory allocation information
 */
struct AllocationInfo {
    void* address;
    size_t size;
    size_t alignment;
    AllocationType type;
    std::string tag;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    std::thread::id threadId;
    size_t callStackHash;
    
    AllocationInfo() : 
        address(nullptr), 
        size(0), 
        alignment(0), 
        type(AllocationType::STANDARD),
        line(0),
        callStackHash(0) {}
};

/**
 * @brief Memory statistics structure
 */
struct MemoryStatistics {
    size_t totalAllocated;
    size_t totalFreed;
    size_t currentAllocated;
    size_t peakAllocated;
    size_t allocationCount;
    size_t freeCount;
    size_t leakCount;
    size_t fragmentationBytes;
    double fragmentationRatio;
    
    MemoryStatistics() : 
        totalAllocated(0), 
        totalFreed(0), 
        currentAllocated(0), 
        peakAllocated(0),
        allocationCount(0), 
        freeCount(0), 
        leakCount(0), 
        fragmentationBytes(0),
        fragmentationRatio(0.0) {}
};

/**
 * @brief Memory pool structure
 */
struct MemoryPool {
    std::string name;
    void* baseAddress;
    size_t totalSize;
    size_t blockSize;
    size_t blockCount;
    size_t freeBlocks;
    std::vector<bool> blockUsage;
    std::mutex poolMutex;
    
    MemoryPool() : 
        baseAddress(nullptr), 
        totalSize(0), 
        blockSize(0), 
        blockCount(0), 
        freeBlocks(0) {}
};

/**
 * @brief Custom allocator interface
 */
class IAllocator {
public:
    virtual ~IAllocator() = default;
    virtual void* Allocate(size_t size, size_t alignment = 0) = 0;
    virtual void Free(void* ptr) = 0;
    virtual size_t GetSize(void* ptr) const = 0;
    virtual bool IsValid(void* ptr) const = 0;
    virtual std::string GetName() const = 0;
};

/**
 * @brief Memory manager class
 * 
 * Manages memory allocation, deallocation, and leak detection
 */
class MemoryManager {
public:
    /**
     * @brief Constructor
     */
    MemoryManager();
    
    /**
     * @brief Destructor
     */
    ~MemoryManager();
    
    /**
     * @brief Initialize the memory manager
     * @param configManager Configuration manager instance
     * @param logger Logger instance
     * @return true if initialization successful, false otherwise
     */
    bool Initialize(std::shared_ptr<ConfigurationManager> configManager, std::shared_ptr<Logger> logger);
    
    /**
     * @brief Shutdown the memory manager
     */
    void Shutdown();
    
    /**
     * @brief Allocate memory
     * @param size Size in bytes
     * @param alignment Memory alignment (0 for default)
     * @param type Allocation type
     * @param tag Allocation tag for tracking
     * @param file Source file name
     * @param line Source line number
     * @param function Source function name
     * @return Allocated memory pointer or nullptr if failed
     */
    void* Allocate(size_t size, size_t alignment = 0, AllocationType type = AllocationType::STANDARD,
                   const std::string& tag = "", const char* file = __builtin_FILE(), 
                   int line = __builtin_LINE(), const char* function = __builtin_FUNCTION());
    
    /**
     * @brief Free memory
     * @param ptr Memory pointer to free
     * @param file Source file name
     * @param line Source line number
     * @param function Source function name
     * @return true if freed successfully, false otherwise
     */
    bool Free(void* ptr, const char* file = __builtin_FILE(), 
              int line = __builtin_LINE(), const char* function = __builtin_FUNCTION());
    
    /**
     * @brief Reallocate memory
     * @param ptr Original memory pointer
     * @param newSize New size in bytes
     * @param file Source file name
     * @param line Source line number
     * @param function Source function name
     * @return Reallocated memory pointer or nullptr if failed
     */
    void* Reallocate(void* ptr, size_t newSize, const char* file = __builtin_FILE(), 
                     int line = __builtin_LINE(), const char* function = __builtin_FUNCTION());
    
    /**
     * @brief Get allocation size
     * @param ptr Memory pointer
     * @return Size in bytes or 0 if not found
     */
    size_t GetAllocationSize(void* ptr) const;
    
    /**
     * @brief Check if pointer is valid allocation
     * @param ptr Memory pointer
     * @return true if valid allocation, false otherwise
     */
    bool IsValidAllocation(void* ptr) const;
    
    /**
     * @brief Get allocation information
     * @param ptr Memory pointer
     * @return Allocation information or nullptr if not found
     */
    std::shared_ptr<AllocationInfo> GetAllocationInfo(void* ptr) const;
    
    /**
     * @brief Get all allocations
     * @return Vector of allocation information
     */
    std::vector<std::shared_ptr<AllocationInfo>> GetAllAllocations() const;
    
    /**
     * @brief Get allocations by tag
     * @param tag Allocation tag
     * @return Vector of allocation information
     */
    std::vector<std::shared_ptr<AllocationInfo>> GetAllocationsByTag(const std::string& tag) const;
    
    /**
     * @brief Get memory statistics
     * @return Memory statistics structure
     */
    MemoryStatistics GetStatistics() const;
    
    /**
     * @brief Get memory statistics by tag
     * @param tag Allocation tag
     * @return Memory statistics for specific tag
     */
    MemoryStatistics GetStatisticsByTag(const std::string& tag) const;
    
    /**
     * @brief Detect memory leaks
     * @return Vector of leaked allocations
     */
    std::vector<std::shared_ptr<AllocationInfo>> DetectLeaks() const;
    
    /**
     * @brief Check for memory corruption
     * @return Vector of corrupted allocations
     */
    std::vector<std::shared_ptr<AllocationInfo>> CheckCorruption() const;
    
    /**
     * @brief Create memory pool
     * @param name Pool name
     * @param blockSize Size of each block
     * @param blockCount Number of blocks
     * @return true if pool created successfully, false otherwise
     */
    bool CreatePool(const std::string& name, size_t blockSize, size_t blockCount);
    
    /**
     * @brief Destroy memory pool
     * @param name Pool name
     * @return true if pool destroyed successfully, false otherwise
     */
    bool DestroyPool(const std::string& name);
    
    /**
     * @brief Allocate from memory pool
     * @param poolName Pool name
     * @param tag Allocation tag
     * @return Allocated memory pointer or nullptr if failed
     */
    void* AllocateFromPool(const std::string& poolName, const std::string& tag = "");
    
    /**
     * @brief Free to memory pool
     * @param poolName Pool name
     * @param ptr Memory pointer to free
     * @return true if freed successfully, false otherwise
     */
    bool FreeToPool(const std::string& poolName, void* ptr);
    
    /**
     * @brief Get pool statistics
     * @param poolName Pool name
     * @return Pool statistics as JSON string
     */
    std::string GetPoolStatistics(const std::string& poolName) const;
    
    /**
     * @brief Register custom allocator
     * @param name Allocator name
     * @param allocator Allocator instance
     * @return true if registered successfully, false otherwise
     */
    bool RegisterAllocator(const std::string& name, std::shared_ptr<IAllocator> allocator);
    
    /**
     * @brief Unregister custom allocator
     * @param name Allocator name
     * @return true if unregistered successfully, false otherwise
     */
    bool UnregisterAllocator(const std::string& name);
    
    /**
     * @brief Get custom allocator
     * @param name Allocator name
     * @return Allocator instance or nullptr if not found
     */
    std::shared_ptr<IAllocator> GetAllocator(const std::string& name) const;
    
    /**
     * @brief Set allocation callback
     * @param callback Callback function
     */
    void SetAllocationCallback(std::function<void(const AllocationInfo&)> callback);
    
    /**
     * @brief Set deallocation callback
     * @param callback Callback function
     */
    void SetDeallocationCallback(std::function<void(void*)> callback);
    
    /**
     * @brief Enable leak detection
     * @param enabled true to enable, false to disable
     */
    void SetLeakDetectionEnabled(bool enabled);
    
    /**
     * @brief Check if leak detection is enabled
     * @return true if enabled, false otherwise
     */
    bool IsLeakDetectionEnabled() const;
    
    /**
     * @brief Enable corruption detection
     * @param enabled true to enable, false to disable
     */
    void SetCorruptionDetectionEnabled(bool enabled);
    
    /**
     * @brief Check if corruption detection is enabled
     * @return true if enabled, false otherwise
     */
    bool IsCorruptionDetectionEnabled() const;
    
    /**
     * @brief Generate memory report
     * @return Memory report as string
     */
    std::string GenerateReport() const;
    
    /**
     * @brief Save memory report to file
     * @param filename File name
     * @return true if saved successfully, false otherwise
     */
    bool SaveReportToFile(const std::string& filename) const;
    
    /**
     * @brief Garbage collect unused memory
     * @return Number of bytes freed
     */
    size_t GarbageCollect();
    
    /**
     * @brief Defragment memory pools
     * @return Number of bytes defragmented
     */
    size_t DefragmentPools();
    
    /**
     * @brief Get heap information
     * @return Heap information as JSON string
     */
    std::string GetHeapInformation() const;

private:
    // Member variables
    mutable std::mutex m_mutex;
    std::unordered_map<void*, std::unique_ptr<AllocationInfo>> m_allocations;
    std::unordered_map<std::string, std::unique_ptr<MemoryPool>> m_pools;
    std::unordered_map<std::string, std::shared_ptr<IAllocator>> m_allocators;
    std::shared_ptr<ConfigurationManager> m_configManager;
    std::shared_ptr<Logger> m_logger;
    std::atomic<bool> m_initialized;
    
    // Settings
    std::atomic<bool> m_leakDetectionEnabled;
    std::atomic<bool> m_corruptionDetectionEnabled;
    std::atomic<bool> m_trackCallStacks;
    
    // Statistics
    mutable std::atomic<size_t> m_totalAllocated;
    mutable std::atomic<size_t> m_totalFreed;
    mutable std::atomic<size_t> m_currentAllocated;
    mutable std::atomic<size_t> m_peakAllocated;
    mutable std::atomic<size_t> m_allocationCount;
    mutable std::atomic<size_t> m_freeCount;
    
    // Callbacks
    std::function<void(const AllocationInfo&)> m_allocationCallback;
    std::function<void(void*)> m_deallocationCallback;
    
    // Internal methods
    void* AllocateInternal(size_t size, size_t alignment, AllocationType type);
    bool FreeInternal(void* ptr);
    void UpdateStatistics(size_t size, bool isAllocation);
    size_t CalculateCallStackHash() const;
    void AddGuardBytes(void* ptr, size_t size);
    bool CheckGuardBytes(void* ptr, size_t size) const;
    void LogMemoryEvent(const std::string& event, const std::string& details = "");
    
    // Platform-specific methods
#ifdef _WIN32
    void* VirtualAllocateEx(size_t size, uint32_t protection);
    bool VirtualFreeEx(void* ptr, size_t size);
    size_t GetPageSize() const;
#else
    void* VirtualAllocateEx(size_t size, int protection);
    bool VirtualFreeEx(void* ptr, size_t size);
    size_t GetPageSize() const;
#endif
    
    // Configuration keys
    static constexpr const char* CONFIG_SECTION = "MemoryManager";
    static constexpr const char* CONFIG_LEAK_DETECTION = "LeakDetection";
    static constexpr const char* CONFIG_CORRUPTION_DETECTION = "CorruptionDetection";
    static constexpr const char* CONFIG_TRACK_CALL_STACKS = "TrackCallStacks";
    static constexpr const char* CONFIG_DEFAULT_ALIGNMENT = "DefaultAlignment";
};

} // namespace UEVR

// Memory allocation macros for easy tracking
#define UEVR_MALLOC(size) UEVR::MemoryManager::GetInstance().Allocate(size, 0, UEVR::AllocationType::STANDARD, "MALLOC", __FILE__, __LINE__, __FUNCTION__)
#define UEVR_FREE(ptr) UEVR::MemoryManager::GetInstance().Free(ptr, __FILE__, __LINE__, __FUNCTION__)
#define UEVR_REALLOC(ptr, size) UEVR::MemoryManager::GetInstance().Reallocate(ptr, size, __FILE__, __LINE__, __FUNCTION__)

#define UEVR_NEW(type) new(UEVR::MemoryManager::GetInstance().Allocate(sizeof(type), alignof(type), UEVR::AllocationType::STANDARD, #type, __FILE__, __LINE__, __FUNCTION__)) type
#define UEVR_DELETE(ptr) do { if(ptr) { (ptr)->~decltype(*ptr)(); UEVR::MemoryManager::GetInstance().Free(ptr, __FILE__, __LINE__, __FUNCTION__); ptr = nullptr; } } while(0)
