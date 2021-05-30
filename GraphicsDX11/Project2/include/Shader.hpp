#pragma once

#include <Utils.hpp>

#include <wrl.h>
#include <d3d11.h>

#include <span>

namespace GraphicsFramework {

struct ShaderError {
    enum class Kind {
        UNDEFINED,
        COMPILE_ERROR,
        INIT_ERROR,
    };

    Kind kind;
    std::string text;
};

template <typename T>
using ShaderResult = Result<T, ShaderError>;

class Shader {
public:
    Shader(ID3D11Device* device);
    virtual ~Shader() = default;

public:
    virtual ShaderResult<void> initWithSourceCode(
      std::span<std::byte> sourceCode,
      std::span<D3D_SHADER_MACRO> macro,
      std::string_view entryName,
      std::string_view shaderModel)
      = 0;
    virtual ShaderResult<void> initWithSourceCode(
      std::string_view fileName,
      std::span<D3D_SHADER_MACRO> macro,
      std::string_view entryName,
      std::string_view shaderModel)
      = 0;
    virtual ShaderResult<void> initWithByteCode(ID3DBlob* byteCode) = 0;
    virtual ShaderResult<void> initWithByteCode(std::string_view fileName) = 0;

public:
    static ShaderResult<Microsoft::WRL::ComPtr<ID3DBlob>> compileFromFile(
      std::string_view fileName,
      std::span<D3D_SHADER_MACRO> macro,
      std::string_view entryName,
      std::string_view shaderModel);
    static ShaderResult<Microsoft::WRL::ComPtr<ID3DBlob>> compileSource(
      std::span<std::byte> sourceCode,
      std::span<D3D_SHADER_MACRO> macro,
      std::string_view entryName,
      std::string_view shaderModel);

protected:
    ID3D11Device* device_{ nullptr };
};

class VertexShader : public Shader {
public:
    VertexShader(ID3D11Device* device);
    ~VertexShader() override = default;

public:
    ShaderResult<void> initWithSourceCode(
      std::span<std::byte> sourceCode,
      std::span<D3D_SHADER_MACRO> macro,
      std::string_view entryName,
      std::string_view shaderModel) override;
    ShaderResult<void> initWithSourceCode(
      std::string_view fileName,
      std::span<D3D_SHADER_MACRO> macro,
      std::string_view entryName,
      std::string_view shaderModel) override;
    ShaderResult<void> initWithByteCode(ID3DBlob* byteCode) override;
    ShaderResult<void> initWithByteCode(std::string_view fileName) override;

private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader> shader_;
};

class PixelShader : public Shader {
public:
    PixelShader(ID3D11Device* device);
    ~PixelShader() override = default;

public:
    ShaderResult<void> initWithSourceCode(
      std::span<std::byte> sourceCode,
      std::span<D3D_SHADER_MACRO> macro,
      std::string_view entryName,
      std::string_view shaderModel) override;
    ShaderResult<void> initWithSourceCode(
      std::string_view fileName,
      std::span<D3D_SHADER_MACRO> macro,
      std::string_view entryName,
      std::string_view shaderModel) override;
    ShaderResult<void> initWithByteCode(ID3DBlob* byteCode) override;
    ShaderResult<void> initWithByteCode(std::string_view fileName) override;

private:
    Microsoft::WRL::ComPtr<ID3D11PixelShader> shader_;
};

} // namespace GraphicsFramework