#pragma once

#include <Windows.h>

#include <string_view>
#include <unordered_set>
#include <thread>
#include <vector>
#include <mutex>
#include <chrono>
#include <string>
#include <functional>

class WindowFilter {
public:
    static WindowFilter& get();

public:
    WindowFilter();
    virtual ~WindowFilter();

    bool is_filtered(HWND hwnd);

    void filter_window(HWND hwnd) {
        std::scoped_lock _{m_mutex};
        m_filtered_windows.insert(hwnd);
    }

    // Enhanced filtering capabilities
    void add_custom_filter(const std::string& pattern);
    void remove_custom_filter(const std::string& pattern);
    void clear_custom_filters();
    
    // Window management
    void force_filter_window(HWND hwnd);
    void remove_filtered_window(HWND hwnd);
    void clear_all_filters();
    
    // Statistics and monitoring
    size_t get_filtered_window_count() const;
    size_t get_seen_window_count() const;
    size_t get_job_queue_size() const;
    
    // Configuration
    void set_filtering_enabled(bool enabled);
    bool is_filtering_enabled() const;
    void set_debug_mode(bool enabled);
    bool is_debug_mode() const;
    
    // Performance optimization
    void optimize_filtering_performance();
    void clear_cache();

private:
    bool is_filtered_nocache(HWND hwnd);
    bool check_custom_filters(const std::string_view& window_name);
    void log_filtered_window(HWND hwnd, const std::string& reason);
    void update_filtering_statistics();

    std::recursive_mutex m_mutex{};
    std::unordered_set<HWND> m_window_jobs{};
    std::unique_ptr<std::jthread> m_job_thread{};

    std::unordered_set<HWND> m_seen_windows{};
    std::unordered_set<HWND> m_filtered_windows{};
    std::chrono::time_point<std::chrono::steady_clock> m_last_job_tick{};
    
    // Enhanced filtering system
    std::vector<std::string> m_custom_filters{};
    std::unordered_set<std::string> m_custom_filter_set{};
    
    // Configuration and state
    bool m_filtering_enabled = true;
    bool m_debug_mode = false;
    
    // Statistics
    size_t m_total_filtered_windows = 0;
    size_t m_total_seen_windows = 0;
    size_t m_filtering_operations = 0;
    
    // Performance optimization
    std::unordered_set<std::string> m_common_patterns{};
    bool m_performance_optimized = false;
};