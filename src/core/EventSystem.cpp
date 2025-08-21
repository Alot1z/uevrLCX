#include "EventSystem.h"
#include <chrono>
#include <algorithm>

namespace uevrLCX {

EventSystem::EventSystem()
    : maxHistorySize(10000)
    , nextCallbackId(1) {
}

EventSystem::~EventSystem() {
    shutdown();
}

bool EventSystem::initialize() {
    try {
        eventQueue.clear();
        eventHistory.clear();
        callbacks.clear();
        nextCallbackId = 1;
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

void EventSystem::shutdown() {
    std::lock_guard<std::mutex> eventLock(eventMutex);
    std::lock_guard<std::mutex> callbackLock(callbackMutex);
    
    eventQueue.clear();
    eventHistory.clear();
    callbacks.clear();
}

void EventSystem::registerCallback(EventType type, EventCallback callback) {
    std::lock_guard<std::mutex> lock(callbackMutex);
    
    CallbackInfo info;
    info.id = nextCallbackId++;
    info.callback = std::move(callback);
    info.active = true;
    
    callbacks[type].push_back(info);
}

void EventSystem::unregisterCallback(EventType type, size_t callbackId) {
    std::lock_guard<std::mutex> lock(callbackMutex);
    
    auto it = callbacks.find(type);
    if (it != callbacks.end()) {
        auto& callbackList = it->second;
        for (auto& callback : callbackList) {
            if (callback.id == callbackId) {
                callback.active = false;
                break;
            }
        }
    }
}

void EventSystem::emitEvent(EventType type, const std::string& source, const std::string& message, void* userData) {
    EventData event;
    event.type = type;
    event.source = source;
    event.message = message;
    event.userData = userData;
    event.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    
    emitEvent(event);
}

void EventSystem::emitEvent(const EventData& event) {
    std::lock_guard<std::mutex> lock(eventMutex);
    
    eventQueue.push_back(event);
    addToHistory(event);
    notifyCallbacks(event);
}

void EventSystem::processEvents() {
    std::lock_guard<std::mutex> lock(eventMutex);
    
    // Process queued events
    for (const auto& event : eventQueue) {
        // Event processing logic here
    }
    
    eventQueue.clear();
}

void EventSystem::clearEvents() {
    std::lock_guard<std::mutex> lock(eventMutex);
    eventQueue.clear();
    eventHistory.clear();
}

std::vector<EventData> EventSystem::getRecentEvents(size_t count) const {
    std::lock_guard<std::mutex> lock(eventMutex);
    
    size_t actualCount = std::min(count, eventHistory.size());
    std::vector<EventData> result;
    result.reserve(actualCount);
    
    auto start = eventHistory.end() - actualCount;
    result.assign(start, eventHistory.end());
    
    return result;
}

std::vector<EventData> EventSystem::getEventsByType(EventType type, size_t count) const {
    std::lock_guard<std::mutex> lock(eventMutex);
    
    std::vector<EventData> result;
    result.reserve(count);
    
    for (auto it = eventHistory.rbegin(); it != eventHistory.rend() && result.size() < count; ++it) {
        if (it->type == type) {
            result.push_back(*it);
        }
    }
    
    return result;
}

std::vector<EventData> EventSystem::getEventsBySource(const std::string& source, size_t count) const {
    std::lock_guard<std::mutex> lock(eventMutex);
    
    std::vector<EventData> result;
    result.reserve(count);
    
    for (auto it = eventHistory.rbegin(); it != eventHistory.rend() && result.size() < count; ++it) {
        if (it->source == source) {
            result.push_back(*it);
        }
    }
    
    return result;
}

size_t EventSystem::getEventCount() const {
    std::lock_guard<std::mutex> lock(eventMutex);
    return eventHistory.size();
}

size_t EventSystem::getEventCountByType(EventType type) const {
    std::lock_guard<std::mutex> lock(eventMutex);
    
    return std::count_if(eventHistory.begin(), eventHistory.end(),
        [type](const EventData& event) { return event.type == type; });
}

size_t EventSystem::getCallbackCount() const {
    std::lock_guard<std::mutex> lock(callbackMutex);
    
    size_t count = 0;
    for (const auto& pair : callbacks) {
        count += pair.second.size();
    }
    return count;
}

void EventSystem::addToHistory(const EventData& event) {
    eventHistory.push_back(event);
    
    if (eventHistory.size() > maxHistorySize) {
        eventHistory.erase(eventHistory.begin());
    }
}

void EventSystem::notifyCallbacks(const EventData& event) {
    std::lock_guard<std::mutex> lock(callbackMutex);
    
    auto it = callbacks.find(event.type);
    if (it != callbacks.end()) {
        for (const auto& callbackInfo : it->second) {
            if (callbackInfo.active) {
                try {
                    callbackInfo.callback(event);
                } catch (...) {
                    // Ignore callback errors
                }
            }
        }
    }
}

std::string EventSystem::getEventTypeString(EventType type) const {
    switch (type) {
        case EventType::SYSTEM_INIT: return "SYSTEM_INIT";
        case EventType::SYSTEM_SHUTDOWN: return "SYSTEM_SHUTDOWN";
        case EventType::VR_CONNECTED: return "VR_CONNECTED";
        case EventType::VR_DISCONNECTED: return "VR_DISCONNECTED";
        case EventType::GAME_LOADED: return "GAME_LOADED";
        case EventType::GAME_UNLOADED: return "GAME_UNLOADED";
        case EventType::PERFORMANCE_UPDATE: return "PERFORMANCE_UPDATE";
        case EventType::ERROR_OCCURRED: return "ERROR_OCCURRED";
        case EventType::WARNING_OCCURRED: return "WARNING_OCCURRED";
        case EventType::INFO_MESSAGE: return "INFO_MESSAGE";
        case EventType::DEBUG_MESSAGE: return "DEBUG_MESSAGE";
        default: return "UNKNOWN";
    }
}

} // namespace uevrLCX
