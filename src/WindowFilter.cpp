#include "WindowFilter.hpp"
#include <spdlog/spdlog.h>
#include <algorithm>
#include <sstream>

// To prevent usage of statics (TLS breaks the present thread...?)
std::unique_ptr<WindowFilter> g_window_filter{};

WindowFilter& WindowFilter::get() {
    if (g_window_filter == nullptr) {
        g_window_filter = std::make_unique<WindowFilter>();
    }

    return *g_window_filter;
}

WindowFilter::WindowFilter() {
    // Initialize common patterns for performance optimization
    initializeCommonPatterns();
    
    // We create a job thread because GetWindowTextA can actually deadlock inside
    // the present thread...
    m_job_thread = std::make_unique<std::jthread>([this](std::stop_token s){
        while (!s.stop_requested()) {
            std::this_thread::sleep_for(std::chrono::milliseconds{100});

            m_last_job_tick = std::chrono::steady_clock::now();

            if (m_window_jobs.empty()) {
                return;
            }

            std::scoped_lock _{m_mutex};

            for (const auto hwnd : m_window_jobs) {
                if (is_filtered_nocache(hwnd)) {
                    filter_window(hwnd);
                    update_filtering_statistics();
                }
            }

            m_window_jobs.clear();
        }
    });
    
    spdlog::info("[WindowFilter] Window filter initialized with comprehensive filtering capabilities");
}

WindowFilter::~WindowFilter() {
    m_job_thread->request_stop();
    m_job_thread->join();
    
    spdlog::info("[WindowFilter] Window filter shutdown completed");
}

bool WindowFilter::is_filtered(HWND hwnd) {
    if (hwnd == nullptr) {
        return true;
    }
    
    if (!m_filtering_enabled) {
        return false;
    }
    
    std::scoped_lock _{m_mutex};

    if (m_filtered_windows.find(hwnd) != m_filtered_windows.end()) {
        return true;
    }

    // If there is a job for this window, filter it until the job is done
    if (m_window_jobs.find(hwnd) != m_window_jobs.end()) {
        // If the thread is dead for some reason, do not filter it.
        return std::chrono::steady_clock::now() - m_last_job_tick <= std::chrono::seconds{2};
    }

    // if we havent even seen this window yet, add it to the job queue
    // and return true;
    if (m_seen_windows.find(hwnd) == m_seen_windows.end()) {
        m_seen_windows.insert(hwnd);
        m_window_jobs.insert(hwnd);
        m_total_seen_windows++;
        return true;
    }

    return false;
}

// Enhanced filtering capabilities
void WindowFilter::add_custom_filter(const std::string& pattern) {
    std::scoped_lock _{m_mutex};
    
    if (m_custom_filter_set.find(pattern) == m_custom_filter_set.end()) {
        m_custom_filters.push_back(pattern);
        m_custom_filter_set.insert(pattern);
        spdlog::info("[WindowFilter] Added custom filter: {}", pattern);
    }
}

void WindowFilter::remove_custom_filter(const std::string& pattern) {
    std::scoped_lock _{m_mutex};
    
    auto it = std::find(m_custom_filters.begin(), m_custom_filters.end(), pattern);
    if (it != m_custom_filters.end()) {
        m_custom_filters.erase(it);
        m_custom_filter_set.erase(pattern);
        spdlog::info("[WindowFilter] Removed custom filter: {}", pattern);
    }
}

void WindowFilter::clear_custom_filters() {
    std::scoped_lock _{m_mutex};
    
    m_custom_filters.clear();
    m_custom_filter_set.clear();
    spdlog::info("[WindowFilter] Cleared all custom filters");
}

// Window management
void WindowFilter::force_filter_window(HWND hwnd) {
    if (hwnd == nullptr) {
        return;
    }
    
    std::scoped_lock _{m_mutex};
    
    m_filtered_windows.insert(hwnd);
    m_total_filtered_windows++;
    
    if (m_debug_mode) {
        char window_name[256]{};
        GetWindowTextA(hwnd, window_name, sizeof(window_name));
        spdlog::debug("[WindowFilter] Force filtered window: {}", window_name);
    }
}

void WindowFilter::remove_filtered_window(HWND hwnd) {
    if (hwnd == nullptr) {
        return;
    }
    
    std::scoped_lock _{m_mutex};
    
    if (m_filtered_windows.erase(hwnd) > 0) {
        m_total_filtered_windows--;
        spdlog::debug("[WindowFilter] Removed filtered window: {:p}", (void*)hwnd);
    }
}

