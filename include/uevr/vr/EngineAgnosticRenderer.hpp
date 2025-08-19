#pragma once

#include <memory>
#include <vector>
#include <string>
#include <map>
#include <functional>

namespace uevr {
namespace vr {

/**
 * @brief Engine-agnostic VR renderer that works across graphics APIs and game engines
 * 
 * This class provides a unified rendering interface for VR that can adapt to
 * different graphics APIs (D3D11, D3D12, OpenGL, Vulkan) and game engines.
 * It handles stereo rendering, distortion correction, and performance optimization.
 */
class EngineAgnosticRenderer {
public:
    /**
     * @brief Supported graphics APIs
     */
    enum class GraphicsAPI {
        DIRECTX11,
        DIRECTX12,
        OPENGL,
        VULKAN,
        UNKNOWN
    };

    /**
     * @brief VR rendering techniques
     */
    enum class RenderingTechnique {
        INSTANCED_STEREO,       // Single-pass stereo with instancing
        MULTIVIEW,              // Multiview rendering extension
        MULTI_RESOLUTION,       // Variable rate shading/foveated
        TRADITIONAL_STEREO,     // Separate passes for each eye
        SIMULTANEOUS_PROJECTION // Geometry shader stereo
    };

    /**
     * @brief Shader types for VR rendering
     */
    enum class ShaderType {
        VERTEX_SHADER,
        PIXEL_SHADER,
        GEOMETRY_SHADER,
        COMPUTE_SHADER,
        HULL_SHADER,
        DOMAIN_SHADER
    };

    /**
     * @brief Render target information
     */
    struct RenderTarget {
        void* texture_handle = nullptr;      // Graphics API specific texture
        void* render_target_view = nullptr;  // RTV/FBO handle
        void* depth_stencil_view = nullptr;  // DSV handle
        uint32_t width = 0;
        uint32_t height = 0;
        uint32_t format = 0;                 // API-specific format
        uint32_t sample_count = 1;
        bool is_srgb = false;
        bool has_depth = true;
    };

    /**
     * @brief Eye rendering data
     */
    struct EyeRenderData {
        RenderTarget render_target;
        std::vector<float> view_matrix;      // 4x4 view matrix
        std::vector<float> projection_matrix; // 4x4 projection matrix
        std::vector<float> viewport;         // x, y, width, height
        float ipd_offset = 0.0f;            // Inter-pupillary distance offset
        int eye_index = 0;                  // 0 = left, 1 = right
    };

    /**
     * @brief VR shader uniforms/constants
     */
    struct VRUniforms {
        std::vector<float> left_eye_view_matrix;
        std::vector<float> right_eye_view_matrix;
        std::vector<float> left_eye_projection_matrix;
        std::vector<float> right_eye_projection_matrix;
        float ipd = 0.064f;                 // Inter-pupillary distance
        float world_scale = 1.0f;
        float time = 0.0f;
        std::vector<float> hmd_pose_matrix;
        std::vector<float> controller_poses; // Up to 2 controllers
    };

    /**
     * @brief Rendering performance metrics
     */
    struct RenderMetrics {
        float gpu_time_ms = 0.0f;
        float cpu_time_ms = 0.0f;
        uint32_t draw_calls = 0;
        uint32_t triangles = 0;
        uint32_t vertices = 0;
        float fill_rate = 0.0f;
        float memory_usage_mb = 0.0f;
        bool vsync_enabled = false;
        float render_scale = 1.0f;
    };

    /**
     * @brief Distortion correction parameters
     */
    struct DistortionCorrection {
        bool enabled = true;
        float k1 = 0.0f;                    // Radial distortion coefficient
        float k2 = 0.0f;                    // Radial distortion coefficient
        float k3 = 0.0f;                    // Radial distortion coefficient
        float p1 = 0.0f;                    // Tangential distortion coefficient
        float p2 = 0.0f;                    // Tangential distortion coefficient
        std::vector<float> lens_center;     // Lens center offset
        float scale_factor = 1.0f;
    };

public:
    EngineAgnosticRenderer();
    ~EngineAgnosticRenderer();

    // Disable copy constructor and assignment
    EngineAgnosticRenderer(const EngineAgnosticRenderer&) = delete;
    EngineAgnosticRenderer& operator=(const EngineAgnosticRenderer&) = delete;

