#include "Depth3DIntegration.hpp"
#include <spdlog/spdlog.h>
#include <d3d11.h>
#include <d3d12.h>
#include <vulkan/vulkan.h>
#include <Windows.h>
#include <Psapi.h>
#include <TlHelp32.h>
#include <memory>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <thread>
#include <algorithm>
#include <cmath>

namespace Depth3D {

class Depth3DCore : public IDepth3DCore {
private:
    Depth3DConfig m_config;
    Depth3DMetrics m_metrics;
    bool m_initialized;
    bool m_extractionActive;
    
    // Process information
    HANDLE m_targetProcess;
    DWORD m_targetProcessId;
    std::string m_targetProcessName;
    
    // Memory regions for depth extraction
    struct MemoryRegion {
        LPVOID baseAddress;
        SIZE_T regionSize;
        DWORD protection;
        std::vector<uint8_t> data;
        bool isDepthBuffer;
        uint32_t width;
        uint32_t height;
        Depth3DFormat format;
    };
    std::vector<MemoryRegion> m_memoryRegions;
    
    // Depth buffer cache
    struct DepthBufferCache {
        std::vector<float> depthData;
        uint32_t width;
        uint32_t height;
        Depth3DFormat format;
        std::chrono::steady_clock::time_point timestamp;
        bool valid;
    };
    DepthBufferCache m_currentDepthBuffer;
    std::vector<DepthBufferCache> m_depthBufferHistory;
    
    // DirectX resources for extraction
    ID3D11Device* m_d3d11Device;
    ID3D11DeviceContext* m_d3d11Context;
    ID3D12Device* m_d3d12Device;
    ID3D12CommandQueue* m_d3d12CommandQueue;
    
    // Vulkan resources for extraction
    VkInstance m_vulkanInstance;
    VkPhysicalDevice m_vulkanPhysicalDevice;
    VkDevice m_vulkanDevice;
    VkQueue m_vulkanQueue;
    
    // Extraction thread
    std::thread m_extractionThread;
    std::mutex m_extractionMutex;
    bool m_extractionThreadRunning;
    
    // Pattern matching for depth buffer detection
    struct DepthPattern {
        std::string name;
        std::vector<uint8_t> signature;
        uint32_t offset;
        Depth3DFormat format;
        float confidence;
    };
    std::vector<DepthPattern> m_depthPatterns;

public:
    Depth3DCore() : m_targetProcess(nullptr), m_targetProcessId(0), m_initialized(false), 
                    m_extractionActive(false), m_d3d11Device(nullptr), m_d3d11Context(nullptr),
                    m_d3d12Device(nullptr), m_d3d12CommandQueue(nullptr), 
                    m_vulkanInstance(VK_NULL_HANDLE), m_vulkanPhysicalDevice(VK_NULL_HANDLE),
                    m_vulkanDevice(VK_NULL_HANDLE), m_vulkanQueue(VK_NULL_HANDLE),
                    m_extractionThreadRunning(false) {
        spdlog::info("Depth3DCore: Constructor called");
        
        // Initialize depth buffer cache
        m_currentDepthBuffer.valid = false;
        
        // Initialize depth patterns
        InitializeDepthPatterns();
    }
    
    ~Depth3DCore() {
        Shutdown();
    }

    bool Initialize(const Depth3DConfig& config) override {
        std::lock_guard<std::mutex> lock(m_extractionMutex);
        
        spdlog::info("Depth3DCore: Initializing with config - enableRealTime: {}, enableQualityAssessment: {}, enableMemoryScanning: {}", 
                     config.enableRealTime, config.enableQualityAssessment, config.enableMemoryScanning);
        
        m_config = config;
        m_metrics = Depth3DMetrics{};
        
        // Initialize rendering APIs based on config
        if (config.enableDirectX11 && !InitializeDirectX11()) {
            spdlog::error("Depth3DCore: Failed to initialize DirectX 11");
            return false;
        }
        
        if (config.enableDirectX12 && !InitializeDirectX12()) {
            spdlog::error("Depth3DCore: Failed to initialize DirectX 12");
            return false;
        }
        
        if (config.enableVulkan && !InitializeVulkan()) {
            spdlog::error("Depth3DCore: Failed to initialize Vulkan");
            return false;
        }
        
        m_initialized = true;
        spdlog::info("Depth3DCore: Initialization completed successfully");
        return true;
    }

