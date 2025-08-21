#include "ReshadeIntegration.hpp"
#include <spdlog/spdlog.h>
#include <d3d11.h>
#include <d3d12.h>
#include <vulkan/vulkan.h>
#include <Windows.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <memory>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <thread>
#include <regex>

namespace Reshade {

class ReshadeCore : public IReshadeCore {
private:
    ReshadeConfig m_config;
    ReshadeMetrics m_metrics;
    bool m_initialized;
    bool m_injectionActive;
    
    // Process information
    HANDLE m_targetProcess;
    DWORD m_targetProcessId;
    std::string m_targetProcessName;
    
    // DirectX resources for injection
    ID3D11Device* m_d3d11Device;
    ID3D11DeviceContext* m_d3d11Context;
    ID3D12Device* m_d3d12Device;
    ID3D12CommandQueue* m_d3d12CommandQueue;
    
    // Vulkan resources for injection
    VkInstance m_vulkanInstance;
    VkPhysicalDevice m_vulkanPhysicalDevice;
    VkDevice m_vulkanDevice;
    VkQueue m_vulkanQueue;
    
    // Shader management
    struct ShaderInfo {
        std::string name;
        std::string sourceCode;
        std::string compiledCode;
        ReshadeShaderType type;
        bool enabled;
        std::chrono::steady_clock::time_point lastModified;
    };
    std::unordered_map<std::string, ShaderInfo> m_shaders;
    
    // Post-processing pipeline
    struct PostProcessingPass {
        std::string name;
        std::string shaderName;
        bool enabled;
        std::unordered_map<std::string, float> parameters;
        uint32_t order;
    };
    std::vector<PostProcessingPass> m_postProcessingPipeline;
    
    // Injection thread
    std::thread m_injectionThread;
    std::mutex m_injectionMutex;
    bool m_injectionThreadRunning;
    
    // Shader compilation cache
    struct CompiledShader {
        std::string name;
        std::vector<uint8_t> bytecode;
        std::chrono::steady_clock::time_point compilationTime;
    };
    std::unordered_map<std::string, CompiledShader> m_compiledShaders;

public:
    ReshadeCore() : m_targetProcess(nullptr), m_targetProcessId(0), m_initialized(false), 
                    m_injectionActive(false), m_d3d11Device(nullptr), m_d3d11Context(nullptr),
                    m_d3d12Device(nullptr), m_d3d12CommandQueue(nullptr), 
                    m_vulkanInstance(VK_NULL_HANDLE), m_vulkanPhysicalDevice(VK_NULL_HANDLE),
                    m_vulkanDevice(VK_NULL_HANDLE), m_vulkanQueue(VK_NULL_HANDLE),
                    m_injectionThreadRunning(false) {
        spdlog::info("ReshadeCore: Constructor called");
    }
    
    ~ReshadeCore() {
        Shutdown();
    }

    bool Initialize(const ReshadeConfig& config) override {
        std::lock_guard<std::mutex> lock(m_injectionMutex);
        
        spdlog::info("ReshadeCore: Initializing with config - enableShaderInjection: {}, enableRealTimeUpdates: {}, enablePostProcessing: {}", 
                     config.enableShaderInjection, config.enableRealTimeUpdates, config.enablePostProcessing);
        
        m_config = config;
        m_metrics = ReshadeMetrics{};
        
        // Initialize rendering APIs based on config
        if (config.enableDirectX11 && !InitializeDirectX11()) {
            spdlog::error("ReshadeCore: Failed to initialize DirectX 11");
            return false;
        }
        
        if (config.enableDirectX12 && !InitializeDirectX12()) {
            spdlog::error("ReshadeCore: Failed to initialize DirectX 12");
            return false;
        }
        
        if (config.enableVulkan && !InitializeVulkan()) {
            spdlog::error("ReshadeCore: Failed to initialize Vulkan");
            return false;
        }
        
        // Load default shaders
        if (config.enableShaderInjection && !LoadDefaultShaders()) {
            spdlog::error("ReshadeCore: Failed to load default shaders");
            return false;
        }
        
        // Initialize post-processing pipeline
        if (config.enablePostProcessing && !InitializePostProcessingPipeline()) {
            spdlog::error("ReshadeCore: Failed to initialize post-processing pipeline");
            return false;
        }
        
        m_initialized = true;
        spdlog::info("ReshadeCore: Initialization completed successfully");
        return true;
    }

