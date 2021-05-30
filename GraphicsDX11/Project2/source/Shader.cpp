#include <Shader.hpp>

#include <wrl.h>
#include <d3dcompiler.h>

using namespace GraphicsFramework;

Shader::Shader(ID3D11Device* device)
    : device_{ device } {
}

ShaderResult<Microsoft::WRL::ComPtr<ID3DBlob>> Shader::compileFromFile(std::string_view fileName, std::span<D3D_SHADER_MACRO> macro, std::string_view entryName, std::string_view shaderModel) {
    Microsoft::WRL::ComPtr<ID3DBlob> errorCode;
    Microsoft::WRL::ComPtr<ID3DBlob> byteCode;

    auto res = D3DCompileFromFile(
      std::wstring(fileName.cbegin(), fileName.cend()).c_str(),
      macro.empty() ? nullptr : macro.data(),
      nullptr, // include,
      entryName.data(),
      shaderModel.data(),
#if _DEBUG
      D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_DEBUG_NAME_FOR_BINARY,
#else
      D3DCOMPILE_OPTIMIZATION_LEVEL3,
#endif
      0,
      byteCode.GetAddressOf(),
      errorCode.GetAddressOf());

    if (FAILED(res)) {
        // If the shader failed to compile it should have written something to the error message.
        if (errorCode) {
            char* compileErrors = (char*)(errorCode->GetBufferPointer());
            return Err(ShaderError{
              .kind = ShaderError::Kind::COMPILE_ERROR,
              .text = std::format("Shader compile error: {}, file: {}", compileErrors, fileName) });
        }
        // If there was  nothing in the error message then it simply could not find the shader file itself.
        else {
            return Err(ShaderError{
              .kind = ShaderError::Kind::COMPILE_ERROR,
              .text = std::format("Shader compile error, maybe missing file: {}", fileName) });
        }
    }

#if _DEBUG
    Microsoft::WRL::ComPtr<ID3DBlob> pPDB;
    D3DGetBlobPart(byteCode->GetBufferPointer(), byteCode->GetBufferSize(), D3D_BLOB_PDB, 0, pPDB.GetAddressOf());
    Microsoft::WRL::ComPtr<ID3DBlob> pPDBName;
    D3DGetBlobPart(byteCode->GetBufferPointer(), byteCode->GetBufferSize(), D3D_BLOB_DEBUG_NAME, 0, pPDBName.GetAddressOf());
    struct ShaderDebugName {
        uint16_t Flags;      // Reserved, must be set to zero.
        uint16_t NameLength; // Length of the debug name, without null terminator.
                             // Followed by NameLength bytes of the UTF-8-encoded name.
                             // Followed by a null terminator.
                             // Followed by [0-3] zero bytes to align to a 4-byte boundary.
    };
    auto pDebugNameData = reinterpret_cast<const ShaderDebugName*>(pPDBName->GetBufferPointer());
    auto pName = reinterpret_cast<const char*>(pDebugNameData + 1);
    D3DWriteBlobToFile(pPDB.Get(), std::wstring(pName, pName + pDebugNameData->NameLength).c_str(), true);
#endif

    return Ok(byteCode);
}

ShaderResult<Microsoft::WRL::ComPtr<ID3DBlob>> Shader::compileSource(
  std::span<std::byte> sourceCode,
  std::span<D3D_SHADER_MACRO> macro,
  std::string_view entryName,
  std::string_view shaderModel) {
    return Err(ShaderError{
      .kind = ShaderError::Kind::UNDEFINED,
      .text = "not implemented" });
}

VertexShader::VertexShader(ID3D11Device* device)
    : Shader{ device } {
}

ShaderResult<void> VertexShader::initWithSourceCode(
  std::span<std::byte> sourceCode,
  std::span<D3D_SHADER_MACRO> macro,
  std::string_view entryName,
  std::string_view shaderModel) {
    auto res = compileSource(sourceCode, macro, entryName, shaderModel);
    if (res.isErr()) {
        return Err(res.unwrapErr());
    }
    if (FAILED(device_->CreateVertexShader(
          res.unwrap()->GetBufferPointer(),
          res.unwrap()->GetBufferSize(),
          nullptr, shader_.GetAddressOf()))) {
        return Err(ShaderError{
          .kind = ShaderError::Kind::INIT_ERROR,
          .text = "can not create vertex shader" });
    }
    return Ok();
}

