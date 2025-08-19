/*
This file (API.hpp) is licensed under the MIT license and is separate from the rest of the uevr codebase.

Copyright (c) 2023 praydog

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include "API.h"
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <filesystem>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>
#include <atomic>
#include <future>
#include <queue>
#include <stack>
#include <deque>
#include <list>
#include <set>
#include <map>
#include <array>
#include <tuple>
#include <optional>
#include <variant>
#include <any>
#include <type_traits>
#include <concepts>
#include <ranges>
#include <algorithm>
#include <numeric>
#include <random>
#include <regex>
#include <sstream>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <locale>
#include <codecvt>
#include <cstring>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cfloat>
#include <climits>
#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <cstdarg>
#include <csignal>
#include <csetjmp>
#include <cctype>
#include <cwchar>
#include <cwctype>
#include <cerrno>
#include <cfenv>
#include <cinttypes>
#include <cstdbool>
#include <ctgmath>
#include <cuchar>
#include <exception>
#include <stdexcept>
#include <system_error>
#include <typeinfo>
#include <typeindex>
// #include <bad_cast>  // Removed - not available in all C++ implementations
// Removed problematic includes that may not be available in all C++ implementations
#include <ios>
#include <iosfwd>
#include <istream>
#include <ostream>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <streambuf>
#include <strstream>
#include <locale>
#include <codecvt>
#include <iterator>
#include <utility>
#include <memory>
#include <scoped_allocator>
#include <new>
#include <limits>
#include <climits>
#include <cfloat>
#include <cstdint>
#include <cstddef>
#include <cstdbool>
#include <cstdarg>
#include <csetjmp>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <cwctype>
#include <cwchar>
#include <cuchar>
#include <cstdint>
#include <cinttypes>
#include <cstdbool>
#include <cstdarg>
#include <csetjmp>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <cwctype>
#include <cwchar>
#include <cuchar>

// Forward declarations
class CrossEngineAdapter;
class AdapterFactory;
class GameProfile;
class CrossEngineAdapterRegistry;
class HookManager;
class VRModManager;

namespace uevr {

// Extended API functionality
namespace APIExtensions {

// API extension types
enum class APIExtensionType {
    UNKNOWN = 0,
    HOOK_EXTENSION,
    ADAPTER_EXTENSION,
    MOD_EXTENSION,
    PLUGIN_EXTENSION,
    UTILITY_EXTENSION,
    DEBUG_EXTENSION,
    PROFILING_EXTENSION,
    NETWORK_EXTENSION,
    STORAGE_EXTENSION,
    SECURITY_EXTENSION,
    CUSTOM_EXTENSION
};

// API extension information
struct APIExtensionInfo {
    std::string extension_id;
    std::string extension_name;
    std::string extension_version;
    std::string extension_description;
    std::string extension_author;
    std::string extension_website;
    APIExtensionType extension_type;
    std::vector<std::string> dependencies;
    std::vector<std::string> conflicts;
    std::vector<std::string> features;
    std::unordered_map<std::string, std::string> metadata;
    std::chrono::steady_clock::time_point load_time;
    bool enabled;
    bool auto_load;
    uint32_t priority;
};

// API extension manager
class APIExtensionManager {
public:
    // Singleton access
    static APIExtensionManager& getInstance();
    
    // Constructor and destructor
    explicit APIExtensionManager();
    ~APIExtensionManager();
    
    // Copy and move operations
    APIExtensionManager(const APIExtensionManager&) = delete;
    APIExtensionManager& operator=(const APIExtensionManager&) = delete;
    APIExtensionManager(APIExtensionManager&&) = delete;
    APIExtensionManager& operator=(APIExtensionManager&&) = delete;

    // Extension management
    bool loadExtension(const std::string& extension_id);
    bool unloadExtension(const std::string& extension_id);
    bool isExtensionLoaded(const std::string& extension_id) const;
    bool enableExtension(const std::string& extension_id);
    bool disableExtension(const std::string& extension_id);
    
    // Extension information
    const APIExtensionInfo* getExtensionInfo(const std::string& extension_id) const;
    std::vector<APIExtensionInfo> getLoadedExtensions() const;
    std::vector<APIExtensionInfo> getAvailableExtensions() const;
    std::vector<APIExtensionInfo> getExtensionsByType(APIExtensionType type) const;
    
    // Extension discovery
    bool scanForExtensions();
    bool reloadExtensions();
    std::vector<std::string> getExtensionDependencies(const std::string& extension_id) const;
    std::vector<std::string> getExtensionConflicts(const std::string& extension_id) const;
    
    // Extension lifecycle
    bool initializeExtension(const std::string& extension_id);
    void shutdownExtension(const std::string& extension_id);
    bool isExtensionInitialized(const std::string& extension_id) const;
    
    // Extension validation
    bool validateExtension(const std::string& extension_id) const;
    bool checkExtensionCompatibility(const std::string& extension_id) const;
    bool resolveExtensionDependencies(const std::string& extension_id);
    
    // Extension callbacks
    using ExtensionLoadCallback = std::function<void(const std::string&, bool)>;
    using ExtensionStateCallback = std::function<void(const std::string&, bool)>;
    void setExtensionLoadCallback(ExtensionLoadCallback callback);
    void setExtensionStateCallback(ExtensionStateCallback callback);
    void clearCallbacks();

protected:
    // Internal extension management
    bool loadExtensionInternal(const std::string& extension_id);
    bool unloadExtensionInternal(const std::string& extension_id);
    bool initializeExtensionInternal(const std::string& extension_id);
    void shutdownExtensionInternal(const std::string& extension_id);
    
    // Extension discovery methods
    bool scanForExtensionsInternal();
    bool discoverExtensionsInDirectory(const std::string& directory_path);
    bool discoverExtensionsFromConfig(const std::string& config_path);
    
    // Extension validation methods
    bool validateExtensionInternal(const std::string& extension_id) const;
    bool checkExtensionCompatibilityInternal(const std::string& extension_id) const;
    bool resolveExtensionDependenciesInternal(const std::string& extension_id);
    
    // Callback management
    void notifyExtensionLoad(const std::string& extension_id, bool loaded);
    void notifyExtensionState(const std::string& extension_id, bool enabled);

private:
    // Extension storage
    std::unordered_map<std::string, APIExtensionInfo> m_loaded_extensions;
    std::unordered_map<std::string, APIExtensionInfo> m_available_extensions;
    std::unordered_map<APIExtensionType, std::vector<std::string>> m_extensions_by_type;
    
    // Callbacks
    ExtensionLoadCallback m_extension_load_callback;
    ExtensionStateCallback m_extension_state_callback;
    
    // Internal state
    bool m_initialized;
    std::mutex m_extension_mutex;
    std::shared_mutex m_extension_shared_mutex;
};

} // namespace APIExtensions

// Advanced API features
namespace AdvancedAPI {

// API performance profiler
class APIPerformanceProfiler {
public:
    // Singleton access
    static APIPerformanceProfiler& getInstance();
    
    // Constructor and destructor
    explicit APIPerformanceProfiler();
    ~APIPerformanceProfiler();
    
    // Copy and move operations
    APIPerformanceProfiler(const APIPerformanceProfiler&) = delete;
    APIPerformanceProfiler& operator=(const APIPerformanceProfiler&) = delete;
    APIPerformanceProfiler(APIPerformanceProfiler&&) = delete;
    APIPerformanceProfiler& operator=(APIPerformanceProfiler&&) = delete;

    // Profiling control
    void startProfiling();
    void stopProfiling();
    void pauseProfiling();
    void resumeProfiling();
    bool isProfiling() const;
    bool isPaused() const;
    
    // Performance measurement
    void beginFrame();
    void endFrame();
    void beginSection(const std::string& section_name);
    void endSection(const std::string& section_name);
    void markEvent(const std::string& event_name);
    
    // Performance data
    double getFrameTime() const;
    double getAverageFrameTime() const;
    double getMinFrameTime() const;
    double getMaxFrameTime() const;
    double getSectionTime(const std::string& section_name) const;
    double getAverageSectionTime(const std::string& section_name) const;
    
    // Performance reporting
    std::string getPerformanceReport() const;
    void exportPerformanceData(const std::string& file_path) const;
    void resetPerformanceData();
    
    // Performance optimization
    void setPerformanceThreshold(double threshold_ms);
    double getPerformanceThreshold() const;
    void enableAutoOptimization(bool enable);
    bool isAutoOptimizationEnabled() const;
    void optimizePerformance();

protected:
    // Internal profiling methods
    void updatePerformanceStats();
    void optimizePerformanceInternal();
    void logPerformanceData() const;

private:
    // Profiling state
    bool m_profiling;
    bool m_paused;
    std::chrono::steady_clock::time_point m_frame_start_time;
    std::chrono::steady_clock::time_point m_section_start_time;
    
    // Performance data
    std::vector<double> m_frame_times;
    std::unordered_map<std::string, std::vector<double>> m_section_times;
    std::vector<std::string> m_events;
    
    // Performance thresholds
    double m_performance_threshold;
    bool m_auto_optimization_enabled;
    
    // Internal state
    std::mutex m_profiler_mutex;
    std::shared_mutex m_profiler_shared_mutex;
};

// API memory manager
class APIMemoryManager {
public:
    // Singleton access
    static APIMemoryManager& getInstance();
    
    // Constructor and destructor
    explicit APIMemoryManager();
    ~APIMemoryManager();
    
    // Copy and move operations
    APIMemoryManager(const APIMemoryManager&) = delete;
    APIMemoryManager& operator=(const APIMemoryManager&) = delete;
    APIMemoryManager(APIMemoryManager&&) = delete;
    APIMemoryManager& operator=(APIMemoryManager&&) = delete;

    // Memory allocation
    void* allocate(size_t size, const std::string& allocation_name = "");
    void deallocate(void* ptr);
    void* reallocate(void* ptr, size_t new_size);
    
    // Memory information
    size_t getCurrentMemoryUsage() const;
    size_t getPeakMemoryUsage() const;
    size_t getTotalAllocatedMemory() const;
    size_t getTotalFreedMemory() const;
    size_t getAllocationCount() const;
    size_t getDeallocationCount() const;
    
    // Memory tracking
    void enableMemoryTracking(bool enable);
    bool isMemoryTrackingEnabled() const;
    void setMemoryLimit(size_t limit_bytes);
    size_t getMemoryLimit() const;
    bool isMemoryLimitExceeded() const;
    
    // Memory optimization
    void optimizeMemory();
    void garbageCollect();
    void defragmentMemory();
    void clearMemoryCache();
    
    // Memory reporting
    std::string getMemoryReport() const;
    void exportMemoryData(const std::string& file_path) const;
    void resetMemoryStats();

protected:
    // Internal memory management
    void* allocateInternal(size_t size, const std::string& allocation_name);
    void deallocateInternal(void* ptr);
    void* reallocateInternal(void* ptr, size_t new_size);
    
    // Memory optimization methods
    void optimizeMemoryInternal();
    void garbageCollectInternal();
    void defragmentMemoryInternal();
    
    // Memory tracking methods
    void updateMemoryStats();
    void checkMemoryLimits();
    void logMemoryUsage() const;

private:
    // Memory state
    bool m_memory_tracking_enabled;
    size_t m_memory_limit;
    size_t m_current_memory_usage;
    size_t m_peak_memory_usage;
    size_t m_total_allocated_memory;
    size_t m_total_freed_memory;
    size_t m_allocation_count;
    size_t m_deallocation_count;
    
    // Memory tracking data
    std::unordered_map<void*, std::pair<size_t, std::string>> m_allocations;
    std::vector<std::pair<size_t, std::chrono::steady_clock::time_point>> m_memory_history;
    
    // Internal state
    std::mutex m_memory_mutex;
    std::shared_mutex m_memory_shared_mutex;
};

// API thread manager
class APIThreadManager {
public:
    // Singleton access
    static APIThreadManager& getInstance();
    
    // Constructor and destructor
    explicit APIThreadManager();
    ~APIThreadManager();
    
    // Copy and move operations
    APIThreadManager(const APIThreadManager&) = delete;
    APIThreadManager& operator=(const APIThreadManager&) = delete;
    APIThreadManager(APIThreadManager&&) = delete;
    APIThreadManager& operator=(APIThreadManager&&) = delete;

    // Thread management
    std::thread::id createThread(std::function<void()> task, const std::string& thread_name = "");
    bool joinThread(std::thread::id thread_id);
    bool detachThread(std::thread::id thread_id);
    bool terminateThread(std::thread::id thread_id);
    
    // Thread information
    bool isThreadRunning(std::thread::id thread_id) const;
    bool isThreadJoinable(std::thread::id thread_id) const;
    std::string getThreadName(std::thread::id thread_id) const;
    std::thread::id getCurrentThreadId() const;
    
    // Thread pool management
    void setThreadPoolSize(size_t size);
    size_t getThreadPoolSize() const;
    void enableThreadPool(bool enable);
    bool isThreadPoolEnabled() const;
    
    // Thread scheduling
    void setThreadPriority(std::thread::id thread_id, int priority);
    int getThreadPriority(std::thread::id thread_id) const;
    void setThreadAffinity(std::thread::id thread_id, uint64_t affinity_mask);
    uint64_t getThreadAffinity(std::thread::id thread_id) const;
    
    // Thread monitoring
    size_t getActiveThreadCount() const;
    size_t getIdleThreadCount() const;
    size_t getTotalThreadCount() const;
    std::string getThreadStatusReport() const;
    
    // Thread optimization
    void optimizeThreadUsage();
    void balanceThreadLoad();
    void clearIdleThreads();

protected:
    // Internal thread management
    std::thread::id createThreadInternal(std::function<void()> task, const std::string& thread_name);
    bool joinThreadInternal(std::thread::id thread_id);
    bool detachThreadInternal(std::thread::id thread_id);
    bool terminateThreadInternal(std::thread::id thread_id);
    
    // Thread optimization methods
    void optimizeThreadUsageInternal();
    void balanceThreadLoadInternal();
    void clearIdleThreadsInternal();
    
    // Thread monitoring methods
    void updateThreadStats();
    void monitorThreadHealth();
    void logThreadStatus() const;

private:
    // Thread state
    std::unordered_map<std::thread::id, std::thread> m_threads;
    std::unordered_map<std::thread::id, std::string> m_thread_names;
    std::unordered_map<std::thread::id, bool> m_thread_running;
    std::unordered_map<std::thread::id, bool> m_thread_joinable;
    
    // Thread pool
    size_t m_thread_pool_size;
    bool m_thread_pool_enabled;
    std::queue<std::function<void()>> m_task_queue;
    
    // Thread monitoring
    size_t m_active_thread_count;
    size_t m_idle_thread_count;
    size_t m_total_thread_count;
    
    // Internal state
    std::mutex m_thread_mutex;
    std::shared_mutex m_thread_shared_mutex;
    std::condition_variable m_thread_condition;
};

} // namespace AdvancedAPI

// API utility functions
namespace APIUtils {

// String utilities
std::string toLower(const std::string& str);
std::string toUpper(const std::string& str);
std::string trim(const std::string& str);
std::string trimLeft(const std::string& str);
std::string trimRight(const std::string& str);
std::vector<std::string> split(const std::string& str, char delimiter);
std::vector<std::string> split(const std::string& str, const std::string& delimiter);
std::string join(const std::vector<std::string>& strings, const std::string& separator);
std::string replace(const std::string& str, const std::string& from, const std::string& to);
std::string replaceAll(const std::string& str, const std::string& from, const std::string& to);
bool startsWith(const std::string& str, const std::string& prefix);
bool endsWith(const std::string& str, const std::string& suffix);
bool contains(const std::string& str, const std::string& substring);
std::string format(const std::string& format, ...);

// File utilities
bool fileExists(const std::string& file_path);
bool directoryExists(const std::string& directory_path);
bool createDirectory(const std::string& directory_path);
bool createDirectories(const std::string& directory_path);
bool deleteFile(const std::string& file_path);
bool deleteDirectory(const std::string& directory_path);
bool copyFile(const std::string& source_path, const std::string& destination_path);
bool moveFile(const std::string& source_path, const std::string& destination_path);
std::string getFileExtension(const std::string& file_path);
std::string getFileName(const std::string& file_path);
std::string getDirectoryPath(const std::string& file_path);
std::string getAbsolutePath(const std::string& relative_path);
std::string getRelativePath(const std::string& absolute_path, const std::string& base_path);
uint64_t getFileSize(const std::string& file_path);
std::chrono::system_clock::time_point getFileModificationTime(const std::string& file_path);
std::vector<std::string> listFiles(const std::string& directory_path);
std::vector<std::string> listDirectories(const std::string& directory_path);
std::vector<std::string> findFiles(const std::string& directory_path, const std::string& pattern);

// Time utilities
std::string getCurrentTimestamp();
std::string getCurrentDate();
std::string getCurrentTime();
std::chrono::system_clock::time_point getCurrentSystemTime();
std::chrono::steady_clock::time_point getCurrentSteadyTime();
double getElapsedTime(const std::chrono::steady_clock::time_point& start_time);
std::string formatDuration(std::chrono::milliseconds duration);
std::string formatDuration(std::chrono::seconds duration);
std::string formatDuration(std::chrono::minutes duration);
std::string formatDuration(std::chrono::hours duration);

// Math utilities
template<typename T>
T clamp(T value, T min_value, T max_value) {
    return std::clamp(value, min_value, max_value);
}

template<typename T>
T lerp(T a, T b, double t) {
    return static_cast<T>(a + (b - a) * t);
}

template<typename T>
T smoothstep(T edge0, T edge1, T x) {
    T t = clamp((x - edge0) / (edge1 - edge0), T(0), T(1));
    return t * t * (T(3) - T(2) * t);
}

template<typename T>
T mapRange(T value, T in_min, T in_max, T out_min, T out_max) {
    return (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// Random utilities
class RandomGenerator {
public:
    explicit RandomGenerator(uint32_t seed = 0);
    
    // Integer random numbers
    int32_t randomInt(int32_t min, int32_t max);
    uint32_t randomUInt(uint32_t min, uint32_t max);
    int64_t randomLong(int64_t min, int64_t max);
    uint64_t randomULong(uint64_t min, uint64_t max);
    
    // Floating point random numbers
    float randomFloat(float min, float max);
    double randomDouble(double min, double max);
    
    // Boolean random
    bool randomBool();
    bool randomBool(double probability);
    
    // Vector random numbers
    std::vector<int32_t> randomIntVector(size_t size, int32_t min, int32_t max);
    std::vector<float> randomFloatVector(size_t size, float min, float max);
    
    // Random selection
    template<typename T>
    T randomChoice(const std::vector<T>& choices) {
        if (choices.empty()) {
            throw std::runtime_error("Cannot choose from empty vector");
        }
        return choices[randomUInt(0, static_cast<uint32_t>(choices.size() - 1))];
    }
    
    // Random shuffle
    template<typename T>
    void randomShuffle(std::vector<T>& vector) {
        std::shuffle(vector.begin(), vector.end(), m_random_engine);
    }

private:
    std::mt19937 m_random_engine;
    std::uniform_int_distribution<uint32_t> m_int_distribution;
    std::uniform_real_distribution<float> m_float_distribution;
    std::uniform_real_distribution<double> m_double_distribution;
};

// Hash utilities
uint32_t hash32(const std::string& str);
uint64_t hash64(const std::string& str);
std::string hashString(const std::string& str);
std::string hashFile(const std::string& file_path);
bool verifyHash(const std::string& data, const std::string& expected_hash);

// Encoding utilities
std::string encodeBase64(const std::string& data);
std::string decodeBase64(const std::string& encoded_data);
std::string encodeHex(const std::string& data);
std::string decodeHex(const std::string& hex_data);
std::string encodeUrl(const std::string& data);
std::string decodeUrl(const std::string& encoded_data);

// Validation utilities
bool isValidEmail(const std::string& email);
bool isValidUrl(const std::string& url);
bool isValidIpAddress(const std::string& ip_address);
bool isValidPort(uint16_t port);
bool isValidFileName(const std::string& file_name);
bool isValidDirectoryName(const std::string& directory_name);
bool isValidPath(const std::string& path);

// Conversion utilities
template<typename T>
std::string toString(T value) {
    if constexpr (std::is_same_v<T, std::string>) {
        return value;
    } else if constexpr (std::is_integral_v<T>) {
        return std::to_string(value);
    } else if constexpr (std::is_floating_point_v<T>) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(10) << value;
        return oss.str();
    } else if constexpr (std::is_enum_v<T>) {
        return std::to_string(static_cast<std::underlying_type_t<T>>(value));
    } else {
        static_assert(std::is_convertible_v<T, std::string>, "Type must be convertible to string");
        return static_cast<std::string>(value);
    }
}

template<typename T>
T fromString(const std::string& str) {
    if constexpr (std::is_same_v<T, std::string>) {
        return str;
    } else if constexpr (std::is_integral_v<T>) {
        if constexpr (std::is_unsigned_v<T>) {
            return static_cast<T>(std::stoull(str));
        } else {
            return static_cast<T>(std::stoll(str));
        }
    } else if constexpr (std::is_floating_point_v<T>) {
        return static_cast<T>(std::stod(str));
    } else if constexpr (std::is_enum_v<T>) {
        return static_cast<T>(std::stoll(str));
    } else {
        static_assert(std::is_constructible_v<T, std::string>, "Type must be constructible from string");
        return T(str);
    }
}

// Type utilities
template<typename T>
bool isNumeric() {
    return std::is_arithmetic_v<T>;
}

template<typename T>
bool isIntegral() {
    return std::is_integral_v<T>;
}

template<typename T>
bool isFloatingPoint() {
    return std::is_floating_point_v<T>;
}

template<typename T>
bool isString() {
    return std::is_same_v<T, std::string> || std::is_same_v<T, const char*> || std::is_same_v<T, char*>;
}

template<typename T>
bool isContainer() {
    return std::is_same_v<T, std::vector<typename T::value_type>> ||
           std::is_same_v<T, std::list<typename T::value_type>> ||
           std::is_same_v<T, std::deque<typename T::value_type>> ||
           std::is_same_v<T, std::set<typename T::value_type>> ||
           std::is_same_v<T, std::map<typename T::key_type, typename T::mapped_type>> ||
           std::is_same_v<T, std::unordered_map<typename T::key_type, typename T::mapped_type>>;
}

} // namespace APIUtils

} // namespace uevr

// Backward compatibility alias
namespace uevr = uevr;