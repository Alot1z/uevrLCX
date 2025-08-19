/*
 * Matrix Extraction Utility for uevr Cross-Engine Adapters
 * 
 * This header provides utilities for extracting view and projection matrices
 * from various graphics APIs and engine formats.
 * 
 * Copyright (c) 2024 uevr Cross-Engine Adapter Project
 * Licensed under MIT License
 */

#pragma once

#include <windows.h>
#include <d3d11.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <vector>
#include <memory>
#include <string>

namespace uevr_common {

/**
 * Matrix extraction utility class
 * 
 * Provides common functionality for extracting view and projection matrices
 * from different graphics APIs and engine formats.
 */
class MatrixExtractor {
public:
    // Matrix extraction result
    struct ExtractionResult {
        DirectX::XMMATRIX view;
        DirectX::XMMATRIX projection;
        DirectX::XMMATRIX view_projection;
        bool valid = false;
        std::string source;
        std::string error_message;
    };

    // Constant buffer information
    struct ConstantBufferInfo {
        UINT slot = 0;
        UINT offset = 0;
        UINT size = 0;
        bool contains_view_matrix = false;
        bool contains_projection_matrix = false;
        std::string description;
    };

    // Matrix layout information
    struct MatrixLayout {
        bool row_major = true;
        bool transpose = false;
        UINT view_offset = 0;
        UINT projection_offset = 64; // Default offset for 4x4 matrices
        UINT matrix_size = 64; // 4x4 float matrix size in bytes
    };

public:
    MatrixExtractor() = default;
    ~MatrixExtractor() = default;

    // D3D11 matrix extraction
    ExtractionResult extract_d3d11_matrices(
        ID3D11DeviceContext* context,
        const std::vector<ConstantBufferInfo>& constant_buffers,
        const MatrixLayout& layout = MatrixLayout{}
    );

    // D3D12 matrix extraction
    ExtractionResult extract_d3d12_matrices(
        ID3D12GraphicsCommandList* command_list,
        const std::vector<ConstantBufferInfo>& constant_buffers,
        const MatrixLayout& layout = MatrixLayout{}
    );

    // Generic matrix extraction from memory
    ExtractionResult extract_matrices_from_memory(
        const void* data,
        size_t data_size,
        const MatrixLayout& layout = MatrixLayout{}
    );

    // Matrix validation
    bool validate_matrix(const DirectX::XMMATRIX& matrix, const std::string& matrix_type);
    bool is_identity_matrix(const DirectX::XMMATRIX& matrix, float tolerance = 0.001f);
    bool is_orthogonal_matrix(const DirectX::XMMATRIX& matrix, float tolerance = 0.001f);

    // Matrix conversion utilities
    DirectX::XMMATRIX convert_matrix_layout(
        const DirectX::XMMATRIX& matrix,
        bool from_row_major,
        bool to_row_major,
        bool transpose = false
    );

    DirectX::XMMATRIX extract_view_matrix_from_view_projection(
        const DirectX::XMMATRIX& view_projection,
        const DirectX::XMMATRIX& projection
    );

    DirectX::XMMATRIX extract_projection_matrix_from_view_projection(
        const DirectX::XMMATRIX& view_projection,
        const DirectX::XMMATRIX& view
    );

    // FOV and projection utilities
    struct ProjectionInfo {
        float fov_y = 0.0f;
        float aspect_ratio = 16.0f / 9.0f;
        float near_plane = 0.1f;
        float far_plane = 1000.0f;
        bool perspective = true;
    };

    ProjectionInfo extract_projection_info(const DirectX::XMMATRIX& projection_matrix);
    DirectX::XMMATRIX create_projection_matrix(const ProjectionInfo& info);

    // Debug and logging
    std::string matrix_to_string(const DirectX::XMMATRIX& matrix, const std::string& name = "Matrix");
    void log_matrix_extraction_result(const ExtractionResult& result);

private:
    // Internal extraction helpers
    DirectX::XMMATRIX extract_matrix_from_buffer(
        const void* buffer_data,
        UINT offset,
        const MatrixLayout& layout
    );

    bool find_matrix_in_buffer(
        const void* buffer_data,
        UINT buffer_size,
        const MatrixLayout& layout,
        DirectX::XMMATRIX& view_matrix,
        DirectX::XMMATRIX& projection_matrix
    );

    // Matrix pattern recognition
    bool is_likely_view_matrix(const DirectX::XMMATRIX& matrix);
    bool is_likely_projection_matrix(const DirectX::XMMATRIX& matrix);

    // Memory safety
    bool is_safe_to_read(const void* ptr, size_t size);
    template<typename T>
    T safe_read(const void* ptr, size_t offset) {
        if (!is_safe_to_read(ptr, offset + sizeof(T))) {
            return T{};
        }
        return *reinterpret_cast<const T*>(static_cast<const uint8_t*>(ptr) + offset);
    }
};

/**
 * Engine-specific matrix extractors
 * 
 * These classes provide specialized matrix extraction for specific engines
 * based on their known matrix layouts and formats.
 */
class REEngineMatrixExtractor : public MatrixExtractor {
public:
    // RE Engine specific matrix extraction
    ExtractionResult extract_re_engine_matrices(
        ID3D11DeviceContext* context,
        const std::vector<ConstantBufferInfo>& constant_buffers
    );

private:
    // RE Engine specific matrix layout
    static constexpr MatrixLayout RE_ENGINE_LAYOUT = {
        .row_major = true,
        .transpose = true,
        .view_offset = 0,
        .projection_offset = 64,
        .matrix_size = 64
    };
};

class REDengine4MatrixExtractor : public MatrixExtractor {
public:
    // REDengine 4 specific matrix extraction
    ExtractionResult extract_redengine4_matrices(
        ID3D12GraphicsCommandList* command_list,
        const std::vector<ConstantBufferInfo>& constant_buffers
    );

private:
    // REDengine 4 specific matrix layout
    static constexpr MatrixLayout REDENGINE4_LAYOUT = {
        .row_major = true,
        .transpose = true,
        .view_offset = 0,
        .projection_offset = 64,
        .matrix_size = 64
    };
};

class MTFrameworkMatrixExtractor : public MatrixExtractor {
public:
    // MT Framework specific matrix extraction
    ExtractionResult extract_mt_framework_matrices(
        ID3D11DeviceContext* context,
        const std::vector<ConstantBufferInfo>& constant_buffers
    );

private:
    // MT Framework specific matrix layout
    static constexpr MatrixLayout MT_FRAMEWORK_LAYOUT = {
        .row_major = true,
        .transpose = true,
        .view_offset = 0,
        .projection_offset = 64,
        .matrix_size = 64
    };
};

} // namespace uevr_common