ShaderResult<void> VertexShader::initWithSourceCode(
  std::string_view fileName,
  std::span<D3D_SHADER_MACRO> macro,
  std::string_view entryName,
  std::string_view shaderModel) {
    auto res = compileFromFile(fileName, macro, entryName, shaderModel);
    if (res.isErr()) {
        return Err(res.unwrapErr());
    }
    if (FAILED(device_->CreateVertexShader(
          res.unwrap()->GetBufferPointer(),
          res.unwrap()->GetBufferSize(),
          nullptr, shader_.GetAddressOf()))) {
        return Err(ShaderError{
          .kind = ShaderError::Kind::INIT_ERROR,
          .text = "can not create vertex shader" });
    }
    return Ok();
}

ShaderResult<void> VertexShader::initWithByteCode(ID3DBlob* byteCode) {
    if (FAILED(device_->CreateVertexShader(
          byteCode->GetBufferPointer(),
          byteCode->GetBufferSize(),
          nullptr, shader_.GetAddressOf()))) {
        return Err(ShaderError{
          .kind = ShaderError::Kind::INIT_ERROR,
          .text = "can not create vertex shader" });
    }
    return Ok();
}

ShaderResult<void> VertexShader::initWithByteCode(std::string_view fileName) {
    Microsoft::WRL::ComPtr<ID3DBlob> byteCode;
    if (FAILED(D3DReadFileToBlob(
          std::wstring(fileName.cbegin(), fileName.cend()).c_str(),
          byteCode.GetAddressOf()))) {
        return Err(ShaderError{
          .kind = ShaderError::Kind::INIT_ERROR,
          .text = std::format("can not read vertex bytecode, file: {}", fileName) });
    }
    if (FAILED(device_->CreateVertexShader(
          byteCode->GetBufferPointer(),
          byteCode->GetBufferSize(),
          nullptr, shader_.GetAddressOf()))) {
        return Err(ShaderError{
          .kind = ShaderError::Kind::INIT_ERROR,
          .text = "can not create vertex shader" });
    }
    return Ok();
}

PixelShader::PixelShader(ID3D11Device* device)
    : Shader{ device } {
}

ShaderResult<void> PixelShader::initWithSourceCode(
  std::span<std::byte> sourceCode,
  std::span<D3D_SHADER_MACRO> macro,
  std::string_view entryName,
  std::string_view shaderModel) {
    auto res = compileSource(sourceCode, macro, entryName, shaderModel);
    if (res.isErr()) {
        return Err(res.unwrapErr());
    }
    if (FAILED(device_->CreatePixelShader(
          res.unwrap()->GetBufferPointer(),
          res.unwrap()->GetBufferSize(),
          nullptr, shader_.GetAddressOf()))) {
        return Err(ShaderError{
          .kind = ShaderError::Kind::INIT_ERROR,
          .text = "can not create vertex shader" });
    }
    return Ok();
}

ShaderResult<void> PixelShader::initWithSourceCode(
  std::string_view fileName,
  std::span<D3D_SHADER_MACRO> macro,
  std::string_view entryName,
  std::string_view shaderModel) {
    auto res = compileFromFile(fileName, macro, entryName, shaderModel);
    if (res.isErr()) {
        return Err(res.unwrapErr());
    }
    if (FAILED(device_->CreatePixelShader(
          res.unwrap()->GetBufferPointer(),
          res.unwrap()->GetBufferSize(),
          nullptr, shader_.GetAddressOf()))) {
        return Err(ShaderError{
          .kind = ShaderError::Kind::INIT_ERROR,
          .text = "can not create vertex shader" });
    }
    return Ok();
}

ShaderResult<void> PixelShader::initWithByteCode(ID3DBlob* byteCode) {
    if (FAILED(device_->CreatePixelShader(
          byteCode->GetBufferPointer(),
          byteCode->GetBufferSize(),
          nullptr, shader_.GetAddressOf()))) {
        return Err(ShaderError{
          .kind = ShaderError::Kind::INIT_ERROR,
          .text = "can not create vertex shader" });
    }
    return Ok();
}

ShaderResult<void> PixelShader::initWithByteCode(std::string_view fileName) {
    Microsoft::WRL::ComPtr<ID3DBlob> byteCode;
    if (FAILED(D3DReadFileToBlob(
          std::wstring(fileName.cbegin(), fileName.cend()).c_str(),
          byteCode.GetAddressOf()))) {
        return Err(ShaderError{
          .kind = ShaderError::Kind::INIT_ERROR,
          .text = std::format("can not read vertex bytecode, file: {}", fileName) });
    }
    if (FAILED(device_->CreatePixelShader(
          byteCode->GetBufferPointer(),
          byteCode->GetBufferSize(),
          nullptr, shader_.GetAddressOf()))) {
        return Err(ShaderError{
          .kind = ShaderError::Kind::INIT_ERROR,
          .text = "can not create vertex shader" });
    }
    return Ok();
}