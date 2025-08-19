#include <uevr/hooks/VulkanHooks.hpp>
#include <MinHook.h>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <cstring>

namespace uevr {

VulkanHooks::VulkanHooks() 
    : m_isHooked(false)
    , m_vrEnabled(false)
    , m_lastError("")
    , m_performanceMetrics{0, 0, 0.0, 0.0, 0}
{
    // Initialize MinHook
    if (MH_Initialize() != MH_OK) {
        m_lastError = "Failed to initialize MinHook";
        return;
    }
    
    // Initialize VR state
    m_vrState.stereoEnabled = false;
    m_vrState.ipd = 0.064f; // Default IPD
    m_vrState.nearPlane = 0.1f;
    m_vrState.farPlane = 1000.0f;
    
    // Initialize matrices to identity
    std::memset(m_vrState.leftEye.viewMatrix, 0, sizeof(m_vrState.leftEye.viewMatrix));
    std::memset(m_vrState.leftEye.projectionMatrix, 0, sizeof(m_vrState.leftEye.projectionMatrix));
    std::memset(m_vrState.rightEye.viewMatrix, 0, sizeof(m_vrState.rightEye.viewMatrix));
    std::memset(m_vrState.rightEye.projectionMatrix, 0, sizeof(m_vrState.rightEye.projectionMatrix));
    
    // Set identity matrices
    m_vrState.leftEye.viewMatrix[0] = m_vrState.leftEye.viewMatrix[5] = 
    m_vrState.leftEye.viewMatrix[10] = m_vrState.leftEye.viewMatrix[15] = 1.0f;
    
    m_vrState.leftEye.projectionMatrix[0] = m_vrState.leftEye.projectionMatrix[5] = 
    m_vrState.leftEye.projectionMatrix[10] = m_vrState.leftEye.projectionMatrix[15] = 1.0f;
    
    m_vrState.rightEye.viewMatrix[0] = m_vrState.rightEye.viewMatrix[5] = 
    m_vrState.rightEye.viewMatrix[10] = m_vrState.rightEye.viewMatrix[15] = 1.0f;
    
    m_vrState.rightEye.projectionMatrix[0] = m_vrState.rightEye.projectionMatrix[5] = 
    m_vrState.rightEye.projectionMatrix[10] = m_vrState.rightEye.projectionMatrix[15] = 1.0f;
}

VulkanHooks::~VulkanHooks() {
    cleanupHooks();
    MH_Uninitialize();
}

bool VulkanHooks::initializeHooks() {
    if (m_isHooked) {
        return true; // Already hooked
    }
    
    std::cout << "[VulkanHooks] Initializing Vulkan hooks..." << std::endl;
    
    // Get Vulkan function pointers from loaded modules
    HMODULE vulkanModule = GetModuleHandleA("vulkan-1.dll");
    if (!vulkanModule) {
        vulkanModule = GetModuleHandleA("vulkan-1");
    }
    
    if (!vulkanModule) {
        m_lastError = "Vulkan module not found";
        return false;
    }
    
    // Get original function pointers
    m_originalFunctions.vkCreateSwapchainKHR = reinterpret_cast<PFN_vkCreateSwapchainKHR>(
        GetProcAddress(vulkanModule, "vkCreateSwapchainKHR")
    );
    m_originalFunctions.vkQueuePresentKHR = reinterpret_cast<PFN_vkQueuePresentKHR>(
        GetProcAddress(vulkanModule, "vkQueuePresentKHR")
    );
    m_originalFunctions.vkCmdBeginRenderPass = reinterpret_cast<PFN_vkCmdBeginRenderPass>(
        GetProcAddress(vulkanModule, "vkCmdBeginRenderPass")
    );
    m_originalFunctions.vkCmdDraw = reinterpret_cast<PFN_vkCmdDraw>(
        GetProcAddress(vulkanModule, "vkCmdDraw")
    );
    
    if (!m_originalFunctions.vkCreateSwapchainKHR || !m_originalFunctions.vkQueuePresentKHR ||
        !m_originalFunctions.vkCmdBeginRenderPass || !m_originalFunctions.vkCmdDraw) {
        m_lastError = "Failed to get Vulkan function pointers";
        return false;
    }
    
    // Install hooks
    if (!installHook(reinterpret_cast<void**>(&m_originalFunctions.vkCreateSwapchainKHR),
                     reinterpret_cast<void*>(CreateSwapchainKHR_Hook), "vkCreateSwapchainKHR")) {
        return false;
    }
    
    if (!installHook(reinterpret_cast<void**>(&m_originalFunctions.vkQueuePresentKHR),
                     reinterpret_cast<void*>(QueuePresentKHR_Hook), "vkQueuePresentKHR")) {
        return false;
    }
    
    if (!installHook(reinterpret_cast<void**>(&m_originalFunctions.vkCmdBeginRenderPass),
                     reinterpret_cast<void*>(CmdBeginRenderPass_Hook), "vkCmdBeginRenderPass")) {
        return false;
    }
    
    if (!installHook(reinterpret_cast<void**>(&m_originalFunctions.vkCmdDraw),
                     reinterpret_cast<void*>(CmdDraw_Hook), "vkCmdDraw")) {
        return false;
    }
    
    m_isHooked = true;
    std::cout << "[VulkanHooks] All Vulkan hooks installed successfully" << std::endl;
    return true;
}

void VulkanHooks::cleanupHooks() {
    if (!m_isHooked) {
        return;
    }
    
    std::cout << "[VulkanHooks] Cleaning up Vulkan hooks..." << std::endl;
    
    // Disable all hooks
    MH_DisableHook(MH_ALL_HOOKS);
    
    // Uninstall hooks
    MH_Unhook(reinterpret_cast<void*>(m_originalFunctions.vkCreateSwapchainKHR));
    MH_Unhook(reinterpret_cast<void*>(m_originalFunctions.vkQueuePresentKHR));
    MH_Unhook(reinterpret_cast<void*>(m_originalFunctions.vkCmdBeginRenderPass));
    MH_Unhook(reinterpret_cast<void*>(m_originalFunctions.vkCmdDraw));
    
    m_isHooked = false;
    std::cout << "[VulkanHooks] Vulkan hooks cleaned up" << std::endl;
}

void VulkanHooks::registerSwapchainCreateHook(SwapchainCreateInfoHook hook) {
    m_swapchainCreateHooks.push_back(hook);
}

void VulkanHooks::registerQueuePresentHook(QueuePresentHook hook) {
    m_queuePresentHooks.push_back(hook);
}

void VulkanHooks::registerCmdBeginRenderPassHook(CmdBeginRenderPassHook hook) {
    m_cmdBeginRenderPassHooks.push_back(hook);
}

void VulkanHooks::registerCmdDrawHook(CmdDrawHook hook) {
    m_cmdDrawHooks.push_back(hook);
}

bool VulkanHooks::extractViewProjectionMatrices(VkBuffer uniformBuffer, VkDeviceSize offset, VkDeviceSize size) {
    // This is a simplified implementation
    // In a real scenario, you would need to map the buffer and extract matrices
    
    std::cout << "[VulkanHooks] Extracting matrices from buffer (offset: " << offset << ", size: " << size << ")" << std::endl;
    
    // Simulate matrix extraction
    if (size >= 128) { // Minimum size for view + projection matrices
        // Update performance metrics
        m_performanceMetrics.frameCount++;
        updatePerformanceMetrics();
        
        return true;
    }
    
    return false;
}

bool VulkanHooks::enableVRStereoRendering() {
    if (m_vrEnabled) {
        return true; // Already enabled
    }
    
    std::cout << "[VulkanHooks] Enabling VR stereo rendering..." << std::endl;
    
    m_vrState.stereoEnabled = true;
    m_vrEnabled = true;
    
    // Apply VR transformations
    applyVRTransformation();
    
    std::cout << "[VulkanHooks] VR stereo rendering enabled" << std::endl;
    return true;
}

bool VulkanHooks::disableVRStereoRendering() {
    if (!m_vrEnabled) {
        return true; // Already disabled
    }
    
    std::cout << "[VulkanHooks] Disabling VR stereo rendering..." << std::endl;
    
    m_vrState.stereoEnabled = false;
    m_vrEnabled = false;
    
    std::cout << "[VulkanHooks] VR stereo rendering disabled" << std::endl;
    return true;
}

VulkanHooks::PerformanceMetrics VulkanHooks::getPerformanceMetrics() const {
    return m_performanceMetrics;
}

void VulkanHooks::clearErrors() {
    m_lastError.clear();
}

// Static hook implementations
VkResult VKAPI_CALL VulkanHooks::CreateSwapchainKHR_Hook(
    VkDevice device,
    const VkSwapchainCreateInfoKHR* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkSwapchainKHR* pSwapchain
) {
    std::cout << "[VulkanHooks] vkCreateSwapchainKHR called" << std::endl;
    
    // Call original function
    auto originalFunc = reinterpret_cast<PFN_vkCreateSwapchainKHR>(
        GetProcAddress(GetModuleHandleA("vulkan-1.dll"), "vkCreateSwapchainKHR")
    );
    
    if (originalFunc) {
        return originalFunc(device, pCreateInfo, pAllocator, pSwapchain);
    }
    
    return VK_ERROR_INITIALIZATION_FAILED;
}

VkResult VKAPI_CALL VulkanHooks::QueuePresentKHR_Hook(
    VkQueue queue,
    const VkPresentInfoKHR* pPresentInfo
) {
    static auto startTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - startTime);
    
