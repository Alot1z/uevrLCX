#include "LoggingSystem.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace uevrLCX {

LoggingSystem::LoggingSystem()
    : currentLevel(LogLevel::INFO)
    , maxFileSize(10 * 1024 * 1024) // 10MB
    , maxFileCount(5)
    , consoleOutput(true)
    , fileOutput(true)
    , performanceLogging(false)
    , maxBufferSize(10000) {
}

LoggingSystem::~LoggingSystem() {
    shutdown();
}

bool LoggingSystem::initialize() {
    try {
        if (fileOutput && !logFilename.empty()) {
            initializeLogFile();
        }
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to initialize logging system: " << e.what() << std::endl;
        return false;
    }
}

void LoggingSystem::shutdown() {
    closeLogFile();
}

void LoggingSystem::log(LogLevel level, const std::string& message) {
    log(level, "System", message, "", 0, "");
}

void LoggingSystem::log(LogLevel level, const std::string& component, const std::string& message) {
    log(level, component, message, "", 0, "");
}

void LoggingSystem::log(LogLevel level, const std::string& component, const std::string& message,
                       const std::string& file, int line, const std::string& function) {
    if (level < currentLevel) {
        return;
    }
    
    LogEntry entry;
    entry.timestamp = std::chrono::system_clock::now();
    entry.level = level;
    entry.component = component;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.threadId = std::this_thread::get_id();
    
    writeLogEntry(entry);
}

void LoggingSystem::trace(const std::string& message) {
    log(LogLevel::TRACE, message);
}

void LoggingSystem::debug(const std::string& message) {
    log(LogLevel::DEBUG, message);
}

void LoggingSystem::info(const std::string& message) {
    log(LogLevel::INFO, message);
}

void LoggingSystem::warning(const std::string& message) {
    log(LogLevel::WARNING, message);
}

void LoggingSystem::error(const std::string& message) {
    log(LogLevel::ERROR, message);
}

void LoggingSystem::critical(const std::string& message) {
    log(LogLevel::CRITICAL, message);
}

void LoggingSystem::setLogLevel(LogLevel level) {
    currentLevel = level;
}

LogLevel LoggingSystem::getLogLevel() const {
    return currentLevel;
}

void LoggingSystem::setLogFile(const std::string& filename) {
    logFilename = filename;
    if (fileOutput && !logFilename.empty()) {
        initializeLogFile();
    }
}

void LoggingSystem::setMaxFileSize(size_t sizeInBytes) {
    maxFileSize = sizeInBytes;
}

void LoggingSystem::setMaxFileCount(size_t count) {
    maxFileCount = count;
}

void LoggingSystem::enableConsoleOutput(bool enable) {
    consoleOutput = enable;
}

void LoggingSystem::enableFileOutput(bool enable) {
    fileOutput = enable;
    if (enable && !logFilename.empty()) {
        initializeLogFile();
    } else {
        closeLogFile();
    }
}

void LoggingSystem::addOutputCallback(std::function<void(const LogEntry&)> callback) {
    outputCallbacks.push_back(std::move(callback));
}

void LoggingSystem::removeOutputCallback(size_t index) {
    if (index < outputCallbacks.size()) {
        outputCallbacks.erase(outputCallbacks.begin() + index);
    }
}

void LoggingSystem::clearOutputCallbacks() {
    outputCallbacks.clear();
}

std::vector<LogEntry> LoggingSystem::getRecentLogs(size_t count) const {
    std::lock_guard<std::mutex> lock(logMutex);
    
    size_t actualCount = std::min(count, logBuffer.size());
    std::vector<LogEntry> result;
    result.reserve(actualCount);
    
    auto start = logBuffer.end() - actualCount;
    result.assign(start, logBuffer.end());
    
    return result;
}

std::vector<LogEntry> LoggingSystem::getLogsByLevel(LogLevel level, size_t count) const {
    std::lock_guard<std::mutex> lock(logMutex);
    
    std::vector<LogEntry> result;
    result.reserve(count);
    
    for (auto it = logBuffer.rbegin(); it != logBuffer.rend() && result.size() < count; ++it) {
        if (it->level == level) {
            result.push_back(*it);
        }
    }
    
    return result;
}