    bool AttachToProcess(const std::string& processName) override {
        std::lock_guard<std::mutex> lock(m_extractionMutex);
        
        if (!m_initialized) {
            spdlog::error("Depth3DCore: Cannot attach to process - not initialized");
            return false;
        }
        
        spdlog::info("Depth3DCore: Attaching to process: {}", processName);
        
        // Find the target process
        m_targetProcessId = FindProcessId(processName);
        if (m_targetProcessId == 0) {
            spdlog::error("Depth3DCore: Process '{}' not found", processName);
            return false;
        }
        
        // Open process with required permissions
        m_targetProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_targetProcessId);
        if (!m_targetProcess) {
            spdlog::error("Depth3DCore: Failed to open process (PID: {})", m_targetProcessId);
            return false;
        }
        
        m_targetProcessName = processName;
        
        // Scan process memory for potential depth buffers
        if (!ScanProcessMemory()) {
            spdlog::error("Depth3DCore: Failed to scan process memory");
            return false;
        }
        
        // Start extraction thread if real-time extraction is enabled
        if (m_config.enableRealTime) {
            StartExtractionThread();
        }
        
        spdlog::info("Depth3DCore: Successfully attached to process '{}' (PID: {})", processName, m_targetProcessId);
        return true;
    }

    bool ExtractDepthBuffer() override {
        std::lock_guard<std::mutex> lock(m_extractionMutex);
        
        if (!m_initialized || !m_targetProcess) {
            spdlog::error("Depth3DCore: Cannot extract depth buffer - not initialized or no target process");
            return false;
        }
        
        spdlog::info("Depth3DCore: Extracting depth buffer");
        
        // Try different extraction methods
        if (m_d3d11Device && m_d3d11Context) {
            if (ExtractDepthBufferD3D11()) {
                return true;
            }
        }
        
        if (m_d3d12Device && m_d3d12CommandQueue) {
            if (ExtractDepthBufferD3D12()) {
                return true;
            }
        }
        
        if (m_vulkanDevice) {
            if (ExtractDepthBufferVulkan()) {
                return true;
            }
        }
        
        // Fallback to memory scanning
        if (m_config.enableMemoryScanning) {
            return ExtractDepthBufferFromMemory();
        }
        
        spdlog::error("Depth3DCore: All depth buffer extraction methods failed");
        return false;
    }

    bool ProcessDepthBuffer(const Depth3DBuffer& buffer) override {
        std::lock_guard<std::mutex> lock(m_extractionMutex);
        
        if (!m_initialized) {
            spdlog::error("Depth3DCore: Cannot process depth buffer - not initialized");
            return false;
        }
        
        spdlog::info("Depth3DCore: Processing depth buffer - Size: {}x{}, Format: {}", 
                     buffer.width, buffer.height, static_cast<int>(buffer.format));
        
        // Validate buffer
        if (!ValidateDepthBuffer(buffer)) {
            spdlog::error("Depth3DCore: Invalid depth buffer");
            return false;
        }
        
        // Convert depth data based on format
        std::vector<float> processedDepthData;
        if (!ConvertDepthData(buffer, processedDepthData)) {
            spdlog::error("Depth3DCore: Failed to convert depth data");
            return false;
        }
        
        // Apply quality assessment
        if (m_config.enableQualityAssessment) {
            float quality = AssessDepthQuality(processedDepthData, buffer.width, buffer.height);
            spdlog::info("Depth3DCore: Depth buffer quality assessment: {:.2f}%", quality * 100.0f);
        }
        
        // Update current depth buffer cache
        m_currentDepthBuffer.depthData = std::move(processedDepthData);
        m_currentDepthBuffer.width = buffer.width;
        m_currentDepthBuffer.height = buffer.height;
        m_currentDepthBuffer.format = buffer.format;
        m_currentDepthBuffer.timestamp = std::chrono::steady_clock::now();
        m_currentDepthBuffer.valid = true;
        
        // Add to history if enabled
        if (m_config.enableHistory) {
            AddToHistory(m_currentDepthBuffer);
        }
        
        // Update metrics
        UpdateMetrics();
        
        spdlog::info("Depth3DCore: Depth buffer processed successfully");
        return true;
    }