    /**
     * @brief Initialize the renderer with graphics API
     * @param api Graphics API to use
     * @param device Graphics device handle
     * @param context Graphics context handle
     * @return true if initialization successful
     */
    bool initialize(GraphicsAPI api, void* device, void* context = nullptr);

    /**
     * @brief Shutdown the renderer
     */
    void shutdown();

    /**
     * @brief Check if renderer is initialized
     * @return true if ready for rendering
     */
    bool isInitialized() const;

    /**
     * @brief Create VR render targets for both eyes
     * @param width Render target width
     * @param height Render target height
     * @param format Texture format (API-specific)
     * @param sample_count MSAA sample count
     * @return true if render targets created successfully
     */
    bool createRenderTargets(uint32_t width, uint32_t height, uint32_t format, uint32_t sample_count = 1);

    /**
     * @brief Destroy VR render targets
     */
    void destroyRenderTargets();

    /**
     * @brief Get render target for specific eye
     * @param eye_index Eye index (0 = left, 1 = right)
     * @return Render target information
     */
    RenderTarget getRenderTarget(int eye_index) const;

    /**
     * @brief Begin VR frame rendering
     * @param frame_index Current frame index
     * @return true if frame setup successful
     */
    bool beginFrame(uint64_t frame_index);

    /**
     * @brief Begin rendering for specific eye
     * @param eye_index Eye index (0 = left, 1 = right)
     * @param eye_data Eye rendering data
     * @return true if eye setup successful
     */
    bool beginEyeRender(int eye_index, const EyeRenderData& eye_data);

    /**
     * @brief End rendering for specific eye
     * @param eye_index Eye index (0 = left, 1 = right)
     * @return true if eye rendering completed
     */
    bool endEyeRender(int eye_index);

    /**
     * @brief End VR frame rendering
     * @return true if frame completed successfully
     */
    bool endFrame();

    /**
     * @brief Set VR uniforms/constants for shaders
     * @param uniforms VR uniform data
     * @return true if uniforms set successfully
     */
    bool setVRUniforms(const VRUniforms& uniforms);

    /**
     * @brief Update VR shader constants
     * @param constant_name Name of the constant
     * @param data Constant data
     * @param size Data size in bytes
     * @return true if constant updated successfully
     */
    bool updateShaderConstant(const std::string& constant_name, const void* data, size_t size);

    /**
     * @brief Apply distortion correction
     * @param correction Distortion parameters
     * @return true if distortion applied successfully
     */
    bool applyDistortionCorrection(const DistortionCorrection& correction);

    /**
     * @brief Set rendering technique
     * @param technique VR rendering technique to use
     * @return true if technique set successfully
     */
    bool setRenderingTechnique(RenderingTechnique technique);

    /**
     * @brief Get current rendering technique
     * @return Current VR rendering technique
     */
    RenderingTechnique getRenderingTechnique() const;

    /**
     * @brief Check if technique is supported
     * @param technique Technique to check
     * @return true if technique is supported
     */
    bool isTechniqueSupported(RenderingTechnique technique) const;

    /**
     * @brief Enable/disable foveated rendering
     * @param enable true to enable foveated rendering
     * @param inner_radius Inner radius for full resolution
     * @param outer_radius Outer radius for minimum resolution
     * @return true if foveated rendering configured successfully
     */
    bool setFoveatedRendering(bool enable, float inner_radius = 0.3f, float outer_radius = 0.8f);

    /**
     * @brief Set render scale for performance adjustment
     * @param scale Render scale factor (0.5 - 2.0)
     * @return true if render scale applied successfully
     */
    bool setRenderScale(float scale);

    /**
     * @brief Get current render scale
     * @return Current render scale factor
     */
    float getRenderScale() const;

    /**
     * @brief Enable/disable multi-sampling anti-aliasing
     * @param enable true to enable MSAA
     * @param sample_count Number of samples (2, 4, 8, etc.)
     * @return true if MSAA configured successfully
     */
    bool setMSAA(bool enable, uint32_t sample_count = 4);

    /**
     * @brief Compile VR-specific shader
     * @param type Shader type
     * @param source Shader source code
     * @param entry_point Entry point function name
     * @param defines Preprocessor defines
     * @return Compiled shader handle (API-specific)
     */
    void* compileShader(ShaderType type, const std::string& source, 
                       const std::string& entry_point = "main",
                       const std::map<std::string, std::string>& defines = {});

    /**
     * @brief Set active shader
     * @param type Shader type
     * @param shader_handle Compiled shader handle
     * @return true if shader set successfully
     */
    bool setShader(ShaderType type, void* shader_handle);

