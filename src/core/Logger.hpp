#pragma once

#include <memory>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <mutex>
#include <atomic>
#include <chrono>
#include <thread>
#include <queue>
#include <condition_variable>
#include <functional>

namespace UEVR {

/**
 * @brief Log levels
 */
enum class LogLevel {
    TRACE = 0,
    DEBUG = 1,
    INFO = 2,
    WARNING = 3,
    ERROR = 4,
    CRITICAL = 5,
    OFF = 6
};

/**
 * @brief Log entry structure
 */
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string category;
    std::string function;
    std::string file;
    int line;
    std::chrono::system_clock::time_point timestamp;
    std::thread::id threadId;
    
    LogEntry() : level(LogLevel::INFO), line(0) {}
    
    LogEntry(LogLevel lvl, const std::string& msg, const std::string& cat = "",
             const std::string& func = "", const std::string& f = "", int l = 0)
        : level(lvl), message(msg), category(cat), function(func), file(f), line(l)
        , timestamp(std::chrono::system_clock::now())
        , threadId(std::this_thread::get_id()) {}
};

/**
 * @brief Log sink interface
 */
class ILogSink {
public:
    virtual ~ILogSink() = default;
    virtual void Write(const LogEntry& entry) = 0;
    virtual void Flush() = 0;
    virtual std::string GetName() const = 0;
    virtual bool IsEnabled() const = 0;
    virtual void SetEnabled(bool enabled) = 0;
    virtual LogLevel GetMinLevel() const = 0;
    virtual void SetMinLevel(LogLevel level) = 0;
};

/**
 * @brief Console log sink
 */
class ConsoleSink : public ILogSink {
public:
    ConsoleSink();
    ~ConsoleSink() override = default;
    
    void Write(const LogEntry& entry) override;
    void Flush() override;
    std::string GetName() const override { return "Console"; }
    bool IsEnabled() const override { return m_enabled; }
    void SetEnabled(bool enabled) override { m_enabled = enabled; }
    LogLevel GetMinLevel() const override { return m_minLevel; }
    void SetMinLevel(LogLevel level) override { m_minLevel = level; }
    
    void SetColorEnabled(bool enabled) { m_colorEnabled = enabled; }
    bool IsColorEnabled() const { return m_colorEnabled; }

private:
    std::atomic<bool> m_enabled;
    std::atomic<LogLevel> m_minLevel;
    std::atomic<bool> m_colorEnabled;
    mutable std::mutex m_mutex;
    
    std::string GetColorCode(LogLevel level) const;
    std::string GetResetCode() const;
};

/**
 * @brief File log sink
 */
class FileSink : public ILogSink {
public:
    FileSink(const std::string& filename, bool append = true);
    ~FileSink() override;
    
    void Write(const LogEntry& entry) override;
    void Flush() override;
    std::string GetName() const override { return "File"; }
    bool IsEnabled() const override { return m_enabled && m_file.is_open(); }
    void SetEnabled(bool enabled) override { m_enabled = enabled; }
    LogLevel GetMinLevel() const override { return m_minLevel; }
    void SetMinLevel(LogLevel level) override { m_minLevel = level; }
    
    void SetMaxFileSize(size_t maxSize) { m_maxFileSize = maxSize; }
    size_t GetMaxFileSize() const { return m_maxFileSize; }
    void SetMaxFiles(int maxFiles) { m_maxFiles = maxFiles; }
    int GetMaxFiles() const { return m_maxFiles; }
    
    std::string GetFilename() const { return m_filename; }
    size_t GetCurrentSize() const;

private:
    std::string m_filename;
    std::ofstream m_file;
    std::atomic<bool> m_enabled;
    std::atomic<LogLevel> m_minLevel;
    mutable std::mutex m_mutex;
    
    size_t m_maxFileSize;
    int m_maxFiles;
    size_t m_currentSize;
    
    void RotateFile();
    std::string GetRotatedFilename(int index) const;
};

/**
 * @brief Memory log sink (circular buffer)
 */
class MemorySink : public ILogSink {
public:
    MemorySink(size_t maxEntries = 1000);
    ~MemorySink() override = default;
    
    void Write(const LogEntry& entry) override;
    void Flush() override {}
    std::string GetName() const override { return "Memory"; }
    bool IsEnabled() const override { return m_enabled; }
    void SetEnabled(bool enabled) override { m_enabled = enabled; }
    LogLevel GetMinLevel() const override { return m_minLevel; }
    void SetMinLevel(LogLevel level) override { m_minLevel = level; }
    
    std::vector<LogEntry> GetEntries() const;
    std::vector<LogEntry> GetEntries(LogLevel minLevel) const;
    std::vector<LogEntry> GetEntries(const std::string& category) const;
    void Clear();
    
