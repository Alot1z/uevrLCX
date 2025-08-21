#pragma once

#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include <mutex>
#include <memory>

namespace uevrLCX {

enum class EventType {
    SYSTEM_INIT,
    SYSTEM_SHUTDOWN,
    VR_CONNECTED,
    VR_DISCONNECTED,
    GAME_LOADED,
    GAME_UNLOADED,
    PERFORMANCE_UPDATE,
    ERROR_OCCURRED,
    WARNING_OCCURRED,
    INFO_MESSAGE,
    DEBUG_MESSAGE
};

struct EventData {
    EventType type;
    std::string source;
    std::string message;
    void* userData;
    uint64_t timestamp;
    
    EventData() : type(EventType::INFO_MESSAGE), userData(nullptr), timestamp(0) {}
};

using EventCallback = std::function<void(const EventData&)>;

class EventSystem {
public:
    EventSystem();
    ~EventSystem();

    // Core functionality
    bool initialize();
    void shutdown();
    
    // Event registration
    void registerCallback(EventType type, EventCallback callback);
    void unregisterCallback(EventType type, size_t callbackId);
    
    // Event emission
    void emitEvent(EventType type, const std::string& source, const std::string& message, void* userData = nullptr);
    void emitEvent(const EventData& event);
    
    // Event processing
    void processEvents();
    void clearEvents();
    
    // Event querying
    std::vector<EventData> getRecentEvents(size_t count = 100) const;
    std::vector<EventData> getEventsByType(EventType type, size_t count = 100) const;
    std::vector<EventData> getEventsBySource(const std::string& source, size_t count = 100) const;
    
    // Statistics
    size_t getEventCount() const;
    size_t getEventCountByType(EventType type) const;
    size_t getCallbackCount() const;

private:
    struct CallbackInfo {
        size_t id;
        EventCallback callback;
        bool active;
    };

    // Event storage
    std::vector<EventData> eventQueue;
    std::vector<EventData> eventHistory;
    size_t maxHistorySize;
    
    // Callback storage
    std::unordered_map<EventType, std::vector<CallbackInfo>> callbacks;
    size_t nextCallbackId;
    
    // Thread safety
    mutable std::mutex eventMutex;
    mutable std::mutex callbackMutex;
    
    // Internal methods
    void addToHistory(const EventData& event);
    void notifyCallbacks(const EventData& event);
    std::string getEventTypeString(EventType type) const;
};

} // namespace uevrLCX
