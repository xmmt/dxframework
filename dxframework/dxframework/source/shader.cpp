#include <dxframework/shader.hpp>

#include <wrl.h>
#include <d3dcompiler.h>

using namespace dxframework;

shader::shader(ID3D11Device* device)
    : device_{ device } {
}

Microsoft::WRL::ComPtr<ID3DBlob> shader::compile_from_file(std::string_view file_name, std::span<D3D_SHADER_MACRO> macro, std::string_view entry_name, std::string_view shader_model) {
    Microsoft::WRL::ComPtr<ID3DBlob> error_code;
    Microsoft::WRL::ComPtr<ID3DBlob> byte_code;

    auto res = D3DCompileFromFile(
      std::wstring(file_name.cbegin(), file_name.cend()).c_str(),
      macro.empty() ? nullptr : macro.data(),
      nullptr, // include,
      entry_name.data(),
      shader_model.data(),
#if _DEBUG
      D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_DEBUG_NAME_FOR_BINARY,
#else
      D3DCOMPILE_OPTIMIZATION_LEVEL3,
#endif
      0,
      byte_code.GetAddressOf(),
      error_code.GetAddressOf());

    if (FAILED(res)) {
        // If the shader failed to compile it should have written something to the error message.
        if (error_code) {
            char* compileErrors = (char*)(error_code->GetBufferPointer());
            throw std::runtime_error(std::format("shader compile error: {}, file: {}", compileErrors, file_name));
        }
        // If there was  nothing in the error message then it simply could not find the shader file itself.
        else {
            throw std::runtime_error(std::format("shader compile error, maybe missing file: {}", file_name));
        }
    }

#if _DEBUG
    Microsoft::WRL::ComPtr<ID3DBlob> p_PDB;
    D3DGetBlobPart(byte_code->GetBufferPointer(), byte_code->GetBufferSize(), D3D_BLOB_PDB, 0, p_PDB.GetAddressOf());
    Microsoft::WRL::ComPtr<ID3DBlob> p_PDB_name;
    D3DGetBlobPart(byte_code->GetBufferPointer(), byte_code->GetBufferSize(), D3D_BLOB_DEBUG_NAME, 0, p_PDB_name.GetAddressOf());
    struct shader_debug_name {
        uint16_t flags;       // Reserved, must be set to zero.
        uint16_t name_length; // Length of the debug name, without null terminator.
                              // Followed by NameLength bytes of the UTF-8-encoded name.
                              // Followed by a null terminator.
                              // Followed by [0-3] zero bytes to align to a 4-byte boundary.
    };
    auto p_debug_name_data = reinterpret_cast<const shader_debug_name*>(p_PDB_name->GetBufferPointer());
    auto p_name = reinterpret_cast<const char*>(p_debug_name_data + 1);
    D3DWriteBlobToFile(p_PDB.Get(), std::wstring(p_name, p_name + p_debug_name_data->name_length).c_str(), true);
#endif

    return byte_code;
}

Microsoft::WRL::ComPtr<ID3DBlob> shader::compile_source(
  std::span<std::byte> source_code,
  std::span<D3D_SHADER_MACRO> macro,
  std::string_view entry_name,
  std::string_view shader_model) {
    //static_assert(false, "not implemented");
    throw std::runtime_error("not implemented");
}

vertex_shader::vertex_shader(ID3D11Device* device, std::vector<D3D11_INPUT_ELEMENT_DESC> desc)
    : shader{ device }
    , desc_{ std::move(desc) } {
}

void vertex_shader::init_with_source_code(
  std::span<std::byte> source_code,
  std::span<D3D_SHADER_MACRO> macro,
  std::string_view entry_name,
  std::string_view shader_model) {
    byte_code_ = compile_source(source_code, macro, entry_name, shader_model);
    create_();
}

void vertex_shader::init_with_source_code(
  std::string_view file_name,
  std::span<D3D_SHADER_MACRO> macro,
  std::string_view entry_name,
  std::string_view shader_model) {
    byte_code_ = compile_from_file(file_name, macro, entry_name, shader_model);
    create_();
}

void vertex_shader::init_with_byte_code(Microsoft::WRL::ComPtr<ID3DBlob> byte_code) {
    byte_code_ = byte_code;
    create_();
}

void vertex_shader::init_with_byte_code(std::string_view file_name) {
    throw_if_failed(D3DReadFileToBlob(
      std::wstring(file_name.cbegin(), file_name.cend()).c_str(),
      byte_code_.GetAddressOf()));
    create_();
}

void* vertex_shader::get_shader() const {
    return shader_.Get();
}

Microsoft::WRL::ComPtr<ID3DBlob> vertex_shader::get_byte_code() const {
    return byte_code_;
}

ID3D11InputLayout* vertex_shader::get_input_layout() {
    return input_layout_.Get();
}

void vertex_shader::create_() {
    throw_if_failed(device_->CreateVertexShader(
      byte_code_->GetBufferPointer(),
      byte_code_->GetBufferSize(),
      nullptr, shader_.GetAddressOf()));
    throw_if_failed(device_->CreateInputLayout(desc_.data(), desc_.size(), byte_code_->GetBufferPointer(), byte_code_->GetBufferSize(), input_layout_.GetAddressOf()));
}

pixel_shader::pixel_shader(ID3D11Device* device)
    : shader{ device } {
}

void pixel_shader::init_with_source_code(
  std::span<std::byte> source_code,
  std::span<D3D_SHADER_MACRO> macro,
  std::string_view entry_name,
  std::string_view shader_model) {
    byte_code_ = compile_source(source_code, macro, entry_name, shader_model);
    throw_if_failed(device_->CreatePixelShader(
      byte_code_->GetBufferPointer(),
      byte_code_->GetBufferSize(),
      nullptr, shader_.GetAddressOf()));
}

void pixel_shader::init_with_source_code(
  std::string_view file_name,
  std::span<D3D_SHADER_MACRO> macro,
  std::string_view entry_name,
  std::string_view shader_model) {
    byte_code_ = compile_from_file(file_name, macro, entry_name, shader_model);
    throw_if_failed(device_->CreatePixelShader(
      byte_code_->GetBufferPointer(),
      byte_code_->GetBufferSize(),
      nullptr, shader_.GetAddressOf()));
}

void pixel_shader::init_with_byte_code(Microsoft::WRL::ComPtr<ID3DBlob> byte_code) {
    byte_code_ = byte_code;
    throw_if_failed(device_->CreatePixelShader(
      byte_code_->GetBufferPointer(),
      byte_code_->GetBufferSize(),
      nullptr, shader_.GetAddressOf()));
}

void pixel_shader::init_with_byte_code(std::string_view file_name) {
    throw_if_failed(D3DReadFileToBlob(
      std::wstring(file_name.cbegin(), file_name.cend()).c_str(),
      byte_code_.GetAddressOf()));
    throw_if_failed(device_->CreatePixelShader(
      byte_code_->GetBufferPointer(),
      byte_code_->GetBufferSize(),
      nullptr, shader_.GetAddressOf()));
}

void* pixel_shader::get_shader() const {
    return shader_.Get();
}

Microsoft::WRL::ComPtr<ID3DBlob> pixel_shader::get_byte_code() const {
    return byte_code_;
}