    bool InjectIntoProcess(const std::string& processName) override {
        std::lock_guard<std::mutex> lock(m_injectionMutex);
        
        if (!m_initialized) {
            spdlog::error("ReshadeCore: Cannot inject into process - not initialized");
            return false;
        }
        
        spdlog::info("ReshadeCore: Injecting into process: {}", processName);
        
        // Find the target process
        m_targetProcessId = FindProcessId(processName);
        if (m_targetProcessId == 0) {
            spdlog::error("ReshadeCore: Process '{}' not found", processName);
            return false;
        }
        
        // Open process with required permissions
        m_targetProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_targetProcessId);
        if (!m_targetProcess) {
            spdlog::error("ReshadeCore: Failed to open process (PID: {})", m_targetProcessId);
            return false;
        }
        
        m_targetProcessName = processName;
        
        // Inject Reshade DLL
        if (!InjectReshadeDLL()) {
            spdlog::error("ReshadeCore: Failed to inject Reshade DLL");
            return false;
        }
        
        // Start injection thread if real-time updates are enabled
        if (m_config.enableRealTimeUpdates) {
            StartInjectionThread();
        }
        
        m_injectionActive = true;
        spdlog::info("ReshadeCore: Successfully injected into process '{}' (PID: {})", processName, m_targetProcessId);
        return true;
    }

