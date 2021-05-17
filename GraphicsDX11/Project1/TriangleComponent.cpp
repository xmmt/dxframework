#include "pch.h"

#include "TriangleComponent.hpp"

using namespace DirectX;

TriangleComponent::TriangleComponent(
  std::string fxShader,
  std::vector<D3D_SHADER_MACRO> vertexMacro,
  std::vector<D3D_SHADER_MACRO> pixelMacro,
  std::vector<DirectX::XMFLOAT4> positions,
  std::vector<DirectX::XMFLOAT4> colors)
    : positions_{ std::move(positions) }
    , colors_{ std::move(colors) } {

    assert(positions_.size() == colors_.size());
    for (size_t i = 0; i < positions_.size(); ++i) {
        points_.emplace_back(positions_[i]);
        points_.emplace_back(colors_[i]);
    }

    auto prepareMacro = [](auto& macro) {
        if (macro.size() > 0 && (macro.back().Definition != nullptr || macro.back().Name != nullptr)) {
            macro.emplace_back(nullptr, nullptr);
        } else if (macro.size() == 1) {
            macro.clear();
        }
    };
    prepareMacro(vertexMacro);
    prepareMacro(pixelMacro);


    HRESULT res;
    ID3DBlob* errorVertexCode;
    ID3DBlob* errorPixelCode;

    res = D3DCompileFromFile(
      L"MiniTri.fx",
      vertexMacro.empty() ? nullptr : vertexMacro.data(),
      nullptr /*include*/,
      "VSMain",
      "vs_5_0",
#if _DEBUG
      D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
#else
      D3DCOMPILE_OPTIMIZATION_LEVEL3,
#endif
      0,
      &vertexShaderByteCode_,
      &errorVertexCode);

    if (FAILED(res)) {
        // If the shader failed to compile it should have written something to the error message.
        if (errorVertexCode) {
            char* compileErrors = (char*)(errorVertexCode->GetBufferPointer());

            std::cout << compileErrors << std::endl;
        }
        // If there was  nothing in the error message then it simply could not find the shader file itself.
        else {
            // MessageBox(display_.hWnd(), L"MiniTri.fx", L"Missing Shader File", MB_OK);
            WriteError() << "MiniTri.fx Missing Shader File\n";
        }

        ::exit(0);
    }

    res = D3DCompileFromFile(
      L"MiniTri.fx",
      pixelMacro.empty() ? nullptr : pixelMacro.data(),
      nullptr /*include*/,
      "PSMain",
      "ps_5_0",
#if _DEBUG
      D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
#else
      D3DCOMPILE_OPTIMIZATION_LEVEL3,
#endif
      0,
      &pixelShaderByteCode_,
      &errorPixelCode);

    if (FAILED(res)) {
        // If the shader failed to compile it should have written something to the error message.
        if (errorPixelCode) {
            char* compileErrors = (char*)(errorPixelCode->GetBufferPointer());

            std::cout << compileErrors << std::endl;
        }
        // If there was  nothing in the error message then it simply could not find the shader file itself.
        else {
            // MessageBox(display_.hWnd(), L"MiniTri.fx", L"Missing Shader File", MB_OK);
            WriteError() << "MiniTri.fx Missing Shader File\n";
        }

        ::exit(0);
    }
}