    /**
     * @brief Create and bind VR constant buffer
     * @param slot Buffer slot index
     * @param size Buffer size in bytes
     * @param data Initial data (optional)
     * @return Constant buffer handle
     */
    void* createConstantBuffer(uint32_t slot, size_t size, const void* data = nullptr);

    /**
     * @brief Update constant buffer data
     * @param buffer_handle Buffer handle
     * @param data New data
     * @param size Data size in bytes
     * @return true if buffer updated successfully
     */
    bool updateConstantBuffer(void* buffer_handle, const void* data, size_t size);

    /**
     * @brief Bind texture for VR rendering
     * @param slot Texture slot index
     * @param texture_handle Texture handle
     * @param sampler_handle Sampler handle (optional)
     * @return true if texture bound successfully
     */
    bool bindTexture(uint32_t slot, void* texture_handle, void* sampler_handle = nullptr);

    /**
     * @brief Create render state for VR
     * @param depth_test Enable depth testing
     * @param depth_write Enable depth writing
     * @param alpha_blend Enable alpha blending
     * @param cull_mode Culling mode (0=none, 1=front, 2=back)
     * @return Render state handle
     */
    void* createRenderState(bool depth_test = true, bool depth_write = true, 
                           bool alpha_blend = false, int cull_mode = 2);

    /**
     * @brief Set active render state
     * @param state_handle Render state handle
     * @return true if render state set successfully
     */
    bool setRenderState(void* state_handle);

    /**
     * @brief Submit geometry for VR rendering
     * @param vertex_buffer Vertex buffer handle
     * @param index_buffer Index buffer handle (optional)
     * @param vertex_count Number of vertices
     * @param index_count Number of indices
     * @param primitive_topology Primitive topology
     * @return true if geometry submitted successfully
     */
    bool submitGeometry(void* vertex_buffer, void* index_buffer, 
                       uint32_t vertex_count, uint32_t index_count = 0,
                       uint32_t primitive_topology = 4); // Triangle list

    /**
     * @brief Get rendering performance metrics
     * @return Current performance metrics
     */
    RenderMetrics getMetrics() const;

    /**
     * @brief Reset performance metrics
     */
    void resetMetrics();

    /**
     * @brief Register graphics API adapter callback
     * @param api_name Graphics API name
     * @param callback Adapter function
     */
    void registerAPIAdapter(const std::string& api_name, 
                           std::function<bool(void*, void*)> callback);

    /**
     * @brief Get graphics API capabilities
     * @return Map of capability names to support status
     */
    std::map<std::string, bool> getCapabilities() const;

    /**
     * @brief Set debug mode for rendering
     * @param enable true to enable debug rendering
     */
    void setDebugMode(bool enable);

    /**
     * @brief Export rendering statistics
     * @param format Export format ("json", "csv")
     * @return Exported statistics data
     */
    std::string exportStatistics(const std::string& format = "json") const;

    /**
     * @brief Optimize shaders for VR rendering
     * @param optimization_level Optimization level (1-3)
     * @return true if optimization applied
     */
    bool optimizeShaders(int optimization_level = 2);

private:
    class Impl;
    std::unique_ptr<Impl> m_impl;

    // Internal rendering methods
    bool initializeD3D11(void* device, void* context);
    bool initializeD3D12(void* device, void* context);
    bool initializeOpenGL(void* device, void* context);
    bool initializeVulkan(void* device, void* context);
    
    void shutdownD3D11();
    void shutdownD3D12();
    void shutdownOpenGL();
    void shutdownVulkan();
    
    // VR-specific rendering
    bool setupStereoRenderPipeline();
    void applyStereoTransforms(int eye_index, const EyeRenderData& eye_data);
    void bindVRResources();
    void unbindVRResources();
    
    // Performance optimization
    void updatePerformanceCounters();
    bool shouldUseFoveatedRendering() const;
    float calculateOptimalRenderScale() const;
    
    // Shader management
    std::string generateVRShaderDefines() const;
    void injectVRShaderCode(std::string& shader_source) const;
    bool compileShaderForAPI(ShaderType type, const std::string& source, void** out_shader);
    
    // Resource management
    void* createAPIResource(const std::string& resource_type, const std::map<std::string, void*>& params);
    void destroyAPIResource(void* resource);
    bool validateAPIState() const;
};

} // namespace vr
} // namespace uevr