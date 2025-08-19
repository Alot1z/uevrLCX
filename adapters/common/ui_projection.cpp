/*
 * Cross-Engine UI Projection System Implementation
 * 
 * Implementation of engine-specific UI projection and HUD rendering
 * for all cross-engine VR adapters.
 * 
 * Copyright (c) 2024 UEVR Cross-Engine Adapter Project
 * Licensed under MIT License
 */

#include "ui_projection.hpp"
#include <spdlog/spdlog.h>
#include <algorithm>

namespace uevr_common {

// UIProjectionManager Implementation
UIProjectionManager::UIProjectionManager() {
    setup_ui_rendering_pipeline();
    spdlog::info("[UIProjection] UI projection manager initialized");
}

UIProjectionManager::~UIProjectionManager() {
    cleanup_ui_rendering_pipeline();
    spdlog::info("[UIProjection] UI projection manager destroyed");
}

void UIProjectionManager::set_ui_projection_config(const UIProjectionConfig& config) {
    m_config = config;
    spdlog::info("[UIProjection] UI projection configuration updated");
}

const UIProjectionManager::UIProjectionConfig& UIProjectionManager::get_ui_projection_config() const {
    return m_config;
}

void UIProjectionManager::add_hud_element(const HUDElement& element) {
    // Check if element already exists
    auto it = m_hud_element_map.find(element.name);
    if (it != m_hud_element_map.end()) {
        // Update existing element
        m_hud_elements[it->second] = element;
        spdlog::debug("[UIProjection] Updated HUD element: {}", element.name);
    } else {
        // Add new element
        m_hud_element_map[element.name] = m_hud_elements.size();
        m_hud_elements.push_back(element);
        spdlog::debug("[UIProjection] Added HUD element: {} ({})", element.name, element.type);
    }
}

void UIProjectionManager::remove_hud_element(const std::string& name) {
    auto it = m_hud_element_map.find(name);
    if (it != m_hud_element_map.end()) {
        size_t index = it->second;
        m_hud_elements.erase(m_hud_elements.begin() + index);
        m_hud_element_map.erase(it);
        
        // Update indices for remaining elements
        for (auto& pair : m_hud_element_map) {
            if (pair.second > index) {
                pair.second--;
            }
        }
        
        spdlog::debug("[UIProjection] Removed HUD element: {}", name);
    }
}

void UIProjectionManager::update_hud_element(const std::string& name, const HUDElement& element) {
    auto it = m_hud_element_map.find(name);
    if (it != m_hud_element_map.end()) {
        m_hud_elements[it->second] = element;
        spdlog::debug("[UIProjection] Updated HUD element: {}", name);
    }
}

const UIProjectionManager::HUDElement* UIProjectionManager::get_hud_element(const std::string& name) const {
    auto it = m_hud_element_map.find(name);
    if (it != m_hud_element_map.end()) {
        return &m_hud_elements[it->second];
    }
    return nullptr;
}

void UIProjectionManager::set_ui_rendering_state(const UIRenderingState& state) {
    m_state = state;
    spdlog::info("[UIProjection] UI rendering state updated");
}

const UIProjectionManager::UIRenderingState& UIProjectionManager::get_ui_rendering_state() const {
    return m_state;
}

DirectX::XMMATRIX UIProjectionManager::calculate_projection_matrix(const DirectX::XMFLOAT3& camera_position, 
                                                                 const DirectX::XMFLOAT3& camera_direction) const {
    DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&camera_position);
    DirectX::XMVECTOR dir = DirectX::XMLoadFloat3(&camera_direction);
    
    // Calculate UI position based on camera
    DirectX::XMVECTOR ui_pos = pos + dir * m_config.distance;
    
    // Create projection matrix based on type
    switch (m_config.type) {
        case UIProjectionConfig::ProjectionType::Billboard:
            return create_billboard_matrix(DirectX::XMFLOAT3{}, camera_position);
        case UIProjectionConfig::ProjectionType::Cylindrical:
            return create_cylindrical_matrix(DirectX::XMFLOAT3{}, camera_position);
        case UIProjectionConfig::ProjectionType::Spherical:
            return create_spherical_matrix(DirectX::XMFLOAT3{}, camera_position);
        case UIProjectionConfig::ProjectionType::Fixed:
            return DirectX::XMMatrixTranslation(m_config.offset.x, m_config.offset.y, m_config.offset.z);
        case UIProjectionConfig::ProjectionType::FollowView:
            return DirectX::XMMatrixTranslation(ui_pos.m128_f32[0], ui_pos.m128_f32[1], ui_pos.m128_f32[2]);
        default:
            return DirectX::XMMatrixIdentity();
    }
}

DirectX::XMFLOAT3 UIProjectionManager::calculate_ui_position(const DirectX::XMFLOAT3& world_position,
                                                           const DirectX::XMFLOAT3& camera_position) const {
    DirectX::XMVECTOR world_pos = DirectX::XMLoadFloat3(&world_position);
    DirectX::XMVECTOR cam_pos = DirectX::XMLoadFloat3(&camera_position);
    
    // Calculate direction from camera to world position
    DirectX::XMVECTOR direction = DirectX::XMVector3Normalize(world_pos - cam_pos);
    
    // Apply distance and offset
    DirectX::XMVECTOR ui_pos = cam_pos + direction * m_config.distance;
    ui_pos += DirectX::XMLoadFloat3(&m_config.offset);
    
    DirectX::XMFLOAT3 result;
    DirectX::XMStoreFloat3(&result, ui_pos);
    return result;
}

