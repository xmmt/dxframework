#include <dxframework/texture.hpp>
#include <dxframework/utils.hpp>

#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>

using namespace dxframework;
using namespace DirectX;

texture::texture(ID3D11Device* device, color const& color, aiTextureType type) {
    initialize_1x1_color_texture(device, color, type);
}

texture::texture(ID3D11Device* device, color const* color_data, uint32_t width, uint32_t height, aiTextureType type) {
    initialize_color_texture(device, color_data, width, height, type);
}

texture::texture(ID3D11Device* device, std::string const& file_path, aiTextureType type) {
    type_ = type;

    //if (StringConverter::GetFileExtension(file_path) == ".dds") {
    if (FAILED(CreateDDSTextureFromFile(device, /* StringConverter::StringToWide(filePath).c_str() */ std::wstring(file_path.begin(), file_path.end()).c_str(), texture_.GetAddressOf(), texture_view_.GetAddressOf()))) {
        initialize_1x1_color_texture(device, colors::unloaded_texture_color, type);
    }
    return;
    //}

    if (FAILED(CreateWICTextureFromFile(device, /* StringConverter::StringToWide(filePath).c_str() */ std::wstring(file_path.begin(), file_path.end()).c_str(), texture_.GetAddressOf(), texture_view_.GetAddressOf()))) {

        initialize_1x1_color_texture(device, colors::unloaded_texture_color, type);
    }
}

aiTextureType texture::get_type() {
    return type_;
}

ID3D11ShaderResourceView* texture::get_texture_resource_view() {
    return texture_view_.Get();
}

ID3D11ShaderResourceView** texture::get_texture_resource_view_address() {
    return texture_view_.GetAddressOf();
}

void texture::initialize_1x1_color_texture(ID3D11Device* device, color const& color_data, aiTextureType type) {
    initialize_color_texture(device, &color_data, 1, 1, type);
}

void texture::initialize_color_texture(ID3D11Device* device, color const* color_data, uint32_t width, uint32_t height, aiTextureType type) {
    type_ = type;
    CD3D11_TEXTURE2D_DESC texture_desc(DXGI_FORMAT_R8G8B8A8_UNORM, width, height);
    ID3D11Texture2D* p_2D_texture = nullptr;
    D3D11_SUBRESOURCE_DATA initial_data{
        .pSysMem = color_data,
        .SysMemPitch = width * sizeof(color),
        .SysMemSlicePitch = 0
    };
    throw_if_failed(device->CreateTexture2D(&texture_desc, &initial_data, &p_2D_texture));
    texture_ = static_cast<ID3D11Texture2D*>(p_2D_texture);
    CD3D11_SHADER_RESOURCE_VIEW_DESC srv_desc{
        D3D11_SRV_DIMENSION_TEXTURE2D,
        texture_desc.Format
    };
    throw_if_failed(device->CreateShaderResourceView(texture_.Get(), &srv_desc, texture_view_.GetAddressOf()));
}