    size_t GetMaxEntries() const { return m_maxEntries; }
    void SetMaxEntries(size_t maxEntries);

private:
    std::vector<LogEntry> m_entries;
    size_t m_maxEntries;
    size_t m_writeIndex;
    std::atomic<bool> m_enabled;
    std::atomic<LogLevel> m_minLevel;
    mutable std::mutex m_mutex;
};

/**
 * @brief Logger class
 * 
 * Multi-threaded, multi-sink logging system
 */
class Logger {
public:
    /**
     * @brief Constructor
     */
    Logger();
    
    /**
     * @brief Destructor
     */
    ~Logger();
    
    /**
     * @brief Initialize the logger
     * @return true if initialization successful, false otherwise
     */
    bool Initialize();
    
    /**
     * @brief Shutdown the logger
     */
    void Shutdown();
    
    /**
     * @brief Add log sink
     * @param sink Log sink instance
     * @return true if sink added successfully, false otherwise
     */
    bool AddSink(std::shared_ptr<ILogSink> sink);
    
    /**
     * @brief Remove log sink
     * @param sinkName Sink name
     * @return true if sink removed successfully, false otherwise
     */
    bool RemoveSink(const std::string& sinkName);
    
    /**
     * @brief Get log sink
     * @param sinkName Sink name
     * @return Sink instance or nullptr if not found
     */
    std::shared_ptr<ILogSink> GetSink(const std::string& sinkName) const;
    
    /**
     * @brief Log a message
     * @param level Log level
     * @param category Log category
     * @param message Log message
     * @param function Function name
     * @param file File name
     * @param line Line number
     */
    void Log(LogLevel level, const std::string& category, const std::string& message,
             const std::string& function = "", const std::string& file = "", int line = 0);
    
    /**
     * @brief Log a formatted message
     * @param level Log level
     * @param category Log category
     * @param format Format string
     * @param args Format arguments
     */
    template<typename... Args>
    void LogFormatted(LogLevel level, const std::string& category, const std::string& format, Args&&... args) {
        if (!ShouldLog(level)) return;
        
        try {
            std::ostringstream oss;
            FormatMessage(oss, format, std::forward<Args>(args)...);
            Log(level, category, oss.str());
        } catch (const std::exception& e) {
            Log(LogLevel::ERROR, "Logger", "Format error: " + std::string(e.what()));
        }
    }
    
    /**
     * @brief Convenience logging methods
     */
    void Trace(const std::string& category, const std::string& message) { Log(LogLevel::TRACE, category, message); }
    void Debug(const std::string& category, const std::string& message) { Log(LogLevel::DEBUG, category, message); }
    void Info(const std::string& category, const std::string& message) { Log(LogLevel::INFO, category, message); }
    void Warning(const std::string& category, const std::string& message) { Log(LogLevel::WARNING, category, message); }
    void Error(const std::string& category, const std::string& message) { Log(LogLevel::ERROR, category, message); }
    void Critical(const std::string& category, const std::string& message) { Log(LogLevel::CRITICAL, category, message); }
    
    /**
     * @brief Formatted convenience methods
     */
    template<typename... Args>
    void Trace(const std::string& category, const std::string& format, Args&&... args) {
        LogFormatted(LogLevel::TRACE, category, format, std::forward<Args>(args)...);
    }
    
    template<typename... Args>
    void Debug(const std::string& category, const std::string& format, Args&&... args) {
        LogFormatted(LogLevel::DEBUG, category, format, std::forward<Args>(args)...);
    }
    
    template<typename... Args>
    void Info(const std::string& category, const std::string& format, Args&&... args) {
        LogFormatted(LogLevel::INFO, category, format, std::forward<Args>(args)...);
    }
    
    template<typename... Args>
    void Warning(const std::string& category, const std::string& format, Args&&... args) {
        LogFormatted(LogLevel::WARNING, category, format, std::forward<Args>(args)...);
    }
    
    template<typename... Args>
    void Error(const std::string& category, const std::string& format, Args&&... args) {
        LogFormatted(LogLevel::ERROR, category, format, std::forward<Args>(args)...);
    }
    
    template<typename... Args>
    void Critical(const std::string& category, const std::string& format, Args&&... args) {
        LogFormatted(LogLevel::CRITICAL, category, format, std::forward<Args>(args)...);
    }
    
    /**
     * @brief Set global log level
     * @param level Minimum log level
     */
    void SetLogLevel(LogLevel level) { m_globalLevel = level; }
    
    /**
     * @brief Get global log level
     * @return Current global log level
     */
    LogLevel GetLogLevel() const { return m_globalLevel; }
    
    /**
     * @brief Check if should log at level
     * @param level Log level to check
     * @return true if should log, false otherwise
     */
    bool ShouldLog(LogLevel level) const { return level >= m_globalLevel; }
    
    /**
     * @brief Enable/disable async logging
     * @param enabled true to enable async logging, false for sync
     */
    void SetAsyncEnabled(bool enabled);
    