void UIProjectionManager::render_hud_elements(ID3D11DeviceContext* context, ID3D11Texture2D* render_target) {
    if (!m_state.hud_enabled || !context || !render_target) {
        return;
    }
    
    spdlog::debug("[UIProjection] Rendering {} HUD elements (D3D11)", m_hud_elements.size());
    
    // Save current render state
    ID3D11RenderTargetView* prev_rtv = nullptr;
    ID3D11DepthStencilView* prev_dsv = nullptr;
    context->OMGetRenderTargets(1, &prev_rtv, &prev_dsv);
    

void UIProjectionManager::render_hud_elements_d3d12(ID3D12GraphicsCommandList* command_list, ID3D12Resource* render_target) {
    if (!m_state.hud_enabled || !command_list || !render_target) {
        return;
    }
    
    spdlog::debug("[UIProjection] Rendering {} HUD elements (D3D12)", m_hud_elements.size());
    
    // Set up ImGui for D3D12 rendering
    ImGui::NewFrame();

void UIProjectionManager::adapt_ui_for_engine(const std::string& engine_name, const std::string& game_name) {
    spdlog::info("[UIProjection] Adapting UI for engine: {} - {}", engine_name, game_name);
    
    // Set default HUD elements based on engine
    if (engine_name == "RE Engine") {
        // RE Engine specific HUD elements
        add_hud_element({"health_bar", "progress_bar", {0.1f, 0.9f}, {0.2f, 0.05f}, true, false, "", {"health"}});
        add_hud_element({"ammo_counter", "text", {0.9f, 0.9f}, {0.1f, 0.05f}, true, false, "", {"ammo"}});
        add_hud_element({"inventory", "grid", {0.5f, 0.5f}, {0.4f, 0.4f}, false, true, "", {"items"}});
    } else if (engine_name == "REDengine 4") {
        // REDengine 4 specific HUD elements
        add_hud_element({"health_bar", "progress_bar", {0.1f, 0.9f}, {0.2f, 0.05f}, true, false, "", {"health"}});
        add_hud_element({"stamina_bar", "progress_bar", {0.1f, 0.85f}, {0.2f, 0.03f}, true, false, "", {"stamina"}});
        add_hud_element({"minimap", "overlay", {0.8f, 0.1f}, {0.18f, 0.18f}, true, false, "", {"map"}});
        add_hud_element({"quest_log", "list", {0.7f, 0.3f}, {0.28f, 0.4f}, false, true, "", {"quests"}});
    } else if (engine_name == "MT Framework") {
        // MT Framework specific HUD elements
        add_hud_element({"health_bar", "progress_bar", {0.1f, 0.9f}, {0.2f, 0.05f}, true, false, "", {"health"}});
        add_hud_element({"stamina_bar", "progress_bar", {0.1f, 0.85f}, {0.2f, 0.03f}, true, false, "", {"stamina"}});
        add_hud_element({"sharpness_meter", "progress_bar", {0.1f, 0.8f}, {0.2f, 0.03f}, true, false, "", {"sharpness"}});
        add_hud_element({"monster_info", "panel", {0.7f, 0.1f}, {0.28f, 0.2f}, true, false, "", {"monster"}});
        add_hud_element({"item_bar", "grid", {0.5f, 0.9f}, {0.3f, 0.08f}, true, true, "", {"items"}});
    }
    
    optimize_ui_for_vr();
}

void UIProjectionManager::optimize_ui_for_vr() {
    spdlog::info("[UIProjection] Optimizing UI for VR");
    
    // Adjust UI projection for VR comfort
    m_config.distance = 2.0f; // Optimal distance for VR
    m_config.scale = 1.2f;    // Slightly larger for VR readability
    
    // Enable alpha blending for better VR integration
    m_config.enable_alpha_blending = true;
    m_config.enable_depth_test = false; // UI should always be on top
    
    // Set billboard projection for best VR readability
    m_config.type = UIProjectionConfig::ProjectionType::Billboard;
    
    spdlog::info("[UIProjection] VR optimization applied: distance={}, scale={}, type=Billboard", 
                 m_config.distance, m_config.scale);
}

void UIProjectionManager::setup_ui_rendering_pipeline() {
    spdlog::debug("[UIProjection] Setting up UI rendering pipeline");
    
    try {
        // Get the current renderer type from Framework
        auto* framework = Framework::get();
        if (!framework) {
            spdlog::error("[UIProjection] Failed to get Framework instance");
            return;
        }
        
        auto renderer_type = framework->get_renderer_type();
        
        if (renderer_type == Framework::RendererType::D3D11) {
            setup_d3d11_ui_pipeline();
        } else if (renderer_type == Framework::RendererType::D3D12) {
            setup_d3d12_ui_pipeline();
        } else {
            spdlog::error("[UIProjection] Unsupported renderer type");
            return;
        }
        
        // Initialize common UI resources with comprehensive error handling
        if (!initialize_ui_shaders()) {
            throw std::runtime_error("Failed to initialize UI shaders");
        }
        
        if (!create_ui_vertex_buffers()) {
            throw std::runtime_error("Failed to create UI vertex buffers");
        }
        
        if (!setup_ui_render_states()) {
            throw std::runtime_error("Failed to setup UI render states");
        }
        
        if (!create_ui_texture_samplers()) {
            throw std::runtime_error("Failed to create UI texture samplers");
        }
        
        if (!initialize_ui_constant_buffers()) {
            throw std::runtime_error("Failed to initialize UI constant buffers");
        }
        
        // Setup UI projection matrices with VR optimization
        update_projection_matrices();
        
        // Initialize UI element textures and resources with game-specific loading
        if (!load_ui_element_textures()) {
            spdlog::warn("[UIProjection] Failed to load some UI textures, using defaults");
        }
        
        // Setup ImGui integration with theme support
        if (!setup_imgui_integration()) {
            throw std::runtime_error("Failed to setup ImGui integration");
        }
        
        // Initialize VR-specific UI components if VR is enabled
        if (framework->is_vr_enabled()) {
            if (!initialize_vr_ui_components()) {
                spdlog::warn("[UIProjection] Failed to initialize VR UI components");
            }
        }
        
        // Setup UI performance monitoring
        setup_ui_performance_monitoring();
        
        // Initialize UI resource management system
        initialize_ui_resource_manager();
        
        // Setup UI debugging and profiling tools
        setup_ui_debugging_tools();
        
        m_pipeline_initialized = true;
        spdlog::info("[UIProjection] UI rendering pipeline successfully initialized with full feature set");
        
    } catch (const std::exception& e) {
        spdlog::error("[UIProjection] Failed to setup UI rendering pipeline: {}", e.what());
        m_pipeline_initialized = false;
        
        // Attempt recovery by cleaning up partial initialization
        try {
            cleanup_partial_ui_pipeline();
        } catch (const std::exception& cleanup_error) {
            spdlog::error("[UIProjection] Failed to cleanup partial pipeline: {}", cleanup_error.what());
        }
        
        // Notify framework of UI pipeline failure
        auto* framework = Framework::get();
        if (framework) {
            framework->notify_system_error("UI_PIPELINE_INIT_FAILED", e.what());
        }
    }
}

void UIProjectionManager::setup_d3d11_ui_pipeline() {
    spdlog::debug("[UIProjection] Setting up D3D11 UI pipeline");
    
    auto* framework = Framework::get();
    auto device = framework->get_d3d11_device();
    auto context = framework->get_d3d11_device_context();
    
    if (!device || !context) {
        throw std::runtime_error("Failed to get D3D11 device or context");
    }
    
    // Create UI vertex shader
    const char* vs_source = R"(
        cbuffer UIConstants : register(b0) {
            matrix worldViewProj;
            float4 color;
            float2 texScale;
            float2 texOffset;
        };
        
        struct VSInput {
            float3 position : POSITION;
            float2 texcoord : TEXCOORD0;
        };
        
        struct VSOutput {
            float4 position : SV_POSITION;
            float2 texcoord : TEXCOORD0;
            float4 color : COLOR0;
        };
        
        VSOutput main(VSInput input) {
            VSOutput output;
            output.position = mul(float4(input.position, 1.0f), worldViewProj);
            output.texcoord = input.texcoord * texScale + texOffset;
            output.color = color;
            return output;
        }
    )";
    
    // Create UI pixel shader
    const char* ps_source = R"(
        Texture2D uiTexture : register(t0);
        SamplerState uiSampler : register(s0);
        
        struct PSInput {
            float4 position : SV_POSITION;
            float2 texcoord : TEXCOORD0;
            float4 color : COLOR0;
        };
        
        float4 main(PSInput input) : SV_TARGET {
            float4 texColor = uiTexture.Sample(uiSampler, input.texcoord);
            return texColor * input.color;
        }
    )";
    
    // Compile and create shaders
    Microsoft::WRL::ComPtr<ID3DBlob> vs_blob, ps_blob, error_blob;
    
    HRESULT hr = D3DCompile(vs_source, strlen(vs_source), nullptr, nullptr, nullptr,
                           "main", "vs_5_0", 0, 0, &vs_blob, &error_blob);
    if (FAILED(hr)) {
        if (error_blob) {
            spdlog::error("[UIProjection] VS compilation error: {}", 
                         static_cast<char*>(error_blob->GetBufferPointer()));
        }
        throw std::runtime_error("Failed to compile UI vertex shader");
    }
    
    hr = D3DCompile(ps_source, strlen(ps_source), nullptr, nullptr, nullptr,
                   "main", "ps_5_0", 0, 0, &ps_blob, &error_blob);
    if (FAILED(hr)) {
        if (error_blob) {
            spdlog::error("[UIProjection] PS compilation error: {}", 
                         static_cast<char*>(error_blob->GetBufferPointer()));
        }
        throw std::runtime_error("Failed to compile UI pixel shader");
    }
    
    hr = device->CreateVertexShader(vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(),
                                   nullptr, &m_d3d11_vertex_shader);
    if (FAILED(hr)) {
        throw std::runtime_error("Failed to create UI vertex shader");
    }
    
    hr = device->CreatePixelShader(ps_blob->GetBufferPointer(), ps_blob->GetBufferSize(),
                                  nullptr, &m_d3d11_pixel_shader);
    if (FAILED(hr)) {
        throw std::runtime_error("Failed to create UI pixel shader");
    }
    
    // Create input layout
    D3D11_INPUT_ELEMENT_DESC layout[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };
    
    hr = device->CreateInputLayout(layout, ARRAYSIZE(layout),
                                  vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(),
                                  &m_d3d11_input_layout);
    if (FAILED(hr)) {
        throw std::runtime_error("Failed to create UI input layout");
    }
}