    bool LoadShader(const std::string& shaderPath) override {
        std::lock_guard<std::mutex> lock(m_injectionMutex);
        
        if (!m_initialized) {
            spdlog::error("ReshadeCore: Cannot load shader - not initialized");
            return false;
        }
        
        spdlog::info("ReshadeCore: Loading shader: {}", shaderPath);
        
        // Read shader file
        std::ifstream file(shaderPath);
        if (!file.is_open()) {
            spdlog::error("ReshadeCore: Failed to open shader file: {}", shaderPath);
            return false;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string sourceCode = buffer.str();
        
        // Parse shader metadata
        ReshadeShaderType shaderType = DetectShaderType(sourceCode);
        std::string shaderName = ExtractShaderName(sourceCode);
        
        // Create shader info
        ShaderInfo shaderInfo;
        shaderInfo.name = shaderName;
        shaderInfo.sourceCode = sourceCode;
        shaderInfo.type = shaderType;
        shaderInfo.enabled = true;
        shaderInfo.lastModified = std::chrono::steady_clock::now();
        
        // Compile shader
        if (!CompileShader(shaderInfo)) {
            spdlog::error("ReshadeCore: Failed to compile shader: {}", shaderName);
            return false;
        }
        
        // Add to shader collection
        m_shaders[shaderName] = shaderInfo;
        
        spdlog::info("ReshadeCore: Successfully loaded shader: {} (Type: {})", shaderName, static_cast<int>(shaderType));
        return true;
    }

    bool EnableShader(const std::string& shaderName) override {
        std::lock_guard<std::mutex> lock(m_injectionMutex);
        
        auto it = m_shaders.find(shaderName);
        if (it == m_shaders.end()) {
            spdlog::error("ReshadeCore: Shader not found: {}", shaderName);
            return false;
        }
        
        it->second.enabled = true;
        spdlog::info("ReshadeCore: Enabled shader: {}", shaderName);
        return true;
    }

    bool DisableShader(const std::string& shaderName) override {
        std::lock_guard<std::mutex> lock(m_injectionMutex);
        
        auto it = m_shaders.find(shaderName);
        if (it == m_shaders.end()) {
            spdlog::error("ReshadeCore: Shader not found: {}", shaderName);
            return false;
        }
        
        it->second.enabled = false;
        spdlog::info("ReshadeCore: Disabled shader: {}", shaderName);
        return true;
    }

    bool SetShaderParameter(const std::string& shaderName, const std::string& parameterName, float value) override {
        std::lock_guard<std::mutex> lock(m_injectionMutex);
        
        auto it = m_shaders.find(shaderName);
        if (it == m_shaders.end()) {
            spdlog::error("ReshadeCore: Shader not found: {}", shaderName);
            return false;
        }
        
        // Find the shader in post-processing pipeline
        for (auto& pass : m_postProcessingPipeline) {
            if (pass.shaderName == shaderName) {
                pass.parameters[parameterName] = value;
                spdlog::info("ReshadeCore: Set parameter {} = {} for shader {}", parameterName, value, shaderName);
                return true;
            }
        }
        
        spdlog::error("ReshadeCore: Shader not found in post-processing pipeline: {}", shaderName);
        return false;
    }

    ReshadeMetrics GetMetrics() const override {
        return m_metrics;
    }

    void Shutdown() override {
        std::lock_guard<std::mutex> lock(m_injectionMutex);
        
        spdlog::info("ReshadeCore: Shutting down");
        
        // Stop injection thread
        if (m_injectionThreadRunning) {
            m_injectionThreadRunning = false;
            if (m_injectionThread.joinable()) {
                m_injectionThread.join();
            }
        }
        
        // Close process handle
        if (m_targetProcess) {
            CloseHandle(m_targetProcess);
            m_targetProcess = nullptr;
        }
        
        // Clean up rendering APIs
        CleanupDirectX11();
        CleanupDirectX12();
        CleanupVulkan();
        
        // Clear shaders and pipeline
        m_shaders.clear();
        m_postProcessingPipeline.clear();
        m_compiledShaders.clear();
        
        m_initialized = false;
        m_injectionActive = false;
        
        spdlog::info("ReshadeCore: Shutdown completed");
    }

private:
    bool InitializeDirectX11() {
        spdlog::info("ReshadeCore: Initializing DirectX 11");
        
        // Create D3D11 device and context
        D3D_FEATURE_LEVEL featureLevel;
        D3D_FEATURE_LEVEL featureLevels[] = {
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0
        };
        
        HRESULT hr = D3D11CreateDevice(
            nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
            featureLevels, ARRAYSIZE(featureLevels), D3D11_SDK_VERSION,
            &m_d3d11Device, &featureLevel, &m_d3d11Context
        );
        
        if (FAILED(hr)) {
            spdlog::error("ReshadeCore: Failed to create D3D11 device");
            return false;
        }
        
        spdlog::info("ReshadeCore: DirectX 11 initialized successfully");
        return true;
    }
    
    bool InitializeDirectX12() {
        spdlog::info("ReshadeCore: Initializing DirectX 12");
        
        // Create D3D12 device
        HRESULT hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_d3d12Device));
        if (FAILED(hr)) {
            spdlog::error("ReshadeCore: Failed to create D3D12 device");
            return false;
        }
        
