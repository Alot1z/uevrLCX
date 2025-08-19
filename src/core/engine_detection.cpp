#include "engine_detection.h"
#include <Windows.h>
#include <Psapi.h>
#include <TlHelp32.h>
#include <iostream>
#include <fstream>
#include <json/json.h>
#include <vector>
#include <string>
#include <map>
#include <spdlog/spdlog.h>
#include <filesystem>
#include <algorithm>
#include <memory>

namespace UEVR {
namespace Core {

class EngineDetector {
private:
    struct EngineSignature {
        std::string name;
        std::string version;
        std::vector<uint8_t> pattern;
        std::string mask;
        uintptr_t offset;
        std::string adapter_path;
        std::vector<std::string> executable_names;
        std::vector<std::string> module_names;
        EngineType engine_type;
        bool is_verified;
    };

    std::map<std::string, EngineSignature> signatures;
    std::string current_engine;
    std::string current_adapter;
    EngineType current_engine_type;
    bool detection_initialized;

public:
    EngineDetector() : detection_initialized(false) {
        initializeEngineSignatures();
    }

    bool detectEngine(DWORD processId) {
        try {
            if (!detection_initialized) {
                spdlog::error("[EngineDetector] Engine detection not initialized");
                return false;
            }

            HANDLE hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, processId);
            if (!hProcess) {
                spdlog::error("[EngineDetector] Failed to open process {}", processId);
                return false;
            }

            // Get process executable name
            char executableName[MAX_PATH];
            if (GetModuleFileNameExA(hProcess, nullptr, executableName, MAX_PATH)) {
                std::string exeName = std::filesystem::path(executableName).filename().string();
                spdlog::info("[EngineDetector] Analyzing process {}: {}", processId, exeName);
                
                // First try executable name matching
                if (detectEngineByExecutable(exeName)) {
                    CloseHandle(hProcess);
                    return true;
                }
            }

            // Fallback to memory signature scanning
            MODULEINFO moduleInfo;
            HMODULE hModule = nullptr;
            DWORD cbNeeded;
            
            if (EnumProcessModules(hProcess, &hModule, sizeof(hModule), &cbNeeded)) {
                if (GetModuleInformation(hProcess, hModule, &moduleInfo, sizeof(moduleInfo))) {
                    bool detected = scanMemoryForSignatures(hProcess, moduleInfo.lpBaseOfDll, moduleInfo.SizeOfImage);
                    CloseHandle(hProcess);
                    return detected;
                }
            }

            CloseHandle(hProcess);
            return false;
            
        } catch (const std::exception& e) {
            spdlog::error("[EngineDetector] Engine detection exception: {}", e.what());
            return false;
        }
    }