void UIProjectionManager::setup_d3d12_ui_pipeline() {
    spdlog::debug("[UIProjection] Setting up D3D12 UI pipeline");
    
    auto* framework = Framework::get();
    auto device = framework->get_d3d12_device();
    
    if (!device) {
        throw std::runtime_error("Failed to get D3D12 device");
    }
    
    // Create root signature
    D3D12_ROOT_PARAMETER root_params[2] = {};
    
    // Constants
    root_params[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
    root_params[0].Constants.ShaderRegister = 0;
    root_params[0].Constants.RegisterSpace = 0;
    root_params[0].Constants.Num32BitValues = 20; // Matrix + color + tex params
    root_params[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
    
    // Descriptor table for texture
    D3D12_DESCRIPTOR_RANGE desc_range = {};
    desc_range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    desc_range.NumDescriptors = 1;
    desc_range.BaseShaderRegister = 0;
    desc_range.RegisterSpace = 0;
    desc_range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
    
    root_params[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    root_params[1].DescriptorTable.NumDescriptorRanges = 1;
    root_params[1].DescriptorTable.pDescriptorRanges = &desc_range;
    root_params[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    
    // Static sampler
    D3D12_STATIC_SAMPLER_DESC sampler = {};
    sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    sampler.MipLODBias = 0;
    sampler.MaxAnisotropy = 0;
    sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
    sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
    sampler.MinLOD = 0.0f;
    sampler.MaxLOD = D3D12_FLOAT32_MAX;
    sampler.ShaderRegister = 0;
    sampler.RegisterSpace = 0;
    sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    
    D3D12_ROOT_SIGNATURE_DESC root_sig_desc = {};
    root_sig_desc.NumParameters = ARRAYSIZE(root_params);
    root_sig_desc.pParameters = root_params;
    root_sig_desc.NumStaticSamplers = 1;
    root_sig_desc.pStaticSamplers = &sampler;
    root_sig_desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    
    Microsoft::WRL::ComPtr<ID3DBlob> signature_blob, error_blob;
    HRESULT hr = D3D12SerializeRootSignature(&root_sig_desc, D3D_ROOT_SIGNATURE_VERSION_1,
                                            &signature_blob, &error_blob);
    if (FAILED(hr)) {
        if (error_blob) {
            spdlog::error("[UIProjection] Root signature serialization error: {}", 
                         static_cast<char*>(error_blob->GetBufferPointer()));
        }
        throw std::runtime_error("Failed to serialize UI root signature");
    }
    
    hr = device->CreateRootSignature(0, signature_blob->GetBufferPointer(),
                                    signature_blob->GetBufferSize(),
                                    IID_PPV_ARGS(&m_d3d12_root_signature));
    if (FAILED(hr)) {
        throw std::runtime_error("Failed to create UI root signature");
    }
    
    // Create pipeline state (shaders compiled similarly to D3D11 version)
    // ... (PSO creation code would go here)
}

void UIProjectionManager::initialize_ui_shaders() {
    spdlog::debug("[UIProjection] Initializing UI shaders");
    
    // Shader initialization is handled in renderer-specific setup functions
    // This function can be used for common shader resource management
    
    m_shader_constants.world_view_proj = DirectX::XMMatrixIdentity();
    m_shader_constants.color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    m_shader_constants.tex_scale = DirectX::XMFLOAT2(1.0f, 1.0f);
    m_shader_constants.tex_offset = DirectX::XMFLOAT2(0.0f, 0.0f);
}

void UIProjectionManager::create_ui_vertex_buffers() {
    spdlog::debug("[UIProjection] Creating UI vertex buffers");
    
    // Create quad vertices for UI rendering
    UIVertex quad_vertices[] = {
        {{-0.5f, -0.5f, 0.0f}, {0.0f, 1.0f}}, // Bottom-left
        {{-0.5f,  0.5f, 0.0f}, {0.0f, 0.0f}}, // Top-left
        {{ 0.5f,  0.5f, 0.0f}, {1.0f, 0.0f}}, // Top-right
        {{ 0.5f, -0.5f, 0.0f}, {1.0f, 1.0f}}  // Bottom-right
    };
    
    uint16_t quad_indices[] = {
        0, 1, 2,
        0, 2, 3
    };
    
    auto* framework = Framework::get();
    auto renderer_type = framework->get_renderer_type();
    
    if (renderer_type == Framework::RendererType::D3D11) {
        auto device = framework->get_d3d11_device();
        
        D3D11_BUFFER_DESC vb_desc = {};
        vb_desc.Usage = D3D11_USAGE_DEFAULT;
        vb_desc.ByteWidth = sizeof(quad_vertices);
        vb_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        
        D3D11_SUBRESOURCE_DATA vb_data = {};
        vb_data.pSysMem = quad_vertices;
        
        HRESULT hr = device->CreateBuffer(&vb_desc, &vb_data, &m_d3d11_vertex_buffer);
        if (FAILED(hr)) {
            throw std::runtime_error("Failed to create UI vertex buffer");
        }
        
        D3D11_BUFFER_DESC ib_desc = {};
        ib_desc.Usage = D3D11_USAGE_DEFAULT;
        ib_desc.ByteWidth = sizeof(quad_indices);
        ib_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        
        D3D11_SUBRESOURCE_DATA ib_data = {};
        ib_data.pSysMem = quad_indices;
        
        hr = device->CreateBuffer(&ib_desc, &ib_data, &m_d3d11_index_buffer);
        if (FAILED(hr)) {
            throw std::runtime_error("Failed to create UI index buffer");
        }
    } else if (renderer_type == Framework::RendererType::D3D12) {
        // D3D12 buffer creation would go here
        // ... (D3D12 buffer creation code)
    }
}

void UIProjectionManager::setup_ui_render_states() {
    spdlog::debug("[UIProjection] Setting up UI render states");
    
    auto* framework = Framework::get();
    auto renderer_type = framework->get_renderer_type();
    
    if (renderer_type == Framework::RendererType::D3D11) {
        auto device = framework->get_d3d11_device();
        
        // Create blend state for UI alpha blending
        D3D11_BLEND_DESC blend_desc = {};
        blend_desc.RenderTarget[0].BlendEnable = TRUE;
        blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
        blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
        blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        
        HRESULT hr = device->CreateBlendState(&blend_desc, &m_d3d11_blend_state);
        if (FAILED(hr)) {
            throw std::runtime_error("Failed to create UI blend state");
        }
        
        // Create depth stencil state
        D3D11_DEPTH_STENCIL_DESC ds_desc = {};
        ds_desc.DepthEnable = m_config.enable_depth_test;
        ds_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        ds_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
        
        hr = device->CreateDepthStencilState(&ds_desc, &m_d3d11_depth_stencil_state);
        if (FAILED(hr)) {
            throw std::runtime_error("Failed to create UI depth stencil state");
        }
        
        // Create rasterizer state
        D3D11_RASTERIZER_DESC raster_desc = {};
        raster_desc.FillMode = D3D11_FILL_SOLID;
        raster_desc.CullMode = D3D11_CULL_NONE;
        raster_desc.FrontCounterClockwise = FALSE;
        raster_desc.DepthBias = 0;
        raster_desc.DepthBiasClamp = 0.0f;
        raster_desc.SlopeScaledDepthBias = 0.0f;
        raster_desc.DepthClipEnable = TRUE;
        raster_desc.ScissorEnable = FALSE;
        raster_desc.MultisampleEnable = FALSE;
        raster_desc.AntialiasedLineEnable = FALSE;
        
        hr = device->CreateRasterizerState(&raster_desc, &m_d3d11_rasterizer_state);
        if (FAILED(hr)) {
            throw std::runtime_error("Failed to create UI rasterizer state");
        }
    }
}

void UIProjectionManager::create_ui_texture_samplers() {
    spdlog::debug("[UIProjection] Creating UI texture samplers");
    
    auto* framework = Framework::get();
    auto renderer_type = framework->get_renderer_type();
    
    if (renderer_type == Framework::RendererType::D3D11) {
        auto device = framework->get_d3d11_device();
        
        D3D11_SAMPLER_DESC sampler_desc = {};
        sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        sampler_desc.MipLODBias = 0.0f;
        sampler_desc.MaxAnisotropy = 1;
        sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
        sampler_desc.BorderColor[0] = 0.0f;
        sampler_desc.BorderColor[1] = 0.0f;
        sampler_desc.BorderColor[2] = 0.0f;
        sampler_desc.BorderColor[3] = 0.0f;
        sampler_desc.MinLOD = 0.0f;
        sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
        
        HRESULT hr = device->CreateSamplerState(&sampler_desc, &m_d3d11_sampler_state);
        if (FAILED(hr)) {
            throw std::runtime_error("Failed to create UI sampler state");
        }
    }
}

void UIProjectionManager::initialize_ui_constant_buffers() {
    spdlog::debug("[UIProjection] Initializing UI constant buffers");
    
    auto* framework = Framework::get();
    auto renderer_type = framework->get_renderer_type();
    
    if (renderer_type == Framework::RendererType::D3D11) {
        auto device = framework->get_d3d11_device();
        
        D3D11_BUFFER_DESC cb_desc = {};
        cb_desc.Usage = D3D11_USAGE_DYNAMIC;
        cb_desc.ByteWidth = sizeof(UIShaderConstants);
        cb_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cb_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        
        HRESULT hr = device->CreateBuffer(&cb_desc, nullptr, &m_d3d11_constant_buffer);
        if (FAILED(hr)) {
            throw std::runtime_error("Failed to create UI constant buffer");
        }
    }
}

void UIProjectionManager::update_projection_matrices() {
    spdlog::debug("[UIProjection] Updating projection matrices");
    
    // Get camera information from VR system or game
    auto* framework = Framework::get();
    if (framework && framework->is_vr_enabled()) {
        // VR projection matrices - get from VR runtime
        auto vr_runtime = framework->get_vr_runtime();
        if (vr_runtime) {
            // Get eye projection matrices from VR system
            DirectX::XMMATRIX left_eye_proj, right_eye_proj;
            DirectX::XMMATRIX left_eye_view, right_eye_view;
            
            vr_runtime->get_eye_projection_matrix(0, &left_eye_proj);
            vr_runtime->get_eye_projection_matrix(1, &right_eye_proj);
            vr_runtime->get_eye_view_matrix(0, &left_eye_view);
            vr_runtime->get_eye_view_matrix(1, &right_eye_view);
            
            // Store matrices for stereo rendering
            m_left_eye_view_matrix = left_eye_view;
            m_right_eye_view_matrix = right_eye_view;
            m_left_eye_projection_matrix = left_eye_proj;
            m_right_eye_projection_matrix = right_eye_proj;
            
            // Use left eye as primary for UI positioning
            m_view_matrix = left_eye_view;
            m_projection_matrix = left_eye_proj;
            
            // Update VR-specific UI positioning
            update_vr_ui_positioning();
        } else {
            // Fallback VR matrices if runtime not available
            m_view_matrix = DirectX::XMMatrixIdentity();
            m_projection_matrix = DirectX::XMMatrixPerspectiveFovLH(
                DirectX::XMConvertToRadians(110.0f), 1.0f, 0.01f, 1000.0f);
        }
    } else {
        // Standard projection matrices for flat screen
        DirectX::XMFLOAT3 camera_pos = get_camera_position_from_game();
        DirectX::XMFLOAT3 camera_target = get_camera_target_from_game();
        DirectX::XMFLOAT3 camera_up = get_camera_up_from_game();
        
        m_view_matrix = DirectX::XMMatrixLookAtLH(
            DirectX::XMLoadFloat3(&camera_pos),
            DirectX::XMLoadFloat3(&camera_target),
            DirectX::XMLoadFloat3(&camera_up)
        );
        
        // Get viewport dimensions for aspect ratio
        auto viewport = get_current_viewport();
        float aspect_ratio = static_cast<float>(viewport.width) / static_cast<float>(viewport.height);
        
        m_projection_matrix = DirectX::XMMatrixPerspectiveFovLH(
            DirectX::XMConvertToRadians(m_config.fov), aspect_ratio, m_config.near_plane, m_config.far_plane);
    }
    
    // Update world-view-projection matrix for shaders
    m_world_view_proj_matrix = m_view_matrix * m_projection_matrix;
    
    // Mark matrices as dirty for next render
    m_matrices_dirty = true;
    
    spdlog::debug("[UIProjection] Projection matrices updated successfully");
}

void UIProjectionManager::load_ui_element_textures() {
    spdlog::debug("[UIProjection] Loading UI element textures");
    
    // Load default UI textures and game-specific UI elements
    auto* framework = Framework::get();
    auto renderer_type = framework->get_renderer_type();
    
    if (renderer_type == Framework::RendererType::D3D11) {
        auto device = framework->get_d3d11_device();
        
        // Create default white texture
        create_default_texture_d3d11(device);
        
        // Load UI element textures from game resources
        load_game_ui_textures_d3d11(device);
        
        // Create UI atlas texture for efficient rendering
        create_ui_atlas_texture_d3d11(device);
        
    } else if (renderer_type == Framework::RendererType::D3D12) {
        auto device = framework->get_d3d12_device();
        auto cmd_queue = framework->get_d3d12_command_queue();
        
        // Create default white texture
        create_default_texture_d3d12(device, cmd_queue);
        
        // Load UI element textures from game resources
        load_game_ui_textures_d3d12(device, cmd_queue);
        
        // Create UI atlas texture for efficient rendering
        create_ui_atlas_texture_d3d12(device, cmd_queue);
    }
    
    // Initialize texture streaming system for dynamic UI elements
    initialize_texture_streaming_system();
    
    spdlog::info("[UIProjection] UI element textures loaded successfully");
}

void UIProjectionManager::setup_imgui_integration() {
    spdlog::debug("[UIProjection] Setting up ImGui integration");
    
    // Initialize ImGui context for UI debugging and configuration
    if (!ImGui::GetCurrentContext()) {
        ImGui::CreateContext();
    }
    
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    
    // Setup ImGui style for VR/game integration
    setup_imgui_style_for_vr();
    
    // Initialize ImGui renderer backend
    auto* framework = Framework::get();
    auto renderer_type = framework->get_renderer_type();
    
    if (renderer_type == Framework::RendererType::D3D11) {
        auto device = framework->get_d3d11_device();
        auto context = framework->get_d3d11_context();
        
        if (!ImGui_ImplDX11_Init(device, context)) {
            throw std::runtime_error("Failed to initialize ImGui D3D11 backend");
        }
    } else if (renderer_type == Framework::RendererType::D3D12) {
        auto device = framework->get_d3d12_device();
        auto srv_heap = framework->get_d3d12_srv_heap();
        
        if (!ImGui_ImplDX12_Init(device, 3, DXGI_FORMAT_R8G8B8A8_UNORM, srv_heap,
                                srv_heap->GetCPUDescriptorHandleForHeapStart(),
                                srv_heap->GetGPUDescriptorHandleForHeapStart())) {
            throw std::runtime_error("Failed to initialize ImGui D3D12 backend");
        }
    }
    
    // Setup ImGui platform backend
    auto hwnd = framework->get_window_handle();
    if (!ImGui_ImplWin32_Init(hwnd)) {
        throw std::runtime_error("Failed to initialize ImGui Win32 backend");
    }
    
    // Register ImGui UI panels
    register_imgui_ui_panels();
    
    m_imgui_integration_enabled = true;
    
    spdlog::info("[UIProjection] ImGui integration setup completed");
}

void UIProjectionManager::cleanup_ui_rendering_pipeline() {
    spdlog::debug("[UIProjection] Cleaning up UI rendering pipeline");
    
    // Cleanup ImGui integration
    if (m_imgui_integration_enabled) {
        ImGui_ImplWin32_Shutdown();
        
        auto* framework = Framework::get();
        auto renderer_type = framework->get_renderer_type();
        
        if (renderer_type == Framework::RendererType::D3D11) {
            ImGui_ImplDX11_Shutdown();
        } else if (renderer_type == Framework::RendererType::D3D12) {
            ImGui_ImplDX12_Shutdown();
        }
        
        if (ImGui::GetCurrentContext()) {
            ImGui::DestroyContext();
        }
        
        m_imgui_integration_enabled = false;
    }
    
    // Cleanup D3D11 resources
    if (m_d3d11_vertex_shader) {
        m_d3d11_vertex_shader.Reset();
    }
    if (m_d3d11_pixel_shader) {
        m_d3d11_pixel_shader.Reset();
    }
    if (m_d3d11_input_layout) {
        m_d3d11_input_layout.Reset();
    }
    if (m_d3d11_vertex_buffer) {
        m_d3d11_vertex_buffer.Reset();
    }
    if (m_d3d11_index_buffer) {
        m_d3d11_index_buffer.Reset();
    }
    if (m_d3d11_constant_buffer) {
        m_d3d11_constant_buffer.Reset();
    }
    if (m_d3d11_sampler_state) {
        m_d3d11_sampler_state.Reset();
    }
    if (m_default_texture_srv) {
        m_default_texture_srv.Reset();
    }
    
    // Cleanup D3D12 resources
    if (m_d3d12_root_signature) {
        m_d3d12_root_signature.Reset();
    }
    if (m_d3d12_pipeline_state) {
        m_d3d12_pipeline_state.Reset();
    }
    
    // Cleanup texture streaming system
    cleanup_texture_streaming_system();
    
    // Clear UI element cache
    m_ui_elements.clear();
    m_ui_element_cache.clear();
    
    // Reset state
    m_initialized = false;
    m_matrices_dirty = false;
    
    spdlog::info("[UIProjection] UI rendering pipeline cleanup completed");
}

DirectX::XMMATRIX UIProjectionManager::create_billboard_matrix(const DirectX::XMFLOAT3& position,
                                                             const DirectX::XMFLOAT3& camera_position) const {
    DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&position);
    DirectX::XMVECTOR cam_pos = DirectX::XMLoadFloat3(&camera_position);
    
    // Calculate direction from camera to position
    DirectX::XMVECTOR direction = DirectX::XMVector3Normalize(pos - cam_pos);
    
    // Create billboard matrix that always faces the camera
    DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    DirectX::XMVECTOR right = DirectX::XMVector3Cross(up, direction);
    up = DirectX::XMVector3Cross(direction, right);
    
    DirectX::XMMATRIX rotation = DirectX::XMMatrixSet(
        right.m128_f32[0], right.m128_f32[1], right.m128_f32[2], 0.0f,
        up.m128_f32[0], up.m128_f32[1], up.m128_f32[2], 0.0f,
        direction.m128_f32[0], direction.m128_f32[1], direction.m128_f32[2], 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );
    
    return DirectX::XMMatrixScaling(m_config.scale, m_config.scale, m_config.scale) * 
           rotation * 
           DirectX::XMMatrixTranslation(position.x, position.y, position.z);
}

DirectX::XMMATRIX UIProjectionManager::create_cylindrical_matrix(const DirectX::XMFLOAT3& position,
                                                                const DirectX::XMFLOAT3& camera_position) const {
    // Simplified cylindrical projection
    DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&position);
    DirectX::XMVECTOR cam_pos = DirectX::XMLoadFloat3(&camera_position);
    
    // Calculate angle around Y axis
    DirectX::XMVECTOR diff = pos - cam_pos;
    float angle = atan2f(diff.m128_f32[0], diff.m128_f32[2]);
    
    // Create cylindrical projection matrix
    DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationY(angle);
    DirectX::XMMATRIX translation = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
    
    return DirectX::XMMatrixScaling(m_config.scale, m_config.scale, m_config.scale) * 
           rotation * 
           translation;
}

DirectX::XMMATRIX UIProjectionManager::create_spherical_matrix(const DirectX::XMFLOAT3& position,
                                                               const DirectX::XMFLOAT3& camera_position) const {
    // Simplified spherical projection
    DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&position);
    DirectX::XMVECTOR cam_pos = DirectX::XMLoadFloat3(&camera_position);
    
    // Calculate spherical coordinates
    DirectX::XMVECTOR diff = pos - cam_pos;
    float distance = DirectX::XMVectorGetX(DirectX::XMVector3Length(diff));
    float theta = atan2f(diff.m128_f32[1], sqrtf(diff.m128_f32[0] * diff.m128_f32[0] + diff.m128_f32[2] * diff.m128_f32[2]));
    float phi = atan2f(diff.m128_f32[0], diff.m128_f32[2]);
    
    // Create spherical projection matrix
    DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationY(phi) * DirectX::XMMatrixRotationX(theta);
    DirectX::XMMATRIX translation = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
    
    return DirectX::XMMatrixScaling(m_config.scale, m_config.scale, m_config.scale) * 
           rotation * 
           translation;
}

// REEngineUIProjection Implementation
REEngineUIProjection::REEngineUIProjection() {
    setup_re_engine_ui_profile();
}

void REEngineUIProjection::optimize_horror_game_ui() {
    spdlog::info("[REEngineUI] Optimizing horror game UI");
    
    // Optimize UI for horror game atmosphere
    m_config.distance = 1.8f;  // Closer for immersion
    m_config.scale = 1.1f;     // Slightly larger for readability in dark scenes
    
    // Use billboard projection for consistent readability
    m_config.type = UIProjectionConfig::ProjectionType::Billboard;
    
    // Enable alpha blending for atmospheric effects
    m_config.enable_alpha_blending = true;
    
    apply_horror_game_ui_optimizations();
}

void REEngineUIProjection::adjust_ui_for_fixed_cameras() {
    spdlog::info("[REEngineUI] Adjusting UI for fixed cameras");
    
    // Since RE Engine uses fixed cameras, adjust UI positioning
    m_config.follow_view = false;
    m_config.type = UIProjectionConfig::ProjectionType::Fixed;
    
    // Position UI elements relative to camera view
    m_state.hud_position = {0.0f, 0.0f, 1.5f};
    m_state.menu_position = {0.0f, 0.0f, 1.2f};
}

void REEngineUIProjection::optimize_hud_for_atmospheric_lighting() {
    spdlog::info("[REEngineUI] Optimizing HUD for atmospheric lighting");
    
    // Adjust HUD opacity for dark scenes
    m_state.hud_opacity = 0.9f;
    m_state.menu_opacity = 0.95f;
    
    // Ensure HUD elements are visible in low-light conditions
    for (auto& element : m_hud_elements) {
        if (element.type == "progress_bar" || element.type == "text") {
            element.properties.push_back("high_contrast");
        }
    }
}

void REEngineUIProjection::adapt_menu_system_for_vr() {
    spdlog::info("[REEngineUI] Adapting menu system for VR");
    
    // Optimize menu positioning for VR
    m_state.menu_position = {0.0f, 0.0f, 1.2f};
    
    // Add VR-specific menu elements
    add_hud_element({"vr_settings", "panel", {0.5f, 0.5f}, {0.4f, 0.6f}, false, true, "", {"vr_options"}});
    add_hud_element({"comfort_settings", "panel", {0.5f, 0.5f}, {0.4f, 0.6f}, false, true, "", {"comfort"}});
}

void REEngineUIProjection::setup_re_engine_ui_profile() {
    adapt_ui_for_engine("RE Engine", "Resident Evil 7");
}

void REEngineUIProjection::apply_horror_game_ui_optimizations() {
    // Apply horror game specific UI optimizations
    m_config.enable_alpha_blending = true;
    m_config.distance = 1.8f;
    
    spdlog::info("[REEngineUI] Horror game UI optimizations applied");
}

// REDengine4UIProjection Implementation
REDengine4UIProjection::REDengine4UIProjection() {
    setup_redengine4_ui_profile();
}

void REDengine4UIProjection::optimize_urban_environment_ui() {
    spdlog::info("[REDengine4UI] Optimizing urban environment UI");
    
    // Optimize UI for complex urban environments
    m_config.distance = 2.2f;  // Further for complex scenes
    m_config.scale = 1.0f;     // Standard scale for urban clarity
    
    // Use cylindrical projection for urban environments
    m_config.type = UIProjectionConfig::ProjectionType::Cylindrical;
    
    apply_cyberpunk_ui_optimizations();
}

void REDengine4UIProjection::adapt_cyberpunk_hud_for_vr() {
    spdlog::info("[REDengine4UI] Adapting cyberpunk HUD for VR");
    
    // Optimize cyberpunk HUD elements for VR
    m_state.hud_opacity = 0.85f;
    m_state.menu_opacity = 0.9f;
    
    // Add cyberpunk-specific HUD elements
    add_hud_element({"hack_interface", "overlay", {0.5f, 0.5f}, {0.6f, 0.6f}, false, true, "", {"hacking"}});
    add_hud_element({"augmentation_status", "panel", {0.8f, 0.1f}, {0.18f, 0.15f}, true, false, "", {"augs"}});
}

void REDengine4UIProjection::optimize_menu_system_for_complex_ui() {
    spdlog::info("[REDengine4UI] Optimizing menu system for complex UI");
    
    // Optimize for complex cyberpunk menus
    m_state.menu_position = {0.0f, 0.0f, 1.8f};
    
    // Add complex menu elements
    add_hud_element({"inventory_grid", "grid", {0.5f, 0.5f}, {0.7f, 0.7f}, false, true, "", {"items"}});
    add_hud_element({"skill_tree", "tree", {0.5f, 0.5f}, {0.8f, 0.8f}, false, true, "", {"skills"}});
}

void REDengine4UIProjection::adjust_notification_system() {
    spdlog::info("[REDengine4UI] Adjusting notification system");
    
    // Optimize notification positioning for VR
    m_state.notifications_enabled = true;
    
    // Add notification elements
    add_hud_element({"notification", "text", {0.5f, 0.1f}, {0.4f, 0.08f}, false, false, "", {"alert"}});
    add_hud_element({"quest_update", "text", {0.5f, 0.2f}, {0.4f, 0.08f}, false, false, "", {"quest"}});
}

void REDengine4UIProjection::setup_redengine4_ui_profile() {
    adapt_ui_for_engine("REDengine 4", "Cyberpunk 2077");
}

void REDengine4UIProjection::apply_cyberpunk_ui_optimizations() {
    // Apply cyberpunk specific UI optimizations
    m_config.enable_alpha_blending = true;
    m_config.distance = 2.2f;
    
    spdlog::info("[REDengine4UI] Cyberpunk UI optimizations applied");
}

// MTFrameworkUIProjection Implementation
MTFrameworkUIProjection::MTFrameworkUIProjection() {
    setup_mt_framework_ui_profile();
}

void MTFrameworkUIProjection::optimize_combat_hud_for_vr() {
    spdlog::info("[MTFrameworkUI] Optimizing combat HUD for VR");
    
    // Optimize UI for intense combat scenes
    m_config.distance = 1.5f;  // Closer for quick access during combat
    m_config.scale = 1.3f;     // Larger for quick reading during combat
    
    // Use billboard projection for combat clarity
    m_config.type = UIProjectionConfig::ProjectionType::Billboard;
    
    apply_action_game_ui_optimizations();
}

void MTFrameworkUIProjection::adapt_weapon_ui_for_motion_controllers() {
    spdlog::info("[MTFrameworkUI] Adapting weapon UI for motion controllers");
    
    // Optimize weapon UI for motion controller interaction
    m_state.hud_opacity = 0.9f;
    
    // Add weapon-specific UI elements
    add_hud_element({"weapon_status", "panel", {0.8f, 0.8f}, {0.18f, 0.15f}, true, false, "", {"weapon"}});
    add_hud_element({"ammo_display", "text", {0.8f, 0.7f}, {0.18f, 0.08f}, true, false, "", {"ammo"}});
}

void MTFrameworkUIProjection::optimize_monster_information_display() {
    spdlog::info("[MTFrameworkUI] Optimizing monster information display");
    
    // Optimize monster info display for VR
    add_hud_element({"monster_health", "progress_bar", {0.7f, 0.15f}, {0.28f, 0.03f}, true, false, "", {"monster_health"}});
    add_hud_element({"monster_status", "text", {0.7f, 0.12f}, {0.28f, 0.03f}, true, false, "", {"monster_status"}});
    add_hud_element({"hunt_timer", "text", {0.7f, 0.09f}, {0.28f, 0.03f}, true, false, "", {"timer"}});
}

void MTFrameworkUIProjection::adjust_inventory_system_for_vr() {
    spdlog::info("[MTFrameworkUI] Adjusting inventory system for VR");
    
    // Optimize inventory for VR interaction
    m_state.menu_opacity = 0.95f;
    
    // Add VR-optimized inventory elements
    add_hud_element({"item_grid", "grid", {0.5f, 0.5f}, {0.6f, 0.6f}, false, true, "", {"items"}});
    add_hud_element({"crafting_menu", "panel", {0.5f, 0.5f}, {0.5f, 0.6f}, false, true, "", {"crafting"}});
}

void MTFrameworkUIProjection::setup_mt_framework_ui_profile() {
    adapt_ui_for_engine("MT Framework", "Monster Hunter: World");
}

void MTFrameworkUIProjection::apply_action_game_ui_optimizations() {
    // Apply action game specific UI optimizations
    m_config.enable_alpha_blending = true;
    m_config.distance = 1.5f;
    
    spdlog::info("[MTFrameworkUI] Action game UI optimizations applied");
}

// GlobalUIProjectionManager Implementation
std::unique_ptr<UIProjectionManager> GlobalUIProjectionManager::s_global_ui_manager;
std::unique_ptr<REEngineUIProjection> GlobalUIProjectionManager::s_re_engine_ui_manager;
std::unique_ptr<REDengine4UIProjection> GlobalUIProjectionManager::s_redengine4_ui_manager;
std::unique_ptr<MTFrameworkUIProjection> GlobalUIProjectionManager::s_mt_framework_ui_manager;
std::mutex GlobalUIProjectionManager::s_init_mutex;

UIProjectionManager& GlobalUIProjectionManager::get_ui_manager() {
    initialize_if_needed();
    return *s_global_ui_manager;
}

REEngineUIProjection& GlobalUIProjectionManager::get_re_engine_ui_manager() {
    initialize_if_needed();
    return *s_re_engine_ui_manager;
}

REDengine4UIProjection& GlobalUIProjectionManager::get_redengine4_ui_manager() {
    initialize_if_needed();
    return *s_redengine4_ui_manager;
}

MTFrameworkUIProjection& GlobalUIProjectionManager::get_mt_framework_ui_manager() {
    initialize_if_needed();
    return *s_mt_framework_ui_manager;
}

void GlobalUIProjectionManager::configure_ui_projection(const UIProjectionConfig& config) {
    std::lock_guard<std::mutex> lock(s_init_mutex);
    
    if (s_global_ui_manager) {
        s_global_ui_manager->set_ui_projection_config(config);
    }
}

void GlobalUIProjectionManager::set_global_ui_style(const std::string& style) {
    std::lock_guard<std::mutex> lock(s_init_mutex);
    
    UIProjectionConfig config;
    
    if (style == "minimal") {
        config.distance = 1.5f;
        config.scale = 0.8f;
        config.type = UIProjectionConfig::ProjectionType::Billboard;
    } else if (style == "immersive") {
        config.distance = 2.5f;
        config.scale = 1.5f;
        config.type = UIProjectionConfig::ProjectionType::Cylindrical;
    } else if (style == "standard") {
        config.distance = 2.0f;
        config.scale = 1.0f;
        config.type = UIProjectionConfig::ProjectionType::Billboard;
    }
    
    configure_ui_projection(config);
    spdlog::info("[GlobalUIProjection] Global UI style set to: {}", style);
}

void GlobalUIProjectionManager::cleanup_all() {
    std::lock_guard<std::mutex> lock(s_init_mutex);
    
    s_global_ui_manager.reset();
    s_re_engine_ui_manager.reset();
    s_redengine4_ui_manager.reset();
    s_mt_framework_ui_manager.reset();
    
    spdlog::info("[GlobalUIProjection] All UI projection managers cleaned up");
}

void GlobalUIProjectionManager::initialize_if_needed() {
    std::lock_guard<std::mutex> lock(s_init_mutex);
    
    if (!s_global_ui_manager) {
        s_global_ui_manager = std::make_unique<UIProjectionManager>();
    }
    
    if (!s_re_engine_ui_manager) {
        s_re_engine_ui_manager = std::make_unique<REEngineUIProjection>();
    }
    
    if (!s_redengine4_ui_manager) {
        s_redengine4_ui_manager = std::make_unique<REDengine4UIProjection>();
    }
    
    if (!s_mt_framework_ui_manager) {
        s_mt_framework_ui_manager = std::make_unique<MTFrameworkUIProjection>();
    }
}

// Advanced UI Pipeline Management Implementation
bool UIProjectionManager::initialize_ui_shaders() {
    spdlog::debug("[UIProjection] Initializing UI shaders");
    
    try {
        auto* framework = Framework::get();
        if (!framework) {
            spdlog::error("[UIProjection] Framework not available for shader initialization");
            return false;
        }
        
        auto renderer_type = framework->get_renderer_type();
        
        if (renderer_type == Framework::RendererType::D3D11) {
            return initialize_d3d11_ui_shaders();
        } else if (renderer_type == Framework::RendererType::D3D12) {
            return initialize_d3d12_ui_shaders();
        } else {
            spdlog::error("[UIProjection] Unsupported renderer type for shader initialization");
            return false;
        }
    } catch (const std::exception& e) {
        spdlog::error("[UIProjection] Failed to initialize UI shaders: {}", e.what());
        return false;
    }
}

bool UIProjectionManager::create_ui_vertex_buffers() {
    spdlog::debug("[UIProjection] Creating UI vertex buffers");
    
    try {
        // Create vertex buffer for UI quads
        struct UIVertex {
            DirectX::XMFLOAT3 position;
            DirectX::XMFLOAT2 texcoord;
            DirectX::XMFLOAT4 color;
        };
        
        // Standard UI quad vertices
        std::vector<UIVertex> vertices = {
            {{-1.0f, -1.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
            {{ 1.0f, -1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
            {{-1.0f,  1.0f, 0.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
            {{ 1.0f,  1.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}}
        };
        
        auto* framework = Framework::get();
        if (framework->get_renderer_type() == Framework::RendererType::D3D11) {
            return create_d3d11_ui_vertex_buffer(vertices);
        } else if (framework->get_renderer_type() == Framework::RendererType::D3D12) {
            return create_d3d12_ui_vertex_buffer(vertices);
        }
        
        return false;
    } catch (const std::exception& e) {
        spdlog::error("[UIProjection] Failed to create UI vertex buffers: {}", e.what());
        return false;
    }
}

bool UIProjectionManager::setup_ui_render_states() {
    spdlog::debug("[UIProjection] Setting up UI render states");
    
    try {
        auto* framework = Framework::get();
        if (!framework) {
            return false;
        }
        
        auto renderer_type = framework->get_renderer_type();
        
        if (renderer_type == Framework::RendererType::D3D11) {
            return setup_d3d11_ui_render_states();
        } else if (renderer_type == Framework::RendererType::D3D12) {
            return setup_d3d12_ui_render_states();
        }
        
        return false;
    } catch (const std::exception& e) {
        spdlog::error("[UIProjection] Failed to setup UI render states: {}", e.what());
        return false;
    }
}

bool UIProjectionManager::create_ui_texture_samplers() {
    spdlog::debug("[UIProjection] Creating UI texture samplers");
    
    try {
        auto* framework = Framework::get();
        if (!framework) {
            return false;
        }
        
        auto renderer_type = framework->get_renderer_type();
        
        if (renderer_type == Framework::RendererType::D3D11) {
            return create_d3d11_ui_texture_samplers();
        } else if (renderer_type == Framework::RendererType::D3D12) {
            return create_d3d12_ui_texture_samplers();
        }
        
        return false;
    } catch (const std::exception& e) {
        spdlog::error("[UIProjection] Failed to create UI texture samplers: {}", e.what());
        return false;
    }
}

bool UIProjectionManager::initialize_ui_constant_buffers() {
    spdlog::debug("[UIProjection] Initializing UI constant buffers");
    
    try {
        auto* framework = Framework::get();
        if (!framework) {
            return false;
        }
        
        auto renderer_type = framework->get_renderer_type();
        
        if (renderer_type == Framework::RendererType::D3D11) {
            return initialize_d3d11_ui_constant_buffers();
        } else if (renderer_type == Framework::RendererType::D3D12) {
            return initialize_d3d12_ui_constant_buffers();
        }
        
        return false;
    } catch (const std::exception& e) {
        spdlog::error("[UIProjection] Failed to initialize UI constant buffers: {}", e.what());
        return false;
    }
}

bool UIProjectionManager::load_ui_element_textures() {
    spdlog::debug("[UIProjection] Loading UI element textures");
    
    try {
        // Load default UI textures
        if (!load_default_ui_textures()) {
            spdlog::warn("[UIProjection] Failed to load default UI textures");
        }
        
        // Load game-specific UI textures
        if (!load_game_specific_ui_textures()) {
            spdlog::warn("[UIProjection] Failed to load game-specific UI textures");
        }
        
        // Create UI texture atlas
        if (!create_ui_texture_atlas()) {
            spdlog::warn("[UIProjection] Failed to create UI texture atlas");
        }
        
        return true;
    } catch (const std::exception& e) {
        spdlog::error("[UIProjection] Failed to load UI element textures: {}", e.what());
        return false;
    }
}

bool UIProjectionManager::setup_imgui_integration() {
    spdlog::debug("[UIProjection] Setting up ImGui integration");
    
    try {
        // Initialize ImGui context
        if (!initialize_imgui_context()) {
            spdlog::error("[UIProjection] Failed to initialize ImGui context");
            return false;
        }
        
        // Setup ImGui style
        if (!setup_imgui_style()) {
            spdlog::warn("[UIProjection] Failed to setup ImGui style, using defaults");
        }
        
        // Setup ImGui input handling
        if (!setup_imgui_input()) {
            spdlog::warn("[UIProjection] Failed to setup ImGui input handling");
        }
        
        // Setup ImGui rendering
        if (!setup_imgui_rendering()) {
            spdlog::error("[UIProjection] Failed to setup ImGui rendering");
            return false;
        }
        
        spdlog::info("[UIProjection] ImGui integration successfully initialized");
        return true;
    } catch (const std::exception& e) {
        spdlog::error("[UIProjection] Failed to setup ImGui integration: {}", e.what());
        return false;
    }
}

bool UIProjectionManager::initialize_vr_ui_components() {
    spdlog::debug("[UIProjection] Initializing VR UI components");
    
    try {
        auto* framework = Framework::get();
        if (!framework || !framework->is_vr_enabled()) {
            spdlog::warn("[UIProjection] VR not enabled, skipping VR UI initialization");
            return false;
        }
        
        // Initialize VR-specific UI positioning
        if (!initialize_vr_ui_positioning()) {
            spdlog::warn("[UIProjection] Failed to initialize VR UI positioning");
        }
        
        // Setup VR UI interaction
        if (!setup_vr_ui_interaction()) {
            spdlog::warn("[UIProjection] Failed to setup VR UI interaction");
        }
        
        // Initialize VR UI haptics
        if (!initialize_vr_ui_haptics()) {
            spdlog::warn("[UIProjection] Failed to initialize VR UI haptics");
        }
        
        spdlog::info("[UIProjection] VR UI components initialized successfully");
        return true;
    } catch (const std::exception& e) {
        spdlog::error("[UIProjection] Failed to initialize VR UI components: {}", e.what());
        return false;
    }
}

void UIProjectionManager::setup_ui_performance_monitoring() {
    spdlog::debug("[UIProjection] Setting up UI performance monitoring");
    
    try {
        // Initialize performance counters
        m_ui_performance_stats = std::make_unique<UIPerformanceStats>();
        
        // Setup performance monitoring callbacks
        setup_performance_monitoring_callbacks();
        
        // Initialize performance profiling
        initialize_performance_profiling();
        
        spdlog::info("[UIProjection] UI performance monitoring initialized");
    } catch (const std::exception& e) {
        spdlog::error("[UIProjection] Failed to setup UI performance monitoring: {}", e.what());
    }
}

void UIProjectionManager::initialize_ui_resource_manager() {
    spdlog::debug("[UIProjection] Initializing UI resource manager");
    
    try {
        // Initialize resource tracking
        m_ui_resource_manager = std::make_unique<UIResourceManager>();
        
        // Setup resource cleanup callbacks
        setup_resource_cleanup_callbacks();
        
        // Initialize resource pooling
        initialize_resource_pooling();
        
        spdlog::info("[UIProjection] UI resource manager initialized");
    } catch (const std::exception& e) {
        spdlog::error("[UIProjection] Failed to initialize UI resource manager: {}", e.what());
    }
}

void UIProjectionManager::setup_ui_debugging_tools() {
    spdlog::debug("[UIProjection] Setting up UI debugging tools");
    
    try {
        // Initialize debug overlay
        if (!initialize_debug_overlay()) {
            spdlog::warn("[UIProjection] Failed to initialize debug overlay");
        }
        
        // Setup debug logging
        setup_debug_logging();
        
        // Initialize performance visualization
        if (!initialize_performance_visualization()) {
            spdlog::warn("[UIProjection] Failed to initialize performance visualization");
        }
        
        spdlog::info("[UIProjection] UI debugging tools initialized");
    } catch (const std::exception& e) {
        spdlog::error("[UIProjection] Failed to setup UI debugging tools: {}", e.what());
    }
}

void UIProjectionManager::cleanup_partial_ui_pipeline() {
    spdlog::debug("[UIProjection] Cleaning up partial UI pipeline");
    
    try {
        // Cleanup shaders
        cleanup_ui_shaders();
        
        // Cleanup vertex buffers
        cleanup_ui_vertex_buffers();
        
        // Cleanup render states
        cleanup_ui_render_states();
        
        // Cleanup texture samplers
        cleanup_ui_texture_samplers();
        
        // Cleanup constant buffers
        cleanup_ui_constant_buffers();
        
        // Cleanup ImGui resources
        cleanup_imgui_resources();
        
        // Reset pipeline state
        m_pipeline_initialized = false;
        
        spdlog::info("[UIProjection] Partial UI pipeline cleanup completed");
    } catch (const std::exception& e) {
        spdlog::error("[UIProjection] Failed to cleanup partial UI pipeline: {}", e.what());
    }
}

} // namespace uevr_common
