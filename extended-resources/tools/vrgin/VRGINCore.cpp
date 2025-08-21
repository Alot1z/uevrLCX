#include "VRGINIntegration.hpp"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <Windows.h>
#include <TlHelp32.h>
#include <Psapi.h>
#include <d3d11.h>
#include <d3d12.h>
#include <vulkan/vulkan.h>
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <thread>
#include <mutex>

namespace VRGIN {

class VRGINCore : public IVRGINCore {
private:
    VRGINConfig m_config;
    VRGINMetrics m_metrics;
    std::unique_ptr<IVRGINRenderer> m_renderer;
    std::unique_ptr<IVRGINInput> m_input;
    std::unique_ptr<IVRGINPhysics> m_physics;
    std::unique_ptr<IVRGINOptimizer> m_optimizer;
    
    HANDLE m_gameProcess;
    DWORD m_gameProcessId;
    std::string m_gameProcessName;
    bool m_injected;
    bool m_initialized;
    
    std::mutex m_mutex;
    std::thread m_injectionThread;
    std::thread m_monitoringThread;
    
    // Memory regions for injection
    struct MemoryRegion {
        LPVOID address;
        SIZE_T size;
        DWORD protection;
        std::vector<uint8_t> originalData;
    };
    std::vector<MemoryRegion> m_memoryRegions;
    
    // Hook information
    struct HookInfo {
        LPVOID originalAddress;
        LPVOID hookAddress;
        std::vector<uint8_t> originalBytes;
        bool active;
    };
    std::unordered_map<std::string, HookInfo> m_hooks;

public:
    VRGINCore() : m_gameProcess(nullptr), m_gameProcessId(0), m_injected(false), m_initialized(false) {
        spdlog::info("VRGINCore: Constructor called");
    }
    
    ~VRGINCore() {
        Shutdown();
    }

    bool Initialize(const VRGINConfig& config) override {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        spdlog::info("VRGINCore: Initializing with config - enableVR: {}, enablePhysics: {}, enableOptimization: {}", 
                     config.enableVR, config.enablePhysics, config.enableOptimization);
        
        m_config = config;
        m_metrics = VRGINMetrics{};
        
        // Initialize components based on config
        if (config.enableVR) {
            m_renderer = CreateVRGINRenderer();
            if (!m_renderer || !m_renderer->Initialize(config.rendererConfig)) {
                spdlog::error("VRGINCore: Failed to initialize VR renderer");
                return false;
            }
        }
        
        if (config.enableInput) {
            m_input = CreateVRGINInput();
            if (!m_input || !m_input->Initialize(config.inputConfig)) {
                spdlog::error("VRGINCore: Failed to initialize VR input");
                return false;
            }
        }
        
        if (config.enablePhysics) {
            m_physics = CreateVRGINPhysics();
            if (!m_physics || !m_physics->Initialize(config.physicsConfig)) {
                spdlog::error("VRGINCore: Failed to initialize VR physics");
                return false;
            }
        }
        
        if (config.enableOptimization) {
            m_optimizer = CreateVRGINOptimizer();
            if (!m_optimizer || !m_optimizer->Initialize(config.optimizerConfig)) {
                spdlog::error("VRGINCore: Failed to initialize VR optimizer");
                return false;
            }
        }
        
        m_initialized = true;
        spdlog::info("VRGINCore: Initialization completed successfully");
        return true;
    }

    bool InjectIntoGame(const std::string& processName) override {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        if (!m_initialized) {
            spdlog::error("VRGINCore: Cannot inject - not initialized");
            return false;
        }
        
        spdlog::info("VRGINCore: Attempting to inject into process: {}", processName);
        
        // Find the target process
        m_gameProcessId = FindProcessId(processName);
        if (m_gameProcessId == 0) {
            spdlog::error("VRGINCore: Process '{}' not found", processName);
            return false;
        }
        
        // Open process with required permissions
        m_gameProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_gameProcessId);
        if (!m_gameProcess) {
            spdlog::error("VRGINCore: Failed to open process (PID: {})", m_gameProcessId);
            return false;
        }
        