    Depth3DBuffer GetCurrentDepthBuffer() const override {
        std::lock_guard<std::mutex> lock(m_extractionMutex);
        
        if (!m_currentDepthBuffer.valid) {
            return Depth3DBuffer{};
        }
        
        Depth3DBuffer buffer;
        buffer.width = m_currentDepthBuffer.width;
        buffer.height = m_currentDepthBuffer.height;
        buffer.format = m_currentDepthBuffer.format;
        buffer.data = m_currentDepthBuffer.depthData;
        buffer.timestamp = m_currentDepthBuffer.timestamp;
        
        return buffer;
    }

    Depth3DMetrics GetMetrics() const override {
        return m_metrics;
    }

    void Shutdown() override {
        std::lock_guard<std::mutex> lock(m_extractionMutex);
        
        spdlog::info("Depth3DCore: Shutting down");
        
        // Stop extraction thread
        if (m_extractionThreadRunning) {
            m_extractionThreadRunning = false;
            if (m_extractionThread.joinable()) {
                m_extractionThread.join();
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
        
        // Clear memory regions and cache
        m_memoryRegions.clear();
        m_currentDepthBuffer.valid = false;
        m_depthBufferHistory.clear();
        
        m_initialized = false;
        m_extractionActive = false;
        
        spdlog::info("Depth3DCore: Shutdown completed");
    }

private:
    void InitializeDepthPatterns() {
        spdlog::info("Depth3DCore: Initializing depth buffer patterns");
        
        // D3D11 depth buffer patterns
        DepthPattern d3d11Depth32 = {
            "D3D11_Depth32",
            {0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x00, 0x00}, // Common D3D11 depth pattern
            0,
            Depth3DFormat::D32_FLOAT,
            0.8f
        };
        m_depthPatterns.push_back(d3d11Depth32);
        
        // D3D11 depth 24-bit patterns
        DepthPattern d3d11Depth24 = {
            "D3D11_Depth24",
            {0xFF, 0xFF, 0xFF, 0x00}, // 24-bit depth pattern
            0,
            Depth3DFormat::D24_UNORM_S8_UINT,
            0.7f
        };
        m_depthPatterns.push_back(d3d11Depth24);
        
        // Vulkan depth patterns
        DepthPattern vulkanDepth32 = {
            "Vulkan_Depth32",
            {0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x00, 0x00}, // Vulkan depth pattern
            0,
            Depth3DFormat::D32_SFLOAT,
            0.8f
        };
        m_depthPatterns.push_back(vulkanDepth32);
        
        spdlog::info("Depth3DCore: Initialized {} depth buffer patterns", m_depthPatterns.size());
    }
    
    bool InitializeDirectX11() {
        spdlog::info("Depth3DCore: Initializing DirectX 11");
        
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
            spdlog::error("Depth3DCore: Failed to create D3D11 device");
            return false;
        }
        
        spdlog::info("Depth3DCore: DirectX 11 initialized successfully");
        return true;
    }
    
    bool InitializeDirectX12() {
        spdlog::info("Depth3DCore: Initializing DirectX 12");
        
        // Create D3D12 device
        HRESULT hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_d3d12Device));
        if (FAILED(hr)) {
            spdlog::error("Depth3DCore: Failed to create D3D12 device");
            return false;
        }
        
