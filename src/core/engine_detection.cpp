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
    };

    std::map<std::string, EngineSignature> signatures;
    std::string current_engine;
    std::string current_adapter;

public:
    EngineDetector() {
        loadEngineSignatures();
    }

    bool detectEngine(DWORD processId) {
        HANDLE hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, processId);
        if (!hProcess) {
            std::cerr << "Failed to open process " << processId << std::endl;
            return false;
        }

        MODULEINFO moduleInfo;
        HMODULE hModule = nullptr;
        DWORD cbNeeded;
        
        if (EnumProcessModules(hProcess, &hModule, sizeof(hModule), &cbNeeded)) {
            if (GetModuleInformation(hProcess, hModule, &moduleInfo, sizeof(moduleInfo))) {
                return scanMemoryForSignatures(hProcess, moduleInfo.lpBaseOfDll, moduleInfo.SizeOfImage);
            }
        }

        CloseHandle(hProcess);
        return false;
    }

    std::string getCurrentEngine() const { return current_engine; }
    std::string getCurrentAdapter() const { return current_adapter; }

private:
    void loadEngineSignatures() {
        std::ifstream file("data/engine_signatures.json");
        if (!file.is_open()) {
            std::cerr << "Failed to open engine signatures file" << std::endl;
            return;
        }

        Json::Value root;
        Json::Reader reader;
        if (!reader.parse(file, root)) {
            std::cerr << "Failed to parse engine signatures" << std::endl;
            return;
        }

        for (const auto& engine : root["engines"]) {
            EngineSignature sig;
            sig.name = engine["name"].asString();
            sig.version = engine["version"].asString();
            sig.adapter_path = engine["adapter_path"].asString();
            sig.offset = engine["offset"].asUInt64();

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

            signatures[sig.name] = sig;
        }
    }

    bool scanMemoryForSignatures(HANDLE hProcess, LPVOID baseAddress, SIZE_T size) {
        std::vector<uint8_t> buffer(size);
        SIZE_T bytesRead;

        if (!ReadProcessMemory(hProcess, baseAddress, buffer.data(), size, &bytesRead)) {
            return false;
        }

        for (const auto& [name, sig] : signatures) {
            if (findPattern(buffer, sig.pattern, sig.mask, sig.offset)) {
                current_engine = name;
                current_adapter = sig.adapter_path;
                std::cout << "Detected engine: " << name << " (version: " << sig.version << ")" << std::endl;
                std::cout << "Loading adapter: " << sig.adapter_path << std::endl;
                return true;
            }
        }

        return false;
    }

    bool findPattern(const std::vector<uint8_t>& buffer, const std::vector<uint8_t>& pattern, 
                    const std::string& mask, uintptr_t offset) {
        for (size_t i = 0; i <= buffer.size() - pattern.size(); i++) {
            bool found = true;
            for (size_t j = 0; j < pattern.size(); j++) {
                if (mask[j] == 'x' && buffer[i + j] != pattern[j]) {
                    found = false;
                    break;
                }
            }
            if (found) {
                return true;
            }
        }
        return false;
    }
};

// Global engine detector instance
static EngineDetector g_engineDetector;

bool detectEngineForProcess(DWORD processId) {
    return g_engineDetector.detectEngine(processId);
}

std::string getDetectedEngine() {
    return g_engineDetector.getCurrentEngine();
}

std::string getDetectedAdapter() {
    return g_engineDetector.getCurrentAdapter();
}

} // namespace Core
} // namespace UEVR