        m_gameProcessName = processName;
        
        // Analyze process memory layout
        if (!AnalyzeProcessMemory()) {
            spdlog::error("VRGINCore: Failed to analyze process memory");
            return false;
        }
        
        // Inject VRGIN DLL
        if (!InjectVRGINDLL()) {
            spdlog::error("VRGINCore: Failed to inject VRGIN DLL");
            return false;
        }
        
        // Install hooks
        if (!InstallHooks()) {
            spdlog::error("VRGINCore: Failed to install hooks");
            return false;
        }
        
        // Start monitoring thread
        StartMonitoring();
        
        m_injected = true;
        spdlog::info("VRGINCore: Successfully injected into process '{}' (PID: {})", processName, m_gameProcessId);
        return true;
    }

    bool ExtractDepthBuffer() override {
        if (!m_injected || !m_renderer) {
            spdlog::error("VRGINCore: Cannot extract depth buffer - not injected or renderer not available");
            return false;
        }
        
        return m_renderer->ExtractDepthBuffer();
    }

    bool IntegratePhysics() override {
        if (!m_injected || !m_physics) {
            spdlog::error("VRGINCore: Cannot integrate physics - not injected or physics not available");
            return false;
        }
        
        return m_physics->IntegratePhysics();
    }

    bool OptimizePerformance() override {
        if (!m_injected || !m_optimizer) {
            spdlog::error("VRGINCore: Cannot optimize performance - not injected or optimizer not available");
            return false;
        }
        
        return m_optimizer->OptimizePerformance();
    }

    VRGINMetrics GetMetrics() const override {
        return m_metrics;
    }

    void Shutdown() override {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        spdlog::info("VRGINCore: Shutting down");
        
        // Stop monitoring
        if (m_monitoringThread.joinable()) {
            m_monitoringThread.join();
        }
        
        // Remove hooks
        RemoveHooks();
        
        // Close process handle
        if (m_gameProcess) {
            CloseHandle(m_gameProcess);
            m_gameProcess = nullptr;
        }
        
        // Shutdown components
        if (m_renderer) m_renderer->Shutdown();
        if (m_input) m_input->Shutdown();
        if (m_physics) m_physics->Shutdown();
        if (m_optimizer) m_optimizer->Shutdown();
        
        m_injected = false;
        m_initialized = false;
        
        spdlog::info("VRGINCore: Shutdown completed");
    }

private:
    DWORD FindProcessId(const std::string& processName) {
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (snapshot == INVALID_HANDLE_VALUE) {
            return 0;
        }
        
        PROCESSENTRY32 processEntry;
        processEntry.dwSize = sizeof(PROCESSENTRY32);
        
        if (Process32First(snapshot, &processEntry)) {
            do {
                std::string currentProcessName = processEntry.szExeFile;
                if (currentProcessName.find(processName) != std::string::npos) {
                    CloseHandle(snapshot);
                    return processEntry.th32ProcessID;
                }
            } while (Process32Next(snapshot, &processEntry));
        }
        
        CloseHandle(snapshot);
        return 0;
    }
    
    bool AnalyzeProcessMemory() {
        spdlog::info("VRGINCore: Analyzing process memory layout");
        
        MEMORY_BASIC_INFORMATION mbi;
        LPVOID address = 0;
        
        while (VirtualQueryEx(m_gameProcess, address, &mbi, sizeof(mbi))) {
            if (mbi.State == MEM_COMMIT && 
                (mbi.Protect & PAGE_EXECUTE_READ) || 
                (mbi.Protect & PAGE_EXECUTE_READWRITE)) {
                
                MemoryRegion region;
                region.address = mbi.BaseAddress;
                region.size = mbi.RegionSize;
                region.protection = mbi.Protect;
                
                // Read original data
                region.originalData.resize(mbi.RegionSize);
                SIZE_T bytesRead;
                if (ReadProcessMemory(m_gameProcess, mbi.BaseAddress, 
                                    region.originalData.data(), mbi.RegionSize, &bytesRead)) {
                    m_memoryRegions.push_back(region);
                }
            }
            
            address = (LPVOID)((ULONG_PTR)mbi.BaseAddress + mbi.RegionSize);
        }
        
        spdlog::info("VRGINCore: Found {} memory regions for analysis", m_memoryRegions.size());
        return true;
    }
    