        // Create command queue
        D3D12_COMMAND_QUEUE_DESC queueDesc = {};
        queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        
        hr = m_d3d12Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_d3d12CommandQueue));
        if (FAILED(hr)) {
            spdlog::error("Depth3DCore: Failed to create D3D12 command queue");
            return false;
        }
        
        spdlog::info("Depth3DCore: DirectX 12 initialized successfully");
        return true;
    }
    
    bool InitializeVulkan() {
        spdlog::info("Depth3DCore: Initializing Vulkan");
        
        // Create Vulkan instance
        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Depth3D Core";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "Depth3D";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;
        
        VkInstanceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        
        if (vkCreateInstance(&createInfo, nullptr, &m_vulkanInstance) != VK_SUCCESS) {
            spdlog::error("Depth3DCore: Failed to create Vulkan instance");
            return false;
        }
        
        // Select physical device
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(m_vulkanInstance, &deviceCount, nullptr);
        
        if (deviceCount == 0) {
            spdlog::error("Depth3DCore: No Vulkan physical devices found");
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
            spdlog::error("Depth3DCore: Failed to create Vulkan logical device");
            return false;
        }
        
        vkGetDeviceQueue(m_vulkanDevice, 0, 0, &m_vulkanQueue);
        
        spdlog::info("Depth3DCore: Vulkan initialized successfully");
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
    
    bool ScanProcessMemory() {
        spdlog::info("Depth3DCore: Scanning process memory for depth buffers");
        
        MEMORY_BASIC_INFORMATION mbi;
        LPVOID address = 0;
        
        while (VirtualQueryEx(m_targetProcess, address, &mbi, sizeof(mbi))) {
            if (mbi.State == MEM_COMMIT && 
                (mbi.Protect & PAGE_READWRITE) || 
                (mbi.Protect & PAGE_EXECUTE_READWRITE)) {
                
                MemoryRegion region;
                region.baseAddress = mbi.BaseAddress;
                region.regionSize = mbi.RegionSize;
                region.protection = mbi.Protect;
                region.isDepthBuffer = false;
                
                // Read memory data
                region.data.resize(mbi.RegionSize);
                SIZE_T bytesRead;
                if (ReadProcessMemory(m_targetProcess, mbi.BaseAddress, 
                                    region.data.data(), mbi.RegionSize, &bytesRead)) {
                    
                    // Check if this region contains depth buffer patterns
                    if (DetectDepthBufferPattern(region)) {
                        region.isDepthBuffer = true;
                        spdlog::info("Depth3DCore: Found potential depth buffer at 0x{:X}, size: {}", 
                                   reinterpret_cast<uintptr_t>(region.baseAddress), region.regionSize);
                    }
                    
                    m_memoryRegions.push_back(region);
                }
            }
            
            address = (LPVOID)((ULONG_PTR)mbi.BaseAddress + mbi.RegionSize);
        }
        
        spdlog::info("Depth3DCore: Scanned {} memory regions, found {} potential depth buffers", 
                     m_memoryRegions.size(), 
                     std::count_if(m_memoryRegions.begin(), m_memoryRegions.end(), 
                                  [](const MemoryRegion& r) { return r.isDepthBuffer; }));
        return true;
    }
    
    bool DetectDepthBufferPattern(MemoryRegion& region) {
        for (const auto& pattern : m_depthPatterns) {
            if (region.data.size() >= pattern.signature.size()) {
                for (size_t i = 0; i <= region.data.size() - pattern.signature.size(); ++i) {
                    bool match = true;
                    for (size_t j = 0; j < pattern.signature.size(); ++j) {
                        if (region.data[i + j] != pattern.signature[j]) {
                            match = false;
                            break;
                        }
                    }
                    
                    if (match) {
                        region.width = 1920; // Default width, will be refined
                        region.height = 1080; // Default height, will be refined
                        region.format = pattern.format;
                        return true;
                    }
                }
            }
        }
        
        return false;
    }
    
    bool ExtractDepthBufferD3D11() {
        spdlog::info("Depth3DCore: Attempting D3D11 depth buffer extraction");
        
        // This would involve hooking D3D11 functions and extracting depth buffers
        // For now, we'll implement a basic approach using memory scanning
        
        for (const auto& region : m_memoryRegions) {
            if (region.isDepthBuffer && region.format == Depth3DFormat::D32_FLOAT) {
                // Extract depth data from this region
                Depth3DBuffer buffer;
                buffer.width = region.width;
                buffer.height = region.height;
                buffer.format = region.format;
                buffer.data.resize(region.width * region.height);
                
                // Copy and convert depth data
                const float* srcData = reinterpret_cast<const float*>(region.data.data());
                for (uint32_t i = 0; i < buffer.data.size(); ++i) {
                    buffer.data[i] = srcData[i];
                }
                
                buffer.timestamp = std::chrono::steady_clock::now();
                
                if (ProcessDepthBuffer(buffer)) {
                    spdlog::info("Depth3DCore: Successfully extracted D3D11 depth buffer");
                    return true;
                }
            }
        }
        
        spdlog::warn("Depth3DCore: D3D11 depth buffer extraction failed");
        return false;
    }
    
    bool ExtractDepthBufferD3D12() {
        spdlog::info("Depth3DCore: Attempting D3D12 depth buffer extraction");
        // TODO: Implement D3D12 depth buffer extraction
        spdlog::warn("Depth3DCore: D3D12 depth buffer extraction not yet implemented");
        return false;
    }
    
    bool ExtractDepthBufferVulkan() {
        spdlog::info("Depth3DCore: Attempting Vulkan depth buffer extraction");
        // TODO: Implement Vulkan depth buffer extraction
        spdlog::warn("Depth3DCore: Vulkan depth buffer extraction not yet implemented");
        return false;
    }
    
    bool ExtractDepthBufferFromMemory() {
        spdlog::info("Depth3DCore: Attempting memory-based depth buffer extraction");
        
        for (const auto& region : m_memoryRegions) {
            if (region.isDepthBuffer) {
                Depth3DBuffer buffer;
                buffer.width = region.width;
                buffer.height = region.height;
                buffer.format = region.format;
                buffer.data.resize(region.width * region.height);
                
                // Convert depth data based on format
                if (region.format == Depth3DFormat::D32_FLOAT) {
                    const float* srcData = reinterpret_cast<const float*>(region.data.data());
                    for (uint32_t i = 0; i < buffer.data.size(); ++i) {
                        buffer.data[i] = srcData[i];
                    }
                } else if (region.format == Depth3DFormat::D24_UNORM_S8_UINT) {
                    const uint32_t* srcData = reinterpret_cast<const uint32_t*>(region.data.data());
                    for (uint32_t i = 0; i < buffer.data.size(); ++i) {
                        // Extract 24-bit depth value
                        uint32_t depth24 = srcData[i] & 0xFFFFFF;
                        buffer.data[i] = static_cast<float>(depth24) / 16777215.0f;
                    }
                }
                
                buffer.timestamp = std::chrono::steady_clock::now();
                
                if (ProcessDepthBuffer(buffer)) {
                    spdlog::info("Depth3DCore: Successfully extracted depth buffer from memory");
                    return true;
                }
            }
        }
        
        spdlog::warn("Depth3DCore: Memory-based depth buffer extraction failed");
        return false;
    }
    
    bool ValidateDepthBuffer(const Depth3DBuffer& buffer) {
        if (buffer.width == 0 || buffer.height == 0) {
            spdlog::error("Depth3DCore: Invalid buffer dimensions: {}x{}", buffer.width, buffer.height);
            return false;
        }
        
        if (buffer.data.size() != buffer.width * buffer.height) {
            spdlog::error("Depth3DCore: Buffer size mismatch: expected {}, got {}", 
                         buffer.width * buffer.height, buffer.data.size());
            return false;
        }
        
        // Check for reasonable depth values
        bool hasValidDepth = false;
        for (float depth : buffer.data) {
            if (depth >= 0.0f && depth <= 1.0f) {
                hasValidDepth = true;
                break;
            }
        }
        
        if (!hasValidDepth) {
            spdlog::warn("Depth3DCore: No valid depth values found in buffer");
            return false;
        }
        
        return true;
    }
    
    bool ConvertDepthData(const Depth3DBuffer& buffer, std::vector<float>& output) {
        output.resize(buffer.data.size());
        
        switch (buffer.format) {
            case Depth3DFormat::D32_FLOAT:
                // Already in correct format
                output = buffer.data;
                break;
                
            case Depth3DFormat::D24_UNORM_S8_UINT:
                // Convert from 24-bit normalized to float
                for (size_t i = 0; i < buffer.data.size(); ++i) {
                    output[i] = buffer.data[i]; // Already converted in extraction
                }
                break;
                
            case Depth3DFormat::D16_UNORM:
                // Convert from 16-bit normalized to float
                for (size_t i = 0; i < buffer.data.size(); ++i) {
                    output[i] = buffer.data[i] * 65535.0f / 65535.0f; // Normalize
                }
                break;
                
            default:
                spdlog::error("Depth3DCore: Unsupported depth format: {}", static_cast<int>(buffer.format));
                return false;
        }
        
        return true;
    }
    
    float AssessDepthQuality(const std::vector<float>& depthData, uint32_t width, uint32_t height) {
        if (depthData.empty()) {
            return 0.0f;
        }
        
        // Calculate various quality metrics
        
        // 1. Depth range
        float minDepth = *std::min_element(depthData.begin(), depthData.end());
        float maxDepth = *std::max_element(depthData.begin(), depthData.end());
        float depthRange = maxDepth - minDepth;
        
        // 2. Depth variance (indicates detail)
        float mean = 0.0f;
        for (float depth : depthData) {
            mean += depth;
        }
        mean /= depthData.size();
        
        float variance = 0.0f;
        for (float depth : depthData) {
            float diff = depth - mean;
            variance += diff * diff;
        }
        variance /= depthData.size();
        
        // 3. Edge detection (depth discontinuities)
        uint32_t edgeCount = 0;
        for (uint32_t y = 1; y < height - 1; ++y) {
            for (uint32_t x = 1; x < width - 1; ++x) {
                uint32_t idx = y * width + x;
                float center = depthData[idx];
                
                // Check horizontal edge
                float left = depthData[idx - 1];
                float right = depthData[idx + 1];
                if (std::abs(center - left) > 0.1f || std::abs(center - right) > 0.1f) {
                    edgeCount++;
                }
                
                // Check vertical edge
                float top = depthData[idx - width];
                float bottom = depthData[idx + width];
                if (std::abs(center - top) > 0.1f || std::abs(center - bottom) > 0.1f) {
                    edgeCount++;
                }
            }
        }
        
        // Calculate quality score
        float rangeScore = std::min(depthRange, 1.0f);
        float varianceScore = std::min(variance * 10.0f, 1.0f);
        float edgeScore = std::min(static_cast<float>(edgeCount) / (width * height), 1.0f);
        
        float quality = (rangeScore + varianceScore + edgeScore) / 3.0f;
        
        spdlog::debug("Depth3DCore: Quality assessment - Range: {:.3f}, Variance: {:.3f}, Edges: {:.3f}, Overall: {:.3f}", 
                     rangeScore, varianceScore, edgeScore, quality);
        
        return quality;
    }
    
    void AddToHistory(const DepthBufferCache& buffer) {
        m_depthBufferHistory.push_back(buffer);
        
        // Keep only recent history
        const size_t maxHistory = 10;
        if (m_depthBufferHistory.size() > maxHistory) {
            m_depthBufferHistory.erase(m_depthBufferHistory.begin());
        }
    }
    
    void UpdateMetrics() {
        m_metrics.extractionCount++;
        m_metrics.lastExtractionTime = std::chrono::steady_clock::now();
        
        if (m_currentDepthBuffer.valid) {
            m_metrics.currentBufferWidth = m_currentDepthBuffer.width;
            m_metrics.currentBufferHeight = m_currentDepthBuffer.height;
            m_metrics.currentBufferFormat = static_cast<int>(m_currentDepthBuffer.format);
            m_metrics.bufferHistorySize = m_depthBufferHistory.size();
        }
    }
    
    void StartExtractionThread() {
        if (m_extractionThreadRunning) {
            return;
        }
        
        m_extractionThreadRunning = true;
        m_extractionThread = std::thread([this]() {
            spdlog::info("Depth3DCore: Extraction thread started");
            
            while (m_extractionThreadRunning) {
                // Extract depth buffer
                ExtractDepthBuffer();
                
                // Sleep based on extraction frequency
                std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS
            }
            
            spdlog::info("Depth3DCore: Extraction thread stopped");
        });
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
std::unique_ptr<IDepth3DCore> CreateDepth3DCore() {
    return std::make_unique<Depth3DCore>();
}

} // namespace Depth3D
