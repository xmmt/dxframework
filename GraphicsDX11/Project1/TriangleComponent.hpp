#pragma once

#include "pch.h"

#include "GameComponent.hpp"

class TriangleComponent : public GameComponent {
public:
    TriangleComponent() = delete;
    TriangleComponent(
      std::string fxShader,
      std::vector<D3D_SHADER_MACRO> vertexMacro,
      std::vector<D3D_SHADER_MACRO> pixelMacro,
      std::vector<DirectX::XMFLOAT4> positions,
      std::vector<DirectX::XMFLOAT4> colors);

public:
    void destroyResources() override;
    void draw(ID3D11DeviceContext* context) override;
    void initialize(ID3D11Device* device) override;
    void reload() override;
    void update() override;

private:
    ID3D11InputLayout* layout_;
    ID3D11PixelShader* pixelShader_;
    ID3DBlob* pixelShaderByteCode_;
    ID3D11RasterizerState* rastState_;
    ID3D11VertexShader* vertexShader_;
    ID3DBlob* vertexShaderByteCode_;

    std::vector<DirectX::XMFLOAT4> positions_;
    std::vector<DirectX::XMFLOAT4> colors_;
    std::vector<DirectX::XMFLOAT4> points_;

    ID3D11Buffer* vBuffers_[2];
    UINT strides_[2] = { 16, 16 };
    UINT offsets_[2] = { 0, 0 };
    ID3D11Buffer* indexBuffer_;
};