        // Create command queue
        D3D12_COMMAND_QUEUE_DESC queueDesc = {};
        queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        
        hr = m_d3d12Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_d3d12CommandQueue));
        if (FAILED(hr)) {
            spdlog::error("ReshadeCore: Failed to create D3D12 command queue");
            return false;
        }
        
        spdlog::info("ReshadeCore: DirectX 12 initialized successfully");
        return true;
    }
    
    bool InitializeVulkan() {
        spdlog::info("ReshadeCore: Initializing Vulkan");
        
        // Create Vulkan instance
        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Reshade Core";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "Reshade";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;
        
        VkInstanceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        
        if (vkCreateInstance(&createInfo, nullptr, &m_vulkanInstance) != VK_SUCCESS) {
            spdlog::error("ReshadeCore: Failed to create Vulkan instance");
            return false;
        }
        
        // Select physical device
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(m_vulkanInstance, &deviceCount, nullptr);
        
        if (deviceCount == 0) {
            spdlog::error("ReshadeCore: No Vulkan physical devices found");
            return false;
        }
        
        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(m_vulkanInstance, &deviceCount, devices.data());
        
        // Select first suitable device
        m_vulkanPhysicalDevice = devices[0];
        
        // Create logical device
        float queuePriority = 1.0f;
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = 0; // Assume first queue family
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        
        VkPhysicalDeviceFeatures deviceFeatures = {};
        
        VkDeviceCreateInfo deviceCreateInfo = {};
        deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
        deviceCreateInfo.queueCreateInfoCount = 1;
        deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
        
        if (vkCreateDevice(m_vulkanPhysicalDevice, &deviceCreateInfo, nullptr, &m_vulkanDevice) != VK_SUCCESS) {
            spdlog::error("ReshadeCore: Failed to create Vulkan logical device");
            return false;
        }
        
        vkGetDeviceQueue(m_vulkanDevice, 0, 0, &m_vulkanQueue);
        
        spdlog::info("ReshadeCore: Vulkan initialized successfully");
        return true;
    }
    
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
    
    bool InjectReshadeDLL() {
        spdlog::info("ReshadeCore: Injecting Reshade DLL");
        
        // Get path to Reshade DLL
        std::string dllPath = GetReshadeDLLPath();
        if (dllPath.empty()) {
            spdlog::error("ReshadeCore: Reshade DLL not found");
            return false;
        }
        
        // Allocate memory in target process for DLL path
        SIZE_T pathSize = dllPath.length() + 1;
        LPVOID remotePath = VirtualAllocEx(m_targetProcess, nullptr, pathSize, 
                                          MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if (!remotePath) {
            spdlog::error("ReshadeCore: Failed to allocate memory for DLL path");
            return false;
        }
        
        // Write DLL path to target process
        if (!WriteProcessMemory(m_targetProcess, remotePath, dllPath.c_str(), pathSize, nullptr)) {
            spdlog::error("ReshadeCore: Failed to write DLL path to target process");
            VirtualFreeEx(m_targetProcess, remotePath, 0, MEM_RELEASE);
            return false;
        }
        
        // Get LoadLibraryA address
        HMODULE kernel32 = GetModuleHandleA("kernel32.dll");
        LPVOID loadLibraryAddr = GetProcAddress(kernel32, "LoadLibraryA");
        
        // Create remote thread to load DLL
        HANDLE remoteThread = CreateRemoteThread(m_targetProcess, nullptr, 0,
                                                (LPTHREAD_START_ROUTINE)loadLibraryAddr,
                                                remotePath, 0, nullptr);
        if (!remoteThread) {
            spdlog::error("ReshadeCore: Failed to create remote thread for DLL injection");
            VirtualFreeEx(m_targetProcess, remotePath, 0, MEM_RELEASE);
            return false;
        }
        
        // Wait for thread completion
        WaitForSingleObject(remoteThread, INFINITE);
        
        // Clean up
        CloseHandle(remoteThread);
        VirtualFreeEx(m_targetProcess, remotePath, 0, MEM_RELEASE);
        
        spdlog::info("ReshadeCore: DLL injection completed");
        return true;
    }
    
    std::string GetReshadeDLLPath() {
        // Try to find Reshade DLL in various locations
        std::vector<std::string> searchPaths = {
            "./reshade.dll",
            "./bin/reshade.dll",
            "./lib/reshade.dll",
            "./extended-resources/tools/reshade/reshade.dll"
        };
        
        for (const auto& path : searchPaths) {
            if (GetFileAttributesA(path.c_str()) != INVALID_FILE_ATTRIBUTES) {
                return path;
            }
        }
        
        return "";
    }
    
    bool LoadDefaultShaders() {
        spdlog::info("ReshadeCore: Loading default shaders");
        
        // Create some basic default shaders
        std::vector<std::pair<std::string, std::string>> defaultShaders = {
            {"FXAA", GenerateFXAAShader()},
            {"Bloom", GenerateBloomShader()},
            {"ColorCorrection", GenerateColorCorrectionShader()},
            {"Sharpen", GenerateSharpenShader()}
        };
        
        for (const auto& [name, sourceCode] : defaultShaders) {
            ShaderInfo shaderInfo;
            shaderInfo.name = name;
            shaderInfo.sourceCode = sourceCode;
            shaderInfo.type = ReshadeShaderType::PostProcessing;
            shaderInfo.enabled = false; // Disabled by default
            shaderInfo.lastModified = std::chrono::steady_clock::now();
            
            if (CompileShader(shaderInfo)) {
                m_shaders[name] = shaderInfo;
                spdlog::info("ReshadeCore: Loaded default shader: {}", name);
            } else {
                spdlog::error("ReshadeCore: Failed to compile default shader: {}", name);
            }
        }
        
        return true;
    }
    
    std::string GenerateFXAAShader() {
        return R"(
// FXAA Shader
// Generated by ReshadeCore

#include "ReShade.fxh"

uniform float fxaa_quality_subpixel <
    ui_type = "slider";
    ui_min = 0.0; ui_max = 1.0;
    ui_label = "FXAA Quality Subpixel";
> = 0.75;

uniform float fxaa_quality_edge_threshold <
    ui_type = "slider";
    ui_min = 0.063; ui_max = 0.333;
    ui_label = "FXAA Quality Edge Threshold";
> = 0.166;

uniform float fxaa_quality_edge_threshold_min <
    ui_type = "slider";
    ui_min = 0.0312; ui_max = 0.0833;
    ui_label = "FXAA Quality Edge Threshold Min";
> = 0.0625;

float4 FXAA_PS(float4 position : SV_Position, float2 texcoord : TEXCOORD) : SV_Target
{
    float4 color = tex2D(ReShade::BackBuffer, texcoord);
    
    // Simple FXAA implementation
    float2 texelSize = ReShade::PixelSize;
    float2 offset = texelSize * 0.5;
    
    float4 topLeft = tex2D(ReShade::BackBuffer, texcoord + float2(-offset.x, -offset.y));
    float4 topRight = tex2D(ReShade::BackBuffer, texcoord + float2(offset.x, -offset.y));
    float4 bottomLeft = tex2D(ReShade::BackBuffer, texcoord + float2(-offset.x, offset.y));
    float4 bottomRight = tex2D(ReShade::BackBuffer, texcoord + float2(offset.x, offset.y));
    
    // Calculate edge detection
    float4 delta = abs(topLeft - bottomRight) + abs(topRight - bottomLeft);
    float edge = dot(delta.rgb, float3(0.299, 0.587, 0.114));
    
    if (edge < fxaa_quality_edge_threshold)
        return color;
    
    // Apply anti-aliasing
    float4 result = (topLeft + topRight + bottomLeft + bottomRight) * 0.25;
    return lerp(color, result, fxaa_quality_subpixel);
}

technique FXAA
{
    pass
    {
        VertexShader = PostProcessVS;
        PixelShader = FXAA_PS;
    }
}
)";
    }
    
    std::string GenerateBloomShader() {
        return R"(
// Bloom Shader
// Generated by ReshadeCore

#include "ReShade.fxh"

uniform float bloom_intensity <
    ui_type = "slider";
    ui_min = 0.0; ui_max = 2.0;
    ui_label = "Bloom Intensity";
> = 1.0;

uniform float bloom_threshold <
    ui_type = "slider";
    ui_min = 0.0; ui_max = 1.0;
    ui_label = "Bloom Threshold";
> = 0.8;

float4 Bloom_PS(float4 position : SV_Position, float2 texcoord : TEXCOORD) : SV_Target
{
    float4 color = tex2D(ReShade::BackBuffer, texcoord);
    
    // Extract bright areas
    float brightness = dot(color.rgb, float3(0.299, 0.587, 0.114));
    float4 bloom = max(0, color - bloom_threshold);
    
    // Apply bloom
    float4 result = color + bloom * bloom_intensity;
    
    return result;
}

technique Bloom
{
    pass
    {
        VertexShader = PostProcessVS;
        PixelShader = Bloom_PS;
    }
}
)";
    }
    
    std::string GenerateColorCorrectionShader() {
        return R"(
// Color Correction Shader
// Generated by ReshadeCore

#include "ReShade.fxh"

uniform float saturation <
    ui_type = "slider";
    ui_min = 0.0; ui_max = 2.0;
    ui_label = "Saturation";
> = 1.0;

uniform float contrast <
    ui_type = "slider";
    ui_min = 0.0; ui_max = 2.0;
    ui_label = "Contrast";
> = 1.0;

uniform float brightness <
    ui_type = "slider";
    ui_min = 0.0; ui_max = 2.0;
    ui_label = "Brightness";
> = 1.0;

float4 ColorCorrection_PS(float4 position : SV_Position, float2 texcoord : TEXCOORD) : SV_Target
{
    float4 color = tex2D(ReShade::BackBuffer, texcoord);
    
    // Apply brightness
    color.rgb *= brightness;
    
    // Apply contrast
    color.rgb = (color.rgb - 0.5) * contrast + 0.5;
    
    // Apply saturation
    float luminance = dot(color.rgb, float3(0.299, 0.587, 0.114));
    color.rgb = lerp(luminance, color.rgb, saturation);
    
    return color;
}

technique ColorCorrection
{
    pass
    {
        VertexShader = PostProcessVS;
        PixelShader = ColorCorrection_PS;
    }
}
)";
    }
    
    std::string GenerateSharpenShader() {
        return R"(
// Sharpen Shader
// Generated by ReshadeCore

#include "ReShade.fxh"

uniform float sharpen_strength <
    ui_type = "slider";
    ui_min = 0.0; ui_max = 2.0;
    ui_label = "Sharpen Strength";
> = 1.0;

float4 Sharpen_PS(float4 position : SV_Position, float2 texcoord : TEXCOORD) : SV_Target
{
    float2 texelSize = ReShade::PixelSize;
    
    float4 center = tex2D(ReShade::BackBuffer, texcoord);
    float4 left = tex2D(ReShade::BackBuffer, texcoord + float2(-texelSize.x, 0));
    float4 right = tex2D(ReShade::BackBuffer, texcoord + float2(texelSize.x, 0));
    float4 top = tex2D(ReShade::BackBuffer, texcoord + float2(0, -texelSize.y));
    float4 bottom = tex2D(ReShade::BackBuffer, texcoord + float2(0, texelSize.y));
    
    // Unsharp mask
    float4 blur = (left + right + top + bottom) * 0.25;
    float4 result = center + (center - blur) * sharpen_strength;
    
    return result;
}

technique Sharpen
{
    pass
    {
        VertexShader = PostProcessVS;
        PixelShader = Sharpen_PS;
    }
}
)";
    }
    
    bool InitializePostProcessingPipeline() {
        spdlog::info("ReshadeCore: Initializing post-processing pipeline");
        
        // Create default post-processing passes
        std::vector<std::string> defaultPasses = {"FXAA", "Bloom", "ColorCorrection", "Sharpen"};
        
        for (size_t i = 0; i < defaultPasses.size(); ++i) {
            PostProcessingPass pass;
            pass.name = defaultPasses[i];
            pass.shaderName = defaultPasses[i];
            pass.enabled = false; // Disabled by default
            pass.order = static_cast<uint32_t>(i);
            
            // Add default parameters
            if (pass.name == "FXAA") {
                pass.parameters["fxaa_quality_subpixel"] = 0.75f;
                pass.parameters["fxaa_quality_edge_threshold"] = 0.166f;
                pass.parameters["fxaa_quality_edge_threshold_min"] = 0.0625f;
            } else if (pass.name == "Bloom") {
                pass.parameters["bloom_intensity"] = 1.0f;
                pass.parameters["bloom_threshold"] = 0.8f;
            } else if (pass.name == "ColorCorrection") {
                pass.parameters["saturation"] = 1.0f;
                pass.parameters["contrast"] = 1.0f;
                pass.parameters["brightness"] = 1.0f;
            } else if (pass.name == "Sharpen") {
                pass.parameters["sharpen_strength"] = 1.0f;
            }
            
            m_postProcessingPipeline.push_back(pass);
        }
        
        spdlog::info("ReshadeCore: Initialized post-processing pipeline with {} passes", m_postProcessingPipeline.size());
        return true;
    }
    
    ReshadeShaderType DetectShaderType(const std::string& sourceCode) {
        // Simple detection based on shader content
        if (sourceCode.find("technique") != std::string::npos) {
            return ReshadeShaderType::PostProcessing;
        } else if (sourceCode.find("VertexShader") != std::string::npos) {
            return ReshadeShaderType::Vertex;
        } else if (sourceCode.find("PixelShader") != std::string::npos) {
            return ReshadeShaderType::Pixel;
        } else {
            return ReshadeShaderType::PostProcessing; // Default
        }
    }
    
    std::string ExtractShaderName(const std::string& sourceCode) {
        // Extract shader name from technique or other metadata
        std::regex techniqueRegex(R"(technique\s+(\w+))");
        std::smatch match;
        
        if (std::regex_search(sourceCode, match, techniqueRegex)) {
            return match[1].str();
        }
        
        return "UnknownShader";
    }
    
    bool CompileShader(ShaderInfo& shaderInfo) {
        spdlog::info("ReshadeCore: Compiling shader: {}", shaderInfo.name);
        
        // For now, we'll just store the source code as "compiled"
        // In a real implementation, this would use D3D11/D3D12/Vulkan shader compilation
        shaderInfo.compiledCode = shaderInfo.sourceCode;
        
        // Store in compilation cache
        CompiledShader compiledShader;
        compiledShader.name = shaderInfo.name;
        compiledShader.compilationTime = std::chrono::steady_clock::now();
        
        m_compiledShaders[shaderInfo.name] = compiledShader;
        
        spdlog::info("ReshadeCore: Successfully compiled shader: {}", shaderInfo.name);
        return true;
    }
    
    void StartInjectionThread() {
        if (m_injectionThreadRunning) {
            return;
        }
        
        m_injectionThreadRunning = true;
        m_injectionThread = std::thread([this]() {
            spdlog::info("ReshadeCore: Injection thread started");
            
            while (m_injectionThreadRunning) {
                // Monitor shader files for changes
                MonitorShaderFiles();
                
                // Update shader parameters
                UpdateShaderParameters();
                
                // Sleep based on update frequency
                std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 10 FPS
            }
            
            spdlog::info("ReshadeCore: Injection thread stopped");
        });
    }
    
    void MonitorShaderFiles() {
        // Check for modified shader files and reload them
        for (auto& [name, shader] : m_shaders) {
            // This would check file modification times in a real implementation
            // For now, we'll just update metrics
            m_metrics.shaderCount = m_shaders.size();
        }
    }
    
    void UpdateShaderParameters() {
        // Update shader parameters in the target process
        // This would involve communicating with the injected Reshade DLL
        m_metrics.lastUpdateTime = std::chrono::steady_clock::now();
    }
    
    void CleanupDirectX11() {
        if (m_d3d11Context) {
            m_d3d11Context->Release();
            m_d3d11Context = nullptr;
        }
        
        if (m_d3d11Device) {
            m_d3d11Device->Release();
            m_d3d11Device = nullptr;
        }
    }
    
    void CleanupDirectX12() {
        if (m_d3d12CommandQueue) {
            m_d3d12CommandQueue->Release();
            m_d3d12CommandQueue = nullptr;
        }
        
        if (m_d3d12Device) {
            m_d3d12Device->Release();
            m_d3d12Device = nullptr;
        }
    }
    
    void CleanupVulkan() {
        if (m_vulkanDevice) {
            vkDestroyDevice(m_vulkanDevice, nullptr);
            m_vulkanDevice = VK_NULL_HANDLE;
        }
        
        if (m_vulkanInstance) {
            vkDestroyInstance(m_vulkanInstance, nullptr);
            m_vulkanInstance = VK_NULL_HANDLE;
        }
    }
};

// Factory function
std::unique_ptr<IReshadeCore> CreateReshadeCore() {
    return std::make_unique<ReshadeCore>();
}

} // namespace Reshade