void WindowFilter::clear_all_filters() {
    std::scoped_lock _{m_mutex};
    
    m_filtered_windows.clear();
    m_seen_windows.clear();
    m_window_jobs.clear();
    m_total_filtered_windows = 0;
    m_total_seen_windows = 0;
    
    spdlog::info("[WindowFilter] Cleared all window filters");
}

// Statistics and monitoring
size_t WindowFilter::get_filtered_window_count() const {
    std::scoped_lock _{m_mutex};
    return m_filtered_windows.size();
}

size_t WindowFilter::get_seen_window_count() const {
    std::scoped_lock _{m_mutex};
    return m_seen_windows.size();
}

size_t WindowFilter::get_job_queue_size() const {
    std::scoped_lock _{m_mutex};
    return m_window_jobs.size();
}

// Configuration
void WindowFilter::set_filtering_enabled(bool enabled) {
    std::scoped_lock _{m_mutex};
    m_filtering_enabled = enabled;
    spdlog::info("[WindowFilter] Filtering {}", enabled ? "enabled" : "disabled");
}

bool WindowFilter::is_filtering_enabled() const {
    std::scoped_lock _{m_mutex};
    return m_filtering_enabled;
}

void WindowFilter::set_debug_mode(bool enabled) {
    std::scoped_lock _{m_mutex};
    m_debug_mode = enabled;
    spdlog::info("[WindowFilter] Debug mode {}", enabled ? "enabled" : "disabled");
}

bool WindowFilter::is_debug_mode() const {
    std::scoped_lock _{m_mutex};
    return m_debug_mode;
}

// Performance optimization
void WindowFilter::optimize_filtering_performance() {
    std::scoped_lock _{m_mutex};
    
    if (m_performance_optimized) {
        return;
    }
    
    // Pre-compile common patterns for faster matching
    m_common_patterns.clear();
    m_common_patterns.insert("Steam");
    m_common_patterns.insert("Discord");
    m_common_patterns.insert("OBS");
    m_common_patterns.insert("Chrome");
    m_common_patterns.insert("Firefox");
    m_common_patterns.insert("Visual Studio");
    m_common_patterns.insert("NVIDIA");
    m_common_patterns.insert("AMD");
    m_common_patterns.insert("Intel");
    
    m_performance_optimized = true;
    spdlog::info("[WindowFilter] Performance optimization enabled");
}

void WindowFilter::clear_cache() {
    std::scoped_lock _{m_mutex};
    
    m_seen_windows.clear();
    m_window_jobs.clear();
    m_total_seen_windows = 0;
    
    spdlog::info("[WindowFilter] Cache cleared");
}

// Private helper methods
bool WindowFilter::check_custom_filters(const std::string_view& window_name) {
    if (m_custom_filters.empty()) {
        return false;
    }
    
    for (const auto& filter : m_custom_filters) {
        if (window_name.find(filter) != std::string_view::npos) {
            return true;
        }
    }
    
    return false;
}

void WindowFilter::log_filtered_window(HWND hwnd, const std::string& reason) {
    if (!m_debug_mode) {
        return;
    }
    
    char window_name[256]{};
    GetWindowTextA(hwnd, window_name, sizeof(window_name));
    
    spdlog::debug("[WindowFilter] Filtered window '{}' ({:p}) - Reason: {}", 
                  window_name, (void*)hwnd, reason);
}

void WindowFilter::update_filtering_statistics() {
    m_filtering_operations++;
    
    if (m_debug_mode && (m_filtering_operations % 100) == 0) {
        spdlog::debug("[WindowFilter] Statistics - Filtered: {}, Seen: {}, Operations: {}", 
                      m_total_filtered_windows, m_total_seen_windows, m_filtering_operations);
    }
}

void WindowFilter::initializeCommonPatterns() {
    // Initialize common patterns for performance optimization
    m_common_patterns.insert("Steam");
    m_common_patterns.insert("Discord");
    m_common_patterns.insert("OBS");
    m_common_patterns.insert("Chrome");
    m_common_patterns.insert("Firefox");
    m_common_patterns.insert("Visual Studio");
    m_common_patterns.insert("NVIDIA");
    m_common_patterns.insert("AMD");
    m_common_patterns.insert("Intel");
}