    std::string getCurrentEngine() const { return current_engine; }
    std::string getCurrentAdapter() const { return current_adapter; }
    EngineType getCurrentEngineType() const { return current_engine_type; }

private:
    void initializeEngineSignatures() {
        try {
            spdlog::info("[EngineDetector] Initializing engine signatures...");
            
            // RE Engine signatures (Resident Evil 7, Resident Evil 8)
            addEngineSignature({
                "RE Engine",
                "1.0",
                {0x52, 0x45, 0x20, 0x45, 0x6E, 0x67, 0x69, 0x6E, 0x65}, // "RE Engine"
                "xxxxxxxxx",
                0x1000,
                "adapters/re-engine/re_engine_adapter.dll",
                {"re7.exe", "re8.exe", "re7-Win64-Shipping.exe", "re8-Win64-Shipping.exe"},
                {"reengine.dll", "reengine64.dll"},
                EngineType::RE_ENGINE,
                true
            });

            // REDengine 4 signatures (Cyberpunk 2077)
            addEngineSignature({
                "REDengine 4",
                "4.0",
                {0x52, 0x45, 0x44, 0x65, 0x6E, 0x67, 0x69, 0x6E, 0x65, 0x34}, // "REDengine4"
                "xxxxxxxxxx",
                0x1000,
                "adapters/redengine4/redengine4_adapter.dll",
                {"Cyberpunk2077.exe", "cyberpunk2077.exe"},
                {"redengine4.dll", "redengine4_64.dll"},
                EngineType::REDENGINE_4,
                true
            });

            // MT Framework signatures (Monster Hunter World)
            addEngineSignature({
                "MT Framework",
                "2.0",
                {0x4D, 0x54, 0x20, 0x46, 0x72, 0x61, 0x6D, 0x65, 0x77, 0x6F, 0x72, 0x6B}, // "MT Framework"
                "xxxxxxxxxxxx",
                0x1000,
                "adapters/mt-framework/mt_framework_adapter.dll",
                {"MonsterHunterWorld.exe", "mhw.exe", "MonsterHunterWorld-Win64-Shipping.exe"},
                {"mtframework.dll", "mtframework64.dll"},
                EngineType::MT_FRAMEWORK,
                true
            });

            // Unreal Engine 4 signatures
            addEngineSignature({
                "Unreal Engine 4",
                "4.27",
                {0x55, 0x6E, 0x72, 0x65, 0x61, 0x6C, 0x45, 0x6E, 0x67, 0x69, 0x6E, 0x65, 0x34}, // "UnrealEngine4"
                "xxxxxxxxxxxxx",
                0x1000,
                "adapters/unreal-engine4/ue4_adapter.dll",
                {"*.exe"}, // Generic pattern
                {"UE4*.dll", "UnrealEngine4*.dll"},
                EngineType::UNREAL_ENGINE_4,
                true
            });

            // Unreal Engine 5 signatures
            addEngineSignature({
                "Unreal Engine 5",
                "5.0",
                {0x55, 0x6E, 0x72, 0x65, 0x61, 0x6C, 0x45, 0x6E, 0x67, 0x69, 0x6E, 0x65, 0x35}, // "UnrealEngine5"
                "xxxxxxxxxxxxx",
                0x1000,
                "adapters/unreal-engine5/ue5_adapter.dll",
                {"*.exe"}, // Generic pattern
                {"UE5*.dll", "UnrealEngine5*.dll"},
                EngineType::UNREAL_ENGINE_5,
                true
            });

            // Unity Engine signatures
            addEngineSignature({
                "Unity Engine",
                "2022.3",
                {0x55, 0x6E, 0x69, 0x74, 0x79, 0x45, 0x6E, 0x67, 0x69, 0x6E, 0x65}, // "UnityEngine"
                "xxxxxxxxxxx",
                0x1000,
                "adapters/unity-engine/unity_adapter.dll",
                {"*.exe"}, // Generic pattern
                {"UnityEngine*.dll", "Unity*.dll"},
                EngineType::UNITY_ENGINE,
                true
            });

            // CryEngine signatures
            addEngineSignature({
                "CryEngine",
                "5.7",
                {0x43, 0x72, 0x79, 0x45, 0x6E, 0x67, 0x69, 0x6E, 0x65}, // "CryEngine"
                "xxxxxxxxx",
                0x1000,
                "adapters/cryengine/cryengine_adapter.dll",
                {"*.exe"}, // Generic pattern
                {"CryEngine*.dll", "Cry*.dll"},
                EngineType::CRYENGINE,
                true
            });

            // id Tech 6 signatures (DOOM 2016)
            addEngineSignature({
                "id Tech 6",
                "6.0",
                {0x69, 0x64, 0x54, 0x65, 0x63, 0x68, 0x36}, // "idTech6"
                "xxxxxxx",
                0x1000,
                "adapters/id-tech6/idtech6_adapter.dll",
                {"DOOM.exe", "doom.exe"},
                {"idTech6*.dll", "DOOM*.dll"},
                EngineType::IDTECH6,
                true
            });

            // id Tech 7 signatures (DOOM Eternal)
            addEngineSignature({
                "id Tech 7",
                "7.0",
                {0x69, 0x64, 0x54, 0x65, 0x63, 0x68, 0x37}, // "idTech7"
                "xxxxxxx",
                0x1000,
                "adapters/id-tech7/idtech7_adapter.dll",
                {"DOOMEternal.exe", "doometernal.exe"},
                {"idTech7*.dll", "DOOMEternal*.dll"},
                EngineType::IDTECH7,
                true
            });

            // Source Engine signatures
            addEngineSignature({
                "Source Engine",
                "2.0",
                {0x53, 0x6F, 0x75, 0x72, 0x63, 0x65, 0x45, 0x6E, 0x67, 0x69, 0x6E, 0x65}, // "SourceEngine"
                "xxxxxxxxxxxx",
                0x1000,
                "adapters/source-engine/source_adapter.dll",
                {"*.exe"}, // Generic pattern
                {"SourceEngine*.dll", "Source*.dll"},
                EngineType::SOURCE_ENGINE,
                true
            });

            // Source 2 signatures
            addEngineSignature({
                "Source 2",
                "2.0",
                {0x53, 0x6F, 0x75, 0x72, 0x63, 0x65, 0x32}, // "Source2"
                "xxxxxxx",
                0x1000,
                "adapters/source2/source2_adapter.dll",
                {"*.exe"}, // Generic pattern
                {"Source2*.dll", "SourceEngine2*.dll"},
                EngineType::SOURCE2,
                true
            });

            detection_initialized = true;
            spdlog::info("[EngineDetector] Engine signatures initialized: {} engines", signatures.size());
            
        } catch (const std::exception& e) {
            spdlog::error("[EngineDetector] Failed to initialize engine signatures: {}", e.what());
        }
    }

