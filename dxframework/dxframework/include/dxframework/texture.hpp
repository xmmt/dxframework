#pragma once

#include <assimp/material.h>

#include <dxframework/color.hpp>

#include <d3d11.h>
#include <wrl/client.h>

namespace dxframework {

enum class texture_storage_type {
    INVALID,
    NONE,
    EMBEDDED_INDEX_COMPRESSED,
    EMBEDDED_INDEX_NON_COMPRESSED,
    EMBEDDED_COMPRESSED,
    EMBEDDED_NON_COMPRESSED,
    DISK,
};

class texture {
public:
    texture(ID3D11Device* device, color const& color, aiTextureType type);
    texture(ID3D11Device* device, color const* color_data, uint32_t width, uint32_t height, aiTextureType type);
    texture(ID3D11Device* device, std::string const& file_path, aiTextureType type);

    aiTextureType get_type();
    ID3D11ShaderResourceView* get_texture_resource_view();
    ID3D11ShaderResourceView** get_texture_resource_view_address();

private:
    void initialize_1x1_color_texture(ID3D11Device* device, color const& color_data, aiTextureType type);
    void initialize_color_texture(ID3D11Device* device, color const* colorData, uint32_t width, uint32_t height, aiTextureType type);

private:
    Microsoft::WRL::ComPtr<ID3D11Resource> texture_ = nullptr;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture_view_ = nullptr;
    aiTextureType type_ = aiTextureType::aiTextureType_UNKNOWN;
};

} // namespace dxframework
