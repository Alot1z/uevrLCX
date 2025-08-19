#pragma once

#include <memory>
#include <vulkan/vulkan.h>
#include <vector>
#include <unordered_map>
#include <functional>

namespace uevr {

/**
 * @brief Vulkan Hooks System for Cross-Engine VR Support
 * 
 * Provides comprehensive Vulkan API hooking for:
 * - Swapchain creation and presentation
 * - Command buffer recording and submission
 * - Matrix extraction from uniform buffers
 * - VR stereo rendering support
 */
class VulkanHooks {
public:
    using SwapchainCreateInfoHook = std::function<void(VkSwapchainCreateInfoKHR&)>;
    using QueuePresentHook = std::function<bool(VkQueue, const VkPresentInfoKHR*)>;
    using CmdBeginRenderPassHook = std::function<void(VkCommandBuffer, const VkRenderPassBeginInfo*)>;
    using CmdDrawHook = std::function<void(VkCommandBuffer, uint32_t, uint32_t, uint32_t, uint32_t)>;
    
    VulkanHooks();
    ~VulkanHooks();
    
    // Initialize Vulkan hooks
    bool initializeHooks();
    
    // Cleanup hooks
    void cleanupHooks();
    
    // Hook registration
    void registerSwapchainCreateHook(SwapchainCreateInfoHook hook);
    void registerQueuePresentHook(QueuePresentHook hook);
    void registerCmdBeginRenderPassHook(CmdBeginRenderPassHook hook);
    void registerCmdDrawHook(CmdDrawHook hook);
    
    // Matrix extraction
    bool extractViewProjectionMatrices(VkBuffer uniformBuffer, VkDeviceSize offset, VkDeviceSize size);
    
    // VR rendering support
    bool enableVRStereoRendering();
    bool disableVRStereoRendering();
    
    // Performance monitoring
    struct PerformanceMetrics {
        uint64_t frameCount;
        uint64_t drawCallCount;
        double averageFrameTime;
        double averageDrawTime;
        VkDeviceSize totalMemoryUsage;
    };
    
    PerformanceMetrics getPerformanceMetrics() const;
    
    // Hook state management
    bool isHooked() const { return m_isHooked; }
    bool isVREnabled() const { return m_vrEnabled; }
    
    // Error handling
    std::string getLastError() const { return m_lastError; }
    void clearErrors();

private:
    // Hook state
    bool m_isHooked;
    bool m_vrEnabled;
    std::string m_lastError;
    
    // Performance tracking
    PerformanceMetrics m_performanceMetrics;
    
    // Hook callbacks
    std::vector<SwapchainCreateInfoHook> m_swapchainCreateHooks;
    std::vector<QueuePresentHook> m_queuePresentHooks;
    std::vector<CmdBeginRenderPassHook> m_cmdBeginRenderPassHooks;
    std::vector<CmdDrawHook> m_cmdDrawHooks;
    
    // Original function pointers
    struct OriginalFunctions {
        PFN_vkCreateSwapchainKHR vkCreateSwapchainKHR;
        PFN_vkQueuePresentKHR vkQueuePresentKHR;
        PFN_vkCmdBeginRenderPass vkCmdBeginRenderPass;
        PFN_vkCmdDraw vkCmdDraw;
    } m_originalFunctions;
    
    // Hook implementations
    static VkResult VKAPI_CALL CreateSwapchainKHR_Hook(
        VkDevice device,
        const VkSwapchainCreateInfoKHR* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkSwapchainKHR* pSwapchain
    );
    
    static VkResult VKAPI_CALL QueuePresentKHR_Hook(
        VkQueue queue,
        const VkPresentInfoKHR* pPresentInfo
    );
    
    static void VKAPI_CALL CmdBeginRenderPass_Hook(
        VkCommandBuffer commandBuffer,
        const VkRenderPassBeginInfo* pRenderPassBegin,
        VkSubpassContents contents
    );
    
    static void VKAPI_CALL CmdDraw_Hook(
        VkCommandBuffer commandBuffer,
        uint32_t vertexCount,
        uint32_t instanceCount,
        uint32_t firstVertex,
        uint32_t firstInstance
    );
    
    // Helper functions
    bool installHook(void** originalFunction, void* hookFunction, const char* functionName);
    void logError(const std::string& error);
    void updatePerformanceMetrics();
    
    // Matrix extraction helpers
    bool findMatrixInBuffer(const void* bufferData, VkDeviceSize bufferSize);
    void applyVRTransformation();
    
    // VR state
    struct VRState {
        bool stereoEnabled;
        float ipd;
        float nearPlane;
        float farPlane;
        struct {
            float viewMatrix[16];
            float projectionMatrix[16];
        } leftEye, rightEye;
    } m_vrState;
};

} // namespace uevr
