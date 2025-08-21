#pragma once

#include <memory>
#include <vector>
#include <string>
#include <functional>
#include <chrono>
#include <atomic>
#include <mutex>

namespace uevrLCX {

enum class RenderAPI {
    OPENGL,
    DIRECTX11,
    DIRECTX12,
    VULKAN,
    METAL
};

enum class RenderMode {
    STEREO,
    MONO,
    SIDE_BY_SIDE,
    OVER_UNDER
};

struct RenderTarget {
    void* leftEye;
    void* rightEye;
    uint32_t width;
    uint32_t height;
    RenderAPI api;
};

struct RenderStats {
    double frameTime;
    double gpuTime;
    double cpuTime;
    uint32_t drawCalls;
    uint32_t triangles;
    uint32_t vertices;
    double fps;
};

class VRRenderer {
public:
    VRRenderer();
    ~VRRenderer();

    // Core functionality
    bool initialize(RenderAPI api);
    void shutdown();
    void update();
    
    // Rendering
    bool beginFrame();
    bool endFrame();
    bool submitFrame();
    
    // Render targets
    bool setRenderTarget(const RenderTarget& target);
    bool setDepthBuffer(void* depthBuffer);
    bool setStencilBuffer(void* stencilBuffer);
    
    // Shader management
    bool loadShader(const std::string& vertexShader, const std::string& fragmentShader);
    bool useShader(const std::string& shaderName);
    bool setShaderUniform(const std::string& name, const void* data, size_t size);
    
    // Geometry
    bool uploadMesh(const std::string& name, const void* vertices, size_t vertexCount, 
                   const void* indices, size_t indexCount);
    bool renderMesh(const std::string& name);
    
    // Textures
    bool uploadTexture(const std::string& name, const void* data, uint32_t width, 
                      uint32_t height, uint32_t format);
    bool bindTexture(const std::string& name, uint32_t slot);
    
    // Performance
    RenderStats getRenderStats() const;
    void resetRenderStats();
    bool setPerformanceLevel(int level);
    int getPerformanceLevel() const;
    
    // Configuration
    bool setRenderMode(RenderMode mode);
    RenderMode getRenderMode() const;
    bool setResolution(uint32_t width, uint32_t height);
    bool setRefreshRate(float refreshRate);
    
    // Error handling
    void setErrorHandler(std::function<void(const std::string&, const std::string&)> handler);
    void reportError(const std::string& component, const std::string& error);

private:
    // Core members
    std::atomic<bool> initialized;
    RenderAPI currentAPI;
    RenderMode renderMode;
    RenderTarget currentTarget;
    RenderStats renderStats;
    int performanceLevel;
    
    // Shader management
    std::unordered_map<std::string, void*> shaders;
    std::string currentShader;
    
    // Geometry management
    std::unordered_map<std::string, void*> meshes;
    
    // Texture management
    std::unordered_map<std::string, void*> textures;
    
    // Performance tracking
    std::chrono::high_resolution_clock::time_point frameStart;
    std::chrono::high_resolution_clock::time_point frameEnd;
    
    // Callbacks
    std::function<void(const std::string&, const std::string&)> errorHandler;
    
    // Thread safety
    mutable std::mutex renderMutex;
    
    // Internal methods
    bool initializeOpenGL();
    bool initializeDirectX11();
    bool initializeDirectX12();
    bool initializeVulkan();
    bool initializeMetal();
    
    void updateRenderStats();
    void resetFrameStats();
};

} // namespace uevrLCX