void TriangleComponent::initialize(ID3D11Device* device) {
    //auto vertexShaderBytecode = reader->ReadData("SimpleVertexShader.cso");
    device->CreateVertexShader(
      vertexShaderByteCode_->GetBufferPointer(),
      vertexShaderByteCode_->GetBufferSize(),
      nullptr, &vertexShader_);

    device->CreatePixelShader(
      pixelShaderByteCode_->GetBufferPointer(),
      pixelShaderByteCode_->GetBufferSize(),
      nullptr, &pixelShader_);

    D3D11_INPUT_ELEMENT_DESC inputElements[] = {
        D3D11_INPUT_ELEMENT_DESC{
          .SemanticName = "POSITION",
          .SemanticIndex = 0,
          .Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
          .InputSlot = 0,
          .AlignedByteOffset = 0,
          .InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
          .InstanceDataStepRate = 0 },
        D3D11_INPUT_ELEMENT_DESC{
          .SemanticName = "COLOR",
          .SemanticIndex = 0,
          .Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
          .InputSlot = 0,
          .AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT,
          .InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
          .InstanceDataStepRate = 0 }
    };

    ID3D11InputLayout* layout;
    device->CreateInputLayout(
      inputElements,
      2,
      vertexShaderByteCode_->GetBufferPointer(),
      vertexShaderByteCode_->GetBufferSize(),
      &layout);

    D3D11_INPUT_ELEMENT_DESC inputElements2[] = {
        D3D11_INPUT_ELEMENT_DESC{
          .SemanticName = "POSITION",
          .SemanticIndex = 0,
          .Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
          .InputSlot = 0,
          .AlignedByteOffset = 0,
          .InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
          .InstanceDataStepRate = 0 },
        D3D11_INPUT_ELEMENT_DESC{
          .SemanticName = "COLOR",
          .SemanticIndex = 0,
          .Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
          .InputSlot = 1,
          .AlignedByteOffset = 0,
          .InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
          .InstanceDataStepRate = 0 }
    };

    ID3D11InputLayout* layout2;
    device->CreateInputLayout(
      inputElements2,
      2,
      vertexShaderByteCode_->GetBufferPointer(),
      vertexShaderByteCode_->GetBufferSize(),
      &layout2);

    int indices[] = { 0, 1, 2 };

    D3D11_BUFFER_DESC dataBufDesc = {
        .ByteWidth = static_cast<unsigned>(sizeof(XMFLOAT4) * std::size(positions_)),
        .Usage = D3D11_USAGE_DEFAULT,
        .BindFlags = D3D11_BIND_VERTEX_BUFFER,
        .CPUAccessFlags = 0,
        .MiscFlags = 0,
        .StructureByteStride = 0
    };

    ID3D11Buffer* pb;
    ID3D11Buffer* cb;

    D3D11_SUBRESOURCE_DATA positionsData = {
        .pSysMem = positions_.data(),
        .SysMemPitch = 0,
        .SysMemSlicePitch = 0
    };
    D3D11_SUBRESOURCE_DATA colorsData = {
        .pSysMem = colors_.data(),
        .SysMemPitch = 0,
        .SysMemSlicePitch = 0
    };

    device->CreateBuffer(&dataBufDesc, &positionsData, &pb);
    device->CreateBuffer(&dataBufDesc, &colorsData, &cb);

    D3D11_BUFFER_DESC vertexBufDesc = {
        vertexBufDesc.ByteWidth = static_cast<unsigned>(sizeof(XMFLOAT4) * std::size(points_)),
        vertexBufDesc.Usage = D3D11_USAGE_DEFAULT,
        vertexBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER,
        vertexBufDesc.CPUAccessFlags = 0,
        vertexBufDesc.MiscFlags = 0,
        vertexBufDesc.StructureByteStride = 0
    };

    D3D11_SUBRESOURCE_DATA vertexData = {
        .pSysMem = points_.data(),
        .SysMemPitch = 0,
        .SysMemSlicePitch = 0
    };

    ID3D11Buffer* vb;
    device->CreateBuffer(&vertexBufDesc, &vertexData, &vb);

    D3D11_BUFFER_DESC indexBufDesc = {
        .ByteWidth = static_cast<unsigned>(sizeof(int) * std::size(indices)),
        .Usage = D3D11_USAGE_DEFAULT,
        .BindFlags = D3D11_BIND_INDEX_BUFFER,
        .CPUAccessFlags = 0,
        .MiscFlags = 0,
        .StructureByteStride = 0
    };

    D3D11_SUBRESOURCE_DATA indexData = {
        .pSysMem = indices,
        .SysMemPitch = 0,
        .SysMemSlicePitch = 0
    };

    device->CreateBuffer(&indexBufDesc, &indexData, &indexBuffer_);

    vBuffers_[0] = pb;
    vBuffers_[1] = cb;

    layout_ = layout2;

    CD3D11_RASTERIZER_DESC rastDesc{};
    rastDesc.CullMode = D3D11_CULL_NONE;
    rastDesc.FillMode = D3D11_FILL_SOLID;
    ZCHECK(device->CreateRasterizerState(&rastDesc, &rastState_));
}

void TriangleComponent::draw(ID3D11DeviceContext* context) {
    context->IASetInputLayout(layout_);
    context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->IASetIndexBuffer(indexBuffer_, DXGI_FORMAT_R32_UINT, 0);
    context->IASetVertexBuffers(0, 2, vBuffers_, strides_, offsets_);
    context->VSSetShader(vertexShader_, nullptr, 0);
    context->PSSetShader(pixelShader_, nullptr, 0);
    context->RSSetState(rastState_);

    context->DrawIndexed(3, 0, 0);
}

void TriangleComponent::destroyResources() {
    vertexShader_->Release();
    pixelShader_->Release();
}

void TriangleComponent::reload() {
}

void TriangleComponent::update() {
}