    void addEngineSignature(const EngineSignature& sig) {
        signatures[sig.name] = sig;
        spdlog::debug("[EngineDetector] Added signature for {} (type: {})", sig.name, static_cast<int>(sig.engine_type));
    }

    bool detectEngineByExecutable(const std::string& executableName) {
        try {
            for (const auto& [name, sig] : signatures) {
                for (const auto& exePattern : sig.executable_names) {
                    if (exePattern == "*.exe" || executableName == exePattern) {
                        if (exePattern == "*.exe" || executableName == exePattern) {
                            current_engine = name;
                            current_adapter = sig.adapter_path;
                            current_engine_type = sig.engine_type;
                            
                            spdlog::info("[EngineDetector] Detected engine: {} (version: {}) via executable: {}", 
                                        name, sig.version, executableName);
                            spdlog::info("[EngineDetector] Loading adapter: {}", sig.adapter_path);
                            return true;
                        }
                    }
                }
            }
            return false;
            
        } catch (const std::exception& e) {
            spdlog::error("[EngineDetector] Executable detection exception: {}", e.what());
            return false;
        }
    }

    bool scanMemoryForSignatures(HANDLE hProcess, LPVOID baseAddress, SIZE_T size) {
        try {
            std::vector<uint8_t> buffer(size);
            SIZE_T bytesRead;

            if (!ReadProcessMemory(hProcess, baseAddress, buffer.data(), size, &bytesRead)) {
                spdlog::warn("[EngineDetector] Failed to read process memory, size: {}", size);
                return false;
            }

            for (const auto& [name, sig] : signatures) {
                if (findPattern(buffer, sig.pattern, sig.mask, sig.offset)) {
                    current_engine = name;
                    current_adapter = sig.adapter_path;
                    current_engine_type = sig.engine_type;
                    
                    spdlog::info("[EngineDetector] Detected engine: {} (version: {}) via memory signature", 
                                name, sig.version);
                    spdlog::info("[EngineDetector] Loading adapter: {}", sig.adapter_path);
                    return true;
                }
            }

            return false;
            
        } catch (const std::exception& e) {
            spdlog::error("[EngineDetector] Memory signature scanning exception: {}", e.what());
            return false;
        }
    }

    bool findPattern(const std::vector<uint8_t>& buffer, const std::vector<uint8_t>& pattern, 
                    const std::string& mask, uintptr_t offset) {
        try {
            if (pattern.size() != mask.length()) {
                spdlog::warn("[EngineDetector] Pattern and mask size mismatch: {} vs {}", pattern.size(), mask.length());
                return false;
            }

            for (size_t i = 0; i <= buffer.size() - pattern.size(); i++) {
                bool found = true;
                for (size_t j = 0; j < pattern.size(); j++) {
                    if (mask[j] == 'x' && buffer[i + j] != pattern[j]) {
                        found = false;
                        break;
                    }
                }
                if (found) {
                    spdlog::debug("[EngineDetector] Pattern found at offset 0x{:X}", i);
                    return true;
                }
            }
            return false;
            
        } catch (const std::exception& e) {
            spdlog::error("[EngineDetector] Pattern finding exception: {}", e.what());
            return false;
        }
    }

