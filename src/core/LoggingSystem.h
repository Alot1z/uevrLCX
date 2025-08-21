#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <mutex>
#include <fstream>
#include <chrono>
#include <unordered_map> // Added for performance tracking
#include <thread> // Added for std::thread::id

namespace uevrLCX {

enum class LogLevel {
    TRACE,
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

struct LogEntry {
    std::chrono::system_clock::time_point timestamp;
    LogLevel level;
    std::string component;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::thread::id threadId;
};

class LoggingSystem {
public:
    LoggingSystem();
    ~LoggingSystem();

    // Core functionality
    bool initialize();
    void shutdown();
    
    // Logging methods
    void log(LogLevel level, const std::string& message);
    void log(LogLevel level, const std::string& component, const std::string& message);
    void log(LogLevel level, const std::string& component, const std::string& message, 
             const std::string& file, int line, const std::string& function);
    
    // Convenience methods
    void trace(const std::string& message);
    void debug(const std::string& message);
    void info(const std::string& message);
    void warning(const std::string& message);
    void error(const std::string& message);
    void critical(const std::string& message);
    
    // Configuration
    void setLogLevel(LogLevel level);
    LogLevel getLogLevel() const;
    void setLogFile(const std::string& filename);
    void setMaxFileSize(size_t sizeInBytes);
    void setMaxFileCount(size_t count);
    void enableConsoleOutput(bool enable);
    void enableFileOutput(bool enable);
    
    // Output management
    void addOutputCallback(std::function<void(const LogEntry&)> callback);
    void removeOutputCallback(size_t index);
    void clearOutputCallbacks();
    
    // Log retrieval
    std::vector<LogEntry> getRecentLogs(size_t count = 100) const;
    std::vector<LogEntry> getLogsByLevel(LogLevel level, size_t count = 100) const;
    std::vector<LogEntry> getLogsByComponent(const std::string& component, size_t count = 100) const;
    std::vector<LogEntry> getLogsByTimeRange(std::chrono::system_clock::time_point start, 
                                            std::chrono::system_clock::time_point end) const;
    
    // Log analysis
    size_t getLogCount() const;
    size_t getLogCountByLevel(LogLevel level) const;
    size_t getLogCountByComponent(const std::string& component) const;
    LogLevel getMostFrequentLevel() const;
    std::string getMostFrequentComponent() const;
    
    // Log management
    void clearLogs();
    void rotateLogFile();
    bool exportLogs(const std::string& filename, const std::string& format = "json") const;
    
    // Performance monitoring
    void setPerformanceLogging(bool enable);
    void logPerformance(const std::string& operation, double duration);
    std::vector<std::pair<std::string, double>> getPerformanceStats() const;

private:
    // Configuration
    LogLevel currentLevel;
    std::string logFilename;
    size_t maxFileSize;
    size_t maxFileCount;
    bool consoleOutput;
    bool fileOutput;
    bool performanceLogging;
    
    // Output streams
    std::ofstream logFile;
    std::vector<std::function<void(const LogEntry&)>> outputCallbacks;
    
    // Log storage
    std::vector<LogEntry> logBuffer;
    size_t maxBufferSize;
    
    // Performance tracking
    std::unordered_map<std::string, std::vector<double>> performanceData;
    
    // Thread safety
    mutable std::mutex logMutex;
    
    // Internal methods
    void writeLogEntry(const LogEntry& entry);
    std::string formatLogEntry(const LogEntry& entry) const;
    std::string getLogLevelString(LogLevel level) const;
    std::string getTimestampString(const std::chrono::system_clock::time_point& timestamp) const;
    void checkFileRotation();
    void initializeLogFile();
    void closeLogFile();
    
    // Performance methods
    void updatePerformanceStats(const std::string& operation, double duration);
    double calculateAverageDuration(const std::vector<double>& durations) const;
};

// Global logging macros
#define LOG_TRACE(msg) LoggingSystem::getInstance().trace(msg)
#define LOG_DEBUG(msg) LoggingSystem::getInstance().debug(msg)
#define LOG_INFO(msg) LoggingSystem::getInstance().info(msg)
#define LOG_WARNING(msg) LoggingSystem::getInstance().warning(msg)
#define LOG_ERROR(msg) LoggingSystem::getInstance().error(msg)
#define LOG_CRITICAL(msg) LoggingSystem::getInstance().critical(msg)

#define LOG_TRACE_COMPONENT(component, msg) LoggingSystem::getInstance().log(LogLevel::TRACE, component, msg)
#define LOG_DEBUG_COMPONENT(component, msg) LoggingSystem::getInstance().log(LogLevel::DEBUG, component, msg)
#define LOG_INFO_COMPONENT(component, msg) LoggingSystem::getInstance().log(LogLevel::INFO, component, msg)
#define LOG_WARNING_COMPONENT(component, msg) LoggingSystem::getInstance().log(LogLevel::WARNING, component, msg)
#define LOG_ERROR_COMPONENT(component, msg) LoggingSystem::getInstance().log(LogLevel::ERROR, component, msg)
#define LOG_CRITICAL_COMPONENT(component, msg) LoggingSystem::getInstance().log(LogLevel::CRITICAL, component, msg)

#define LOG_TRACE_FULL(component, msg) LoggingSystem::getInstance().log(LogLevel::TRACE, component, msg, __FILE__, __LINE__, __FUNCTION__)
#define LOG_DEBUG_FULL(component, msg) LoggingSystem::getInstance().log(LogLevel::DEBUG, component, msg, __FILE__, __LINE__, __FUNCTION__)
#define LOG_INFO_FULL(component, msg) LoggingSystem::getInstance().log(LogLevel::INFO, component, msg, __FILE__, __LINE__, __FUNCTION__)
#define LOG_WARNING_FULL(component, msg) LoggingSystem::getInstance().log(LogLevel::WARNING, component, msg, __FILE__, __LINE__, __FUNCTION__)
#define LOG_ERROR_FULL(component, msg) LoggingSystem::getInstance().log(LogLevel::ERROR, component, msg, __FILE__, __LINE__, __FUNCTION__)
#define LOG_CRITICAL_FULL(component, msg) LoggingSystem::getInstance().log(LogLevel::CRITICAL, component, msg, __FILE__, __LINE__, __FUNCTION__)

} // namespace uevrLCX