    /**
     * @brief Check if async logging is enabled
     * @return true if async enabled, false otherwise
     */
    bool IsAsyncEnabled() const { return m_asyncEnabled; }
    
    /**
     * @brief Flush all sinks
     */
    void Flush();
    
    /**
     * @brief Get log statistics
     * @return Log statistics as JSON string
     */
    std::string GetStatistics() const;
    
    /**
     * @brief Set log filter
     * @param filter Filter function
     */
    void SetFilter(std::function<bool(const LogEntry&)> filter) { m_filter = filter; }
    
    /**
     * @brief Remove log filter
     */
    void RemoveFilter() { m_filter = nullptr; }
    
    /**
     * @brief Convert log level to string
     * @param level Log level
     * @return Log level as string
     */
    static std::string LogLevelToString(LogLevel level);
    
    /**
     * @brief Convert string to log level
     * @param str Log level string
     * @return Log level
     */
    static LogLevel StringToLogLevel(const std::string& str);
    
    /**
     * @brief Get singleton instance
     * @return Logger instance
     */
    static Logger& GetInstance();

private:
    std::vector<std::shared_ptr<ILogSink>> m_sinks;
    std::atomic<LogLevel> m_globalLevel;
    std::atomic<bool> m_initialized;
    std::atomic<bool> m_asyncEnabled;
    std::atomic<bool> m_shutdownRequested;
    mutable std::mutex m_sinksMutex;
    
    // Async logging
    std::thread m_workerThread;
    std::queue<LogEntry> m_logQueue;
    std::mutex m_queueMutex;
    std::condition_variable m_queueCondition;
    
    // Statistics
    std::atomic<size_t> m_totalMessages;
    std::atomic<size_t> m_droppedMessages;
    
    // Filter
    std::function<bool(const LogEntry&)> m_filter;
    
    // Internal methods
    void ProcessEntry(const LogEntry& entry);
    void WorkerThread();
    void FlushInternal();
    
    // Template helper for formatting
    template<typename T>
    void FormatMessage(std::ostringstream& oss, const std::string& format, T&& value) {
        size_t pos = format.find("{}");
        if (pos != std::string::npos) {
            oss << format.substr(0, pos) << std::forward<T>(value) << format.substr(pos + 2);
        } else {
            oss << format;
        }
    }
    
    template<typename T, typename... Args>
    void FormatMessage(std::ostringstream& oss, const std::string& format, T&& value, Args&&... args) {
        size_t pos = format.find("{}");
        if (pos != std::string::npos) {
            std::string remaining = format.substr(0, pos) + std::to_string(std::forward<T>(value)) + format.substr(pos + 2);
            FormatMessage(oss, remaining, std::forward<Args>(args)...);
        } else {
            oss << format;
        }
    }
};

} // namespace UEVR

// Logging macros
#define UEVR_LOG(level, category, message) \
    UEVR::Logger::GetInstance().Log(level, category, message, __FUNCTION__, __FILE__, __LINE__)

#define UEVR_LOG_TRACE(category, message) UEVR_LOG(UEVR::LogLevel::TRACE, category, message)
#define UEVR_LOG_DEBUG(category, message) UEVR_LOG(UEVR::LogLevel::DEBUG, category, message)
#define UEVR_LOG_INFO(category, message) UEVR_LOG(UEVR::LogLevel::INFO, category, message)
#define UEVR_LOG_WARNING(category, message) UEVR_LOG(UEVR::LogLevel::WARNING, category, message)
#define UEVR_LOG_ERROR(category, message) UEVR_LOG(UEVR::LogLevel::ERROR, category, message)
#define UEVR_LOG_CRITICAL(category, message) UEVR_LOG(UEVR::LogLevel::CRITICAL, category, message)

#define UEVR_LOG_FORMATTED(level, category, format, ...) \
    UEVR::Logger::GetInstance().LogFormatted(level, category, format, __VA_ARGS__)

#define UEVR_TRACE(category, format, ...) UEVR_LOG_FORMATTED(UEVR::LogLevel::TRACE, category, format, __VA_ARGS__)
#define UEVR_DEBUG(category, format, ...) UEVR_LOG_FORMATTED(UEVR::LogLevel::DEBUG, category, format, __VA_ARGS__)
#define UEVR_INFO(category, format, ...) UEVR_LOG_FORMATTED(UEVR::LogLevel::INFO, category, format, __VA_ARGS__)
#define UEVR_WARNING(category, format, ...) UEVR_LOG_FORMATTED(UEVR::LogLevel::WARNING, category, format, __VA_ARGS__)
#define UEVR_ERROR(category, format, ...) UEVR_LOG_FORMATTED(UEVR::LogLevel::ERROR, category, format, __VA_ARGS__)
#define UEVR_CRITICAL(category, format, ...) UEVR_LOG_FORMATTED(UEVR::LogLevel::CRITICAL, category, format, __VA_ARGS__)