std::vector<LogEntry> LoggingSystem::getLogsByComponent(const std::string& component, size_t count) const {
    std::lock_guard<std::mutex> lock(logMutex);
    
    std::vector<LogEntry> result;
    result.reserve(count);
    
    for (auto it = logBuffer.rbegin(); it != logBuffer.rend() && result.size() < count; ++it) {
        if (it->component == component) {
            result.push_back(*it);
        }
    }
    
    return result;
}

std::vector<LogEntry> LoggingSystem::getLogsByTimeRange(std::chrono::system_clock::time_point start,
                                                       std::chrono::system_clock::time_point end) const {
    std::lock_guard<std::mutex> lock(logMutex);
    
    std::vector<LogEntry> result;
    
    for (const auto& entry : logBuffer) {
        if (entry.timestamp >= start && entry.timestamp <= end) {
            result.push_back(entry);
        }
    }
    
    return result;
}

size_t LoggingSystem::getLogCount() const {
    std::lock_guard<std::mutex> lock(logMutex);
    return logBuffer.size();
}

size_t LoggingSystem::getLogCountByLevel(LogLevel level) const {
    std::lock_guard<std::mutex> lock(logMutex);
    
    return std::count_if(logBuffer.begin(), logBuffer.end(),
        [level](const LogEntry& entry) { return entry.level == level; });
}

size_t LoggingSystem::getLogCountByComponent(const std::string& component) const {
    std::lock_guard<std::mutex> lock(logMutex);
    
    return std::count_if(logBuffer.begin(), logBuffer.end(),
        [&component](const LogEntry& entry) { return entry.component == component; });
}

LogLevel LoggingSystem::getMostFrequentLevel() const {
    std::lock_guard<std::mutex> lock(logMutex);
    
    std::unordered_map<LogLevel, size_t> levelCounts;
    for (const auto& entry : logBuffer) {
        levelCounts[entry.level]++;
    }
    
    auto maxIt = std::max_element(levelCounts.begin(), levelCounts.end(),
        [](const auto& a, const auto& b) { return a.second < b.second; });
    
    return maxIt != levelCounts.end() ? maxIt->first : LogLevel::INFO;
}

std::string LoggingSystem::getMostFrequentComponent() const {
    std::lock_guard<std::mutex> lock(logMutex);
    
    std::unordered_map<std::string, size_t> componentCounts;
    for (const auto& entry : logBuffer) {
        componentCounts[entry.component]++;
    }
    
    auto maxIt = std::max_element(componentCounts.begin(), componentCounts.end(),
        [](const auto& a, const auto& b) { return a.second < b.second; });
    
    return maxIt != componentCounts.end() ? maxIt->first : "System";
}

void LoggingSystem::clearLogs() {
    std::lock_guard<std::mutex> lock(logMutex);
    logBuffer.clear();
}

void LoggingSystem::rotateLogFile() {
    checkFileRotation();
}

bool LoggingSystem::exportLogs(const std::string& filename, const std::string& format) const {
    try {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        auto logs = getRecentLogs(1000);
        
        if (format == "json") {
            file << "[\n";
            for (size_t i = 0; i < logs.size(); ++i) {
                file << "  {\n";
                file << "    \"timestamp\": \"" << getTimestampString(logs[i].timestamp) << "\",\n";
                file << "    \"level\": \"" << getLogLevelString(logs[i].level) << "\",\n";
                file << "    \"component\": \"" << logs[i].component << "\",\n";
                file << "    \"message\": \"" << logs[i].message << "\"\n";
                file << "  }" << (i < logs.size() - 1 ? "," : "") << "\n";
            }
            file << "]\n";
        } else {
            for (const auto& log : logs) {
                file << formatLogEntry(log) << "\n";
            }
        }
        
        return true;
    } catch (...) {
        return false;
    }
}

void LoggingSystem::setPerformanceLogging(bool enable) {
    performanceLogging = enable;
}

