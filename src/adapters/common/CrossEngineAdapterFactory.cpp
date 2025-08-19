#include "CrossEngineAdapterFactory.hpp"
#include <iostream>
#include <algorithm>
#include <Windows.h>
#include <TlHelp32.h>
#include <Psapi.h>
#include <chrono>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace uevr {

CrossEngineAdapterFactory::CrossEngineAdapterFactory() {
    initializeDefaultAdapters();
}

void CrossEngineAdapterFactory::registerAdapter(const std::string& engineName, AdapterCreator creator) {
    m_adapters[engineName] = creator;
    std::cout << "[Factory] Registered adapter: " << engineName << std::endl;
}

std::unique_ptr<BaseCrossEngineAdapter> CrossEngineAdapterFactory::createAdapter(const std::string& engineName) {
    auto it = m_adapters.find(engineName);
    if (it != m_adapters.end()) {
        try {
            auto adapter = it->second();
            std::cout << "[Factory] Created adapter: " << engineName << std::endl;
            return adapter;
        } catch (const std::exception& e) {
            std::cerr << "[Factory] Failed to create adapter " << engineName << ": " << e.what() << std::endl;
            return nullptr;
        }
    }
    
    std::cerr << "[Factory] Unknown engine: " << engineName << std::endl;
    return nullptr;
}

std::unique_ptr<BaseCrossEngineAdapter> CrossEngineAdapterFactory::autoDetectAndCreate() {
    std::cout << "[Factory] Auto-detecting engine..." << std::endl;
    
    // Try to detect RE Engine first (Resident Evil 7)
    if (detectREEngine()) {
        std::cout << "[Factory] Detected RE Engine" << std::endl;
        return createAdapter("RE Engine");
    }
    
    // Try to detect REDengine 4 (Cyberpunk 2077)
    if (detectREDengine4()) {
        std::cout << "[Factory] Detected REDengine 4" << std::endl;
        return createAdapter("REDengine 4");
    }
    
    // Try to detect MT Framework (Monster Hunter World)
    if (detectMTFramework()) {
        std::cout << "[Factory] Detected MT Framework" << std::endl;
        return createAdapter("MT Framework");
    }
    
    std::cout << "[Factory] No supported engine detected" << std::endl;
    return nullptr;
}

std::vector<std::string> CrossEngineAdapterFactory::getSupportedEngines() const {
    std::vector<std::string> engines;
    engines.reserve(m_adapters.size());
    
    for (const auto& [engineName, _] : m_adapters) {
        engines.push_back(engineName);
    }
    
    return engines;
}

bool CrossEngineAdapterFactory::isEngineSupported(const std::string& engineName) const {
    return m_adapters.find(engineName) != m_adapters.end();
}

CrossEngineAdapterFactory::AdapterInfo CrossEngineAdapterFactory::getAdapterInfo(const std::string& engineName) const {
    auto it = m_adapterInfo.find(engineName);
    if (it != m_adapterInfo.end()) {
        return it->second;
    }
    
    // Return default info if not found
    return AdapterInfo{
        engineName,
        "Unknown engine",
        "Unknown version",
        {},
        false
    };
}

bool CrossEngineAdapterFactory::detectREEngine() const {
    // Check for Resident Evil 7 process
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        return false;
    }
    
    PROCESSENTRY32W pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32W);
    
    if (Process32FirstW(snapshot, &pe32)) {
        do {
            std::wstring processName = pe32.szExeFile;
            std::string processNameStr(processName.begin(), processName.end());
            
            // Convert to lowercase for comparison
            std::transform(processNameStr.begin(), processNameStr.end(), 
                          processNameStr.begin(), ::tolower);
            
            if (processNameStr.find("re7") != std::string::npos ||
                processNameStr.find("resident evil 7") != std::string::npos) {
                CloseHandle(snapshot);
                return true;
            }
        } while (Process32NextW(snapshot, &pe32));
    }
    
    CloseHandle(snapshot);
    return false;
}

bool CrossEngineAdapterFactory::detectREDengine4() const {
    // Check for Cyberpunk 2077 process
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        return false;
    }
    
    PROCESSENTRY32W pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32W);
    
    if (Process32FirstW(snapshot, &pe32)) {
        do {
            std::wstring processName = pe32.szExeFile;
            std::string processNameStr(processName.begin(), processName.end());
            
            // Convert to lowercase for comparison
            std::transform(processNameStr.begin(), processNameStr.end(), 
                          processNameStr.begin(), ::tolower);
            
            if (processNameStr.find("cyberpunk2077") != std::string::npos ||
                processNameStr.find("cp2077") != std::string::npos) {
                CloseHandle(snapshot);
                return true;
            }
        } while (Process32NextW(snapshot, &pe32));
    }
    
    CloseHandle(snapshot);
    return false;
}

bool CrossEngineAdapterFactory::detectMTFramework() const {
    // Check for Monster Hunter World process
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        return false;
    }
    
    PROCESSENTRY32W pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32W);
    
    if (Process32FirstW(snapshot, &pe32)) {
        do {
            std::wstring processName = pe32.szExeFile;
            std::string processNameStr(processName.begin(), processName.end());
            
            // Convert to lowercase for comparison
            std::transform(processNameStr.begin(), processNameStr.end(), 
                          processNameStr.begin(), ::tolower);
            
            if (processNameStr.find("monster hunter world") != std::string::npos ||
                processNameStr.find("mhw") != std::string::npos) {
                CloseHandle(snapshot);
                return true;
            }
        } while (Process32NextW(snapshot, &pe32));
    }
    
    CloseHandle(snapshot);
    return false;
}

void CrossEngineAdapterFactory::initializeDefaultAdapters() {
    // Initialize adapter information
    m_adapterInfo["RE Engine"] = {
        "RE Engine",
        "Capcom RE Engine VR Adapter",
        "1.0.0",
        {"Resident Evil 7", "Resident Evil 2", "Resident Evil 3"},
        true
    };
    
    m_adapterInfo["REDengine 4"] = {
        "REDengine 4",
        "CD Projekt RED Engine VR Adapter",
        "1.0.0",
        {"Cyberpunk 2077", "The Witcher 3"},
        true
    };
    
    m_adapterInfo["MT Framework"] = {
        "MT Framework",
        "Capcom MT Framework VR Adapter",
        "1.0.0",
        {"Monster Hunter World", "Monster Hunter Rise"},
        true
    };
    
    std::cout << "[Factory] Initialized default adapter information" << std::endl;
}

} // namespace uevr