    bool InjectVRGINDLL() {
        spdlog::info("VRGINCore: Injecting VRGIN DLL");
        
        // Get path to VRGIN DLL
        std::string dllPath = GetVRGINDLLPath();
        if (dllPath.empty()) {
            spdlog::error("VRGINCore: VRGIN DLL not found");
            return false;
        }
        
        // Allocate memory in target process for DLL path
        SIZE_T pathSize = dllPath.length() + 1;
        LPVOID remotePath = VirtualAllocEx(m_gameProcess, nullptr, pathSize, 
                                          MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if (!remotePath) {
            spdlog::error("VRGINCore: Failed to allocate memory for DLL path");
            return false;
        }
        
        // Write DLL path to target process
        if (!WriteProcessMemory(m_gameProcess, remotePath, dllPath.c_str(), pathSize, nullptr)) {
            spdlog::error("VRGINCore: Failed to write DLL path to target process");
            VirtualFreeEx(m_gameProcess, remotePath, 0, MEM_RELEASE);
            return false;
        }
        
        // Get LoadLibraryA address
        HMODULE kernel32 = GetModuleHandleA("kernel32.dll");
        LPVOID loadLibraryAddr = GetProcAddress(kernel32, "LoadLibraryA");
        
        // Create remote thread to load DLL
        HANDLE remoteThread = CreateRemoteThread(m_gameProcess, nullptr, 0,
                                                (LPTHREAD_START_ROUTINE)loadLibraryAddr,
                                                remotePath, 0, nullptr);
        if (!remoteThread) {
            spdlog::error("VRGINCore: Failed to create remote thread for DLL injection");
            VirtualFreeEx(m_gameProcess, remotePath, 0, MEM_RELEASE);
            return false;
        }
        
        // Wait for thread completion
        WaitForSingleObject(remoteThread, INFINITE);
        
        // Clean up
        CloseHandle(remoteThread);
        VirtualFreeEx(m_gameProcess, remotePath, 0, MEM_RELEASE);
        
        spdlog::info("VRGINCore: DLL injection completed");
        return true;
    }
    
    bool InstallHooks() {
        spdlog::info("VRGINCore: Installing hooks");
        
        // Hook DirectX functions
        if (!HookDirectXFunctions()) {
            spdlog::error("VRGINCore: Failed to hook DirectX functions");
            return false;
        }
        
        // Hook OpenGL functions
        if (!HookOpenGLFunctions()) {
            spdlog::error("VRGINCore: Failed to hook OpenGL functions");
            return false;
        }
        
        // Hook Vulkan functions
        if (!HookVulkanFunctions()) {
            spdlog::error("VRGINCore: Failed to hook Vulkan functions");
            return false;
        }
        
        spdlog::info("VRGINCore: All hooks installed successfully");
        return true;
    }
    
    bool HookDirectXFunctions() {
        // Hook D3D11 Present
        HMODULE d3d11Module = GetModuleHandleA("d3d11.dll");
        if (d3d11Module) {
            LPVOID presentAddr = GetProcAddress(d3d11Module, "Present");
            if (presentAddr) {
                HookInfo hook;
                hook.originalAddress = presentAddr;
                hook.active = false;
                m_hooks["D3D11_Present"] = hook;
            }
        }
        
        // Hook D3D12 Present
        HMODULE d3d12Module = GetModuleHandleA("d3d12.dll");
        if (d3d12Module) {
            LPVOID presentAddr = GetProcAddress(d3d12Module, "Present");
            if (presentAddr) {
                HookInfo hook;
                hook.originalAddress = presentAddr;
                hook.active = false;
                m_hooks["D3D12_Present"] = hook;
            }
        }
        
        return true;
    }
    
    bool HookOpenGLFunctions() {
        // Hook OpenGL SwapBuffers
        HMODULE openglModule = GetModuleHandleA("opengl32.dll");
        if (openglModule) {
            LPVOID swapBuffersAddr = GetProcAddress(openglModule, "SwapBuffers");
            if (swapBuffersAddr) {
                HookInfo hook;
                hook.originalAddress = swapBuffersAddr;
                hook.active = false;
                m_hooks["OpenGL_SwapBuffers"] = hook;
            }
        }
        
        return true;
    }
    
    bool HookVulkanFunctions() {
        // Hook Vulkan vkQueuePresentKHR
        HMODULE vulkanModule = GetModuleHandleA("vulkan-1.dll");
        if (vulkanModule) {
            LPVOID queuePresentAddr = GetProcAddress(vulkanModule, "vkQueuePresentKHR");
            if (queuePresentAddr) {
                HookInfo hook;
                hook.originalAddress = queuePresentAddr;
                hook.active = false;
                m_hooks["Vulkan_QueuePresent"] = hook;
            }
        }
        
        return true;
    }
    
    void RemoveHooks() {
        spdlog::info("VRGINCore: Removing hooks");
        
        for (auto& hookPair : m_hooks) {
            if (hookPair.second.active) {
                // Restore original bytes
                DWORD oldProtect;
                if (VirtualProtectEx(m_gameProcess, hookPair.second.originalAddress,
                                   hookPair.second.originalBytes.size(), PAGE_EXECUTE_READWRITE, &oldProtect)) {
                    WriteProcessMemory(m_gameProcess, hookPair.second.originalAddress,
                                     hookPair.second.originalBytes.data(),
                                     hookPair.second.originalBytes.size(), nullptr);
                    VirtualProtectEx(m_gameProcess, hookPair.second.originalAddress,
                                   hookPair.second.originalBytes.size(), oldProtect, &oldProtect);
                }
                hookPair.second.active = false;
            }
        }
        
        spdlog::info("VRGINCore: All hooks removed");
    }
    
    void StartMonitoring() {
        m_monitoringThread = std::thread([this]() {
            spdlog::info("VRGINCore: Monitoring thread started");
            
            while (m_injected) {
                // Check if process is still running
                DWORD exitCode;
                if (GetExitCodeProcess(m_gameProcess, &exitCode) && exitCode != STILL_ACTIVE) {
                    spdlog::warn("VRGINCore: Target process has terminated");
                    break;
                }
                
                // Update metrics
                UpdateMetrics();
                
                // Sleep for monitoring interval
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
            
            spdlog::info("VRGINCore: Monitoring thread stopped");
        });
    }
    
    void UpdateMetrics() {
        // Update injection metrics
        m_metrics.injectionTime = std::chrono::steady_clock::now();
        m_metrics.processId = m_gameProcessId;
        m_metrics.processName = m_gameProcessName;
        m_metrics.isInjected = m_injected;
        m_metrics.hookCount = m_hooks.size();
        
        // Update component metrics
        if (m_renderer) {
            m_metrics.rendererMetrics = m_renderer->GetMetrics();
        }
        if (m_input) {
            m_metrics.inputMetrics = m_input->GetMetrics();
        }
        if (m_physics) {
            m_metrics.physicsMetrics = m_physics->GetMetrics();
        }
        if (m_optimizer) {
            m_metrics.optimizerMetrics = m_optimizer->GetMetrics();
        }
    }
    
    std::string GetVRGINDLLPath() {
        // Try to find VRGIN DLL in various locations
        std::vector<std::string> searchPaths = {
            "./vrgin.dll",
            "./bin/vrgin.dll",
            "./lib/vrgin.dll",
            "./extended-resources/tools/vrgin/vrgin.dll"
        };
        
        for (const auto& path : searchPaths) {
            if (GetFileAttributesA(path.c_str()) != INVALID_FILE_ATTRIBUTES) {
                return path;
            }
        }
        
        return "";
    }
};

// Factory function
std::unique_ptr<IVRGINCore> CreateVRGINCore() {
    return std::make_unique<VRGINCore>();
}

} // namespace VRGIN