void LoggingSystem::logPerformance(const std::string& operation, double duration) {
    if (!performanceLogging) {
        return;
    }
    
    updatePerformanceStats(operation, duration);
}

std::vector<std::pair<std::string, double>> LoggingSystem::getPerformanceStats() const {
    std::lock_guard<std::mutex> lock(logMutex);
    
    std::vector<std::pair<std::string, double>> result;
    result.reserve(performanceData.size());
    
    for (const auto& pair : performanceData) {
        double avgDuration = calculateAverageDuration(pair.second);
        result.emplace_back(pair.first, avgDuration);
    }
    
    return result;
}

void LoggingSystem::writeLogEntry(const LogEntry& entry) {
    std::lock_guard<std::mutex> lock(logMutex);
    
    // Add to buffer
    logBuffer.push_back(entry);
    if (logBuffer.size() > maxBufferSize) {
        logBuffer.erase(logBuffer.begin());
    }
    
    // Format the log entry
    std::string formattedEntry = formatLogEntry(entry);
    
    // Console output
    if (consoleOutput) {
        std::cout << formattedEntry << std::endl;
    }
    
    // File output
    if (fileOutput && logFile.is_open()) {
        logFile << formattedEntry << std::endl;
        logFile.flush();
        
        checkFileRotation();
    }
    
    // Callbacks
    for (const auto& callback : outputCallbacks) {
        try {
            callback(entry);
        } catch (...) {
            // Ignore callback errors
        }
    }
}

std::string LoggingSystem::formatLogEntry(const LogEntry& entry) const {
    std::ostringstream oss;
    
    oss << "[" << getTimestampString(entry.timestamp) << "] "
        << "[" << getLogLevelString(entry.level) << "] "
        << "[" << entry.component << "] "
        << entry.message;
    
    if (!entry.file.empty()) {
        oss << " (" << entry.file << ":" << entry.line;
        if (!entry.function.empty()) {
            oss << " in " << entry.function;
        }
        oss << ")";
    }
    
    return oss.str();
}

std::string LoggingSystem::getLogLevelString(LogLevel level) const {
    switch (level) {
        case LogLevel::TRACE: return "TRACE";
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO: return "INFO";
        case LogLevel::WARNING: return "WARN";
        case LogLevel::ERROR: return "ERROR";
        case LogLevel::CRITICAL: return "CRIT";
        default: return "UNKNOWN";
    }
}

std::string LoggingSystem::getTimestampString(const std::chrono::system_clock::time_point& timestamp) const {
    auto time_t = std::chrono::system_clock::to_time_t(timestamp);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        timestamp.time_since_epoch()) % 1000;
    
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    oss << "." << std::setfill('0') << std::setw(3) << ms.count();
    
    return oss.str();
}

void LoggingSystem::checkFileRotation() {
    if (!logFile.is_open()) {
        return;
    }
    
    logFile.flush();
    auto currentSize = logFile.tellp();
    
    if (currentSize > static_cast<std::streampos>(maxFileSize)) {
        rotateLogFile();
    }
}

void LoggingSystem::initializeLogFile() {
    closeLogFile();
    
    logFile.open(logFilename, std::ios::app);
    if (!logFile.is_open()) {
        std::cerr << "Failed to open log file: " << logFilename << std::endl;
    }
}

void LoggingSystem::closeLogFile() {
    if (logFile.is_open()) {
        logFile.close();
    }
}

void LoggingSystem::updatePerformanceStats(const std::string& operation, double duration) {
    std::lock_guard<std::mutex> lock(logMutex);
    performanceData[operation].push_back(duration);
    
    // Keep only the last 100 measurements per operation
    if (performanceData[operation].size() > 100) {
        performanceData[operation].erase(performanceData[operation].begin());
    }
}

double LoggingSystem::calculateAverageDuration(const std::vector<double>& durations) const {
    if (durations.empty()) {
        return 0.0;
    }
    
    double sum = 0.0;
    for (double duration : durations) {
        sum += duration;
    }
    
    return sum / durations.size();
}

} // namespace uevrLCX