    void loadEngineSignaturesFromFile() {
        try {
            std::ifstream file("data/engine_signatures.json");
            if (!file.is_open()) {
                spdlog::warn("[EngineDetector] No engine signatures file found, using built-in signatures");
                return;
            }

            Json::Value root;
            Json::Reader reader;
            if (!reader.parse(file, root)) {
                spdlog::error("[EngineDetector] Failed to parse engine signatures file");
                return;
            }

            for (const auto& engine : root["engines"]) {
                EngineSignature sig;
                sig.name = engine["name"].asString();
                sig.version = engine["version"].asString();
                sig.adapter_path = engine["adapter_path"].asString();
                sig.offset = engine["offset"].asUInt64();
                sig.engine_type = static_cast<EngineType>(engine["engine_type"].asInt());

                // Parse pattern
                std::string patternStr = engine["pattern"].asString();
                std::string maskStr = engine["mask"].asString();
                
                for (size_t i = 0; i < patternStr.length(); i += 2) {
                    if (i + 1 < patternStr.length()) {
                        std::string byteStr = patternStr.substr(i, 2);
                        uint8_t byte = std::stoi(byteStr, nullptr, 16);
                        sig.pattern.push_back(byte);
                    }
                }
                sig.mask = maskStr;

                // Parse executable names
                if (engine.isMember("executable_names")) {
                    for (const auto& exeName : engine["executable_names"]) {
                        sig.executable_names.push_back(exeName.asString());
                    }
                }

                // Parse module names
                if (engine.isMember("module_names")) {
                    for (const auto& moduleName : engine["module_names"]) {
                        sig.module_names.push_back(moduleName.asString());
                    }
                }

                sig.is_verified = engine.get("is_verified", false).asBool();
                signatures[sig.name] = sig;
            }
            
            spdlog::info("[EngineDetector] Loaded {} engines from signatures file", signatures.size());
            
        } catch (const std::exception& e) {
            spdlog::error("[EngineDetector] Failed to load engine signatures from file: {}", e.what());
        }
    }
};

// Global engine detector instance
static std::unique_ptr<EngineDetector> g_engineDetector;

bool detectEngineForProcess(DWORD processId) {
    try {
        if (!g_engineDetector) {
            g_engineDetector = std::make_unique<EngineDetector>();
        }
        return g_engineDetector->detectEngine(processId);
        
    } catch (const std::exception& e) {
        spdlog::error("[EngineDetector] Engine detection failed: {}", e.what());
        return false;
    }
}

std::string getDetectedEngine() {
    try {
        return g_engineDetector ? g_engineDetector->getCurrentEngine() : "Unknown";
    } catch (const std::exception& e) {
        spdlog::error("[EngineDetector] Failed to get detected engine: {}", e.what());
        return "Unknown";
    }
}

std::string getDetectedAdapter() {
    try {
        return g_engineDetector ? g_engineDetector->getCurrentAdapter() : "";
    } catch (const std::exception& e) {
        spdlog::error("[EngineDetector] Failed to get detected adapter: {}", e.what());
        return "";
    }
}

EngineType getDetectedEngineType() {
    try {
        return g_engineDetector ? g_engineDetector->getCurrentEngineType() : EngineType::UNKNOWN;
    } catch (const std::exception& e) {
        spdlog::error("[EngineDetector] Failed to get detected engine type: {}", e.what());
        return EngineType::UNKNOWN;
    }
}

// Additional utility functions
bool isEngineSupported(EngineType engineType) {
    try {
        switch (engineType) {
            case EngineType::RE_ENGINE:
            case EngineType::REDENGINE_4:
            case EngineType::MT_FRAMEWORK:
            case EngineType::UNREAL_ENGINE_4:
            case EngineType::UNREAL_ENGINE_5:
            case EngineType::UNITY_ENGINE:
            case EngineType::CRYENGINE:
            case EngineType::IDTECH6:
            case EngineType::IDTECH7:
            case EngineType::SOURCE_ENGINE:
            case EngineType::SOURCE2:
                return true;
            default:
                return false;
        }
    } catch (const std::exception& e) {
        spdlog::error("[EngineDetector] Engine support check failed: {}", e.what());
        return false;
    }
}

std::string getEngineTypeString(EngineType engineType) {
    try {
        switch (engineType) {
            case EngineType::RE_ENGINE: return "RE Engine";
            case EngineType::REDENGINE_4: return "REDengine 4";
            case EngineType::MT_FRAMEWORK: return "MT Framework";
            case EngineType::UNREAL_ENGINE_4: return "Unreal Engine 4";
            case EngineType::UNREAL_ENGINE_5: return "Unreal Engine 5";
            case EngineType::UNITY_ENGINE: return "Unity Engine";
            case EngineType::CRYENGINE: return "CryEngine";
            case EngineType::IDTECH6: return "id Tech 6";
            case EngineType::IDTECH7: return "id Tech 7";
            case EngineType::SOURCE_ENGINE: return "Source Engine";
            case EngineType::SOURCE2: return "Source 2";
            case EngineType::CUSTOM_ENGINE: return "Custom Engine";
            default: return "Unknown Engine";
        }
    } catch (const std::exception& e) {
        spdlog::error("[EngineDetector] Engine type string conversion failed: {}", e.what());
        return "Unknown Engine";
    }
}

// Initialize global engine detector
void initializeGlobalEngineDetector() {
    try {
        if (!g_engineDetector) {
            g_engineDetector = std::make_unique<EngineDetector>();
            spdlog::info("[EngineDetector] Global engine detector initialized");
        }
    } catch (const std::exception& e) {
        spdlog::error("[EngineDetector] Failed to initialize global detector: {}", e.what());
    }
}

// Cleanup global engine detector
void cleanupGlobalEngineDetector() {
    try {
        g_engineDetector.reset();
        spdlog::info("[EngineDetector] Global engine detector cleaned up");
    } catch (const std::exception& e) {
        spdlog::error("[EngineDetector] Failed to cleanup global detector: {}", e.what());
    }
}

} // namespace Core
} // namespace UEVR