    std::cout << "[VulkanHooks] vkQueuePresentKHR called (frame time: " << duration.count() << " μs)" << std::endl;
    
    // Call original function
    auto originalFunc = reinterpret_cast<PFN_vkQueuePresentKHR>(
        GetProcAddress(GetModuleHandleA("vulkan-1.dll"), "vkQueuePresentKHR")
    );
    
    if (originalFunc) {
        startTime = currentTime; // Reset for next frame
        return originalFunc(queue, pPresentInfo);
    }
    
    return VK_ERROR_INITIALIZATION_FAILED;
}

void VKAPI_CALL VulkanHooks::CmdBeginRenderPass_Hook(
    VkCommandBuffer commandBuffer,
    const VkRenderPassBeginInfo* pRenderPassBegin,
    VkSubpassContents contents
) {
    std::cout << "[VulkanHooks] vkCmdBeginRenderPass called" << std::endl;
    
    // Call original function
    auto originalFunc = reinterpret_cast<PFN_vkCmdBeginRenderPass>(
        GetProcAddress(GetModuleHandleA("vulkan-1.dll"), "vkCmdBeginRenderPass")
    );
    
    if (originalFunc) {
        originalFunc(commandBuffer, pRenderPassBegin, contents);
    }
}

void VKAPI_CALL VulkanHooks::CmdDraw_Hook(
    VkCommandBuffer commandBuffer,
    uint32_t vertexCount,
    uint32_t instanceCount,
    uint32_t firstVertex,
    uint32_t firstInstance
) {
    std::cout << "[VulkanHooks] vkCmdDraw called (vertices: " << vertexCount << ", instances: " << instanceCount << ")" << std::endl;
    
    // Call original function
    auto originalFunc = reinterpret_cast<PFN_vkCmdDraw>(
        GetProcAddress(GetModuleHandleA("vulkan-1.dll"), "vkCmdDraw")
    );
    
    if (originalFunc) {
        originalFunc(commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
    }
}

// Private helper methods
bool VulkanHooks::installHook(void** originalFunction, void* hookFunction, const char* functionName) {
    if (MH_CreateHook(originalFunction, hookFunction, nullptr) != MH_OK) {
        m_lastError = "Failed to create hook for " + std::string(functionName);
        return false;
    }
    
    if (MH_EnableHook(originalFunction) != MH_OK) {
        m_lastError = "Failed to enable hook for " + std::string(functionName);
        return false;
    }
    
    std::cout << "[VulkanHooks] Hook installed for " << functionName << std::endl;
    return true;
}

void VulkanHooks::logError(const std::string& error) {
    m_lastError = error;
    std::cerr << "[VulkanHooks] ERROR: " << error << std::endl;
}

void VulkanHooks::updatePerformanceMetrics() {
    // Update average frame time
    static auto lastFrameTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto frameTime = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - lastFrameTime);
    
    if (m_performanceMetrics.frameCount > 0) {
        m_performanceMetrics.averageFrameTime = 
            (m_performanceMetrics.averageFrameTime * (m_performanceMetrics.frameCount - 1) + frameTime.count()) / 
            m_performanceMetrics.frameCount;
    }
    
    lastFrameTime = currentTime;
}

bool VulkanHooks::findMatrixInBuffer(const void* bufferData, VkDeviceSize bufferSize) {
    // Simplified matrix detection
    // In reality, you would scan for matrix patterns in the buffer
    return bufferSize >= 128; // Minimum size for view + projection matrices
}

void VulkanHooks::applyVRTransformation() {
    if (!m_vrState.stereoEnabled) {
        return;
    }
    
    // Apply IPD offset for right eye
    float ipdOffset = m_vrState.ipd * 0.5f;
    
    // Modify right eye view matrix to include IPD offset
    // This is a simplified transformation - in reality you'd use proper matrix math
    m_vrState.rightEye.viewMatrix[12] += ipdOffset; // X translation
    
    std::cout << "[VulkanHooks] Applied VR transformation (IPD: " << m_vrState.ipd << "m)" << std::endl;
}

} // namespace uevr
