#pragma once

#include <dxframework/constant_buffer_types.hpp>
#include <dxframework/utils.hpp>

#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>

#include <span>

namespace dxframework {

class shader {
public:
    shader() = default;
    shader(ID3D11Device* device);
    shader(shader const&) = default;
    shader(shader&&) = default;
    shader& operator=(shader const&) = default;
    shader& operator=(shader&&) = default;
    virtual ~shader() = default;

public:
    virtual void init_with_source_code(
      std::span<std::byte> source_code,
      std::span<D3D_SHADER_MACRO> macro,
      std::string_view entry_name,
      std::string_view shader_model)
      = 0;
    virtual void init_with_source_code(
      std::string_view file_name,
      std::span<D3D_SHADER_MACRO> macro,
      std::string_view entry_name,
      std::string_view shader_model)
      = 0;
    virtual void init_with_byte_code(Microsoft::WRL::ComPtr<ID3DBlob> byte_code) = 0;
    virtual void init_with_byte_code(std::string_view file_name) = 0;
    virtual void* get_shader() const = 0;
    virtual Microsoft::WRL::ComPtr<ID3DBlob> get_byte_code() const = 0;

public:
    static Microsoft::WRL::ComPtr<ID3DBlob> compile_from_file(
      std::string_view file_name,
      std::span<D3D_SHADER_MACRO> macro,
      std::string_view entry_name,
      std::string_view shader_model);
    static Microsoft::WRL::ComPtr<ID3DBlob> compile_source(
      std::span<std::byte> source_code,
      std::span<D3D_SHADER_MACRO> macro,
      std::string_view entry_name,
      std::string_view shader_model);

protected:
    ID3D11Device* device_{ nullptr };
    Microsoft::WRL::ComPtr<ID3DBlob> byte_code_;
};

class vertex_shader : public shader {
public:
    vertex_shader() = default;
    vertex_shader(ID3D11Device* device, std::vector<D3D11_INPUT_ELEMENT_DESC> desc);
    vertex_shader(vertex_shader const&) = default;
    vertex_shader(vertex_shader&&) = default;
    vertex_shader& operator=(vertex_shader const&) = default;
    vertex_shader& operator=(vertex_shader&&) = default;
    ~vertex_shader() override = default;

public:
    void init_with_source_code(
      std::span<std::byte> source_code,
      std::span<D3D_SHADER_MACRO> macro,
      std::string_view entry_name,
      std::string_view shader_model) override;
    void init_with_source_code(
      std::string_view file_name,
      std::span<D3D_SHADER_MACRO> macro,
      std::string_view entry_name,
      std::string_view shader_model) override;
    void init_with_byte_code(Microsoft::WRL::ComPtr<ID3DBlob> byte_code) override;
    void init_with_byte_code(std::string_view file_name) override;
    void* get_shader() const override;
    Microsoft::WRL::ComPtr<ID3DBlob> get_byte_code() const override;

public:
    ID3D11InputLayout* get_input_layout();

private:
    void create_();

private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader> shader_;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout_;
    std::vector<D3D11_INPUT_ELEMENT_DESC> desc_;
};

class pixel_shader : public shader {
public:
    pixel_shader() = default;
    pixel_shader(ID3D11Device* device);
    pixel_shader(pixel_shader const&) = default;
    pixel_shader(pixel_shader&&) = default;
    pixel_shader& operator=(pixel_shader const&) = default;
    pixel_shader& operator=(pixel_shader&&) = default;
    ~pixel_shader() override = default;

public:
    void init_with_source_code(
      std::span<std::byte> source_code,
      std::span<D3D_SHADER_MACRO> macro,
      std::string_view entry_name,
      std::string_view shader_model) override;
    void init_with_source_code(
      std::string_view file_name,
      std::span<D3D_SHADER_MACRO> macro,
      std::string_view entry_name,
      std::string_view shader_model) override;
    void init_with_byte_code(Microsoft::WRL::ComPtr<ID3DBlob> byte_code) override;
    void init_with_byte_code(std::string_view file_name) override;
    void* get_shader() const override;
    Microsoft::WRL::ComPtr<ID3DBlob> get_byte_code() const override;

private:
    Microsoft::WRL::ComPtr<ID3D11PixelShader> shader_;
};

class input_layout {
public:
    input_layout() = default;
    input_layout(input_layout const&) = delete;
    input_layout(input_layout&&) = default;
    input_layout& operator=(input_layout&&) = default;
    input_layout& operator=(input_layout const&) = delete;
};

template <typename VertexT>
class vertex_buffer {
public:
    vertex_buffer() = default;
    vertex_buffer(vertex_buffer const&) = delete;
    vertex_buffer(vertex_buffer&&) = default;
    vertex_buffer& operator=(vertex_buffer&&) = default;
    vertex_buffer& operator=(vertex_buffer const&) = delete;

public:
    HRESULT init(ID3D11Device* device, VertexT* data, uint32_t vertex_count) {
        buffer_.Reset();

        vertex_count_ = vertex_count;

        D3D11_BUFFER_DESC vertexBufferDesc{
            .ByteWidth = stride_ * vertex_count_,
            .Usage = D3D11_USAGE_DEFAULT,
            .BindFlags = D3D11_BIND_VERTEX_BUFFER,
            .CPUAccessFlags = 0,
            .MiscFlags = 0,
            .StructureByteStride = 0
        };

        D3D11_SUBRESOURCE_DATA vertexBufferData{
            .pSysMem = data,
            .SysMemPitch = 0,
            .SysMemSlicePitch = 0
        };

        return device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, buffer_.GetAddressOf());
    }

    auto get_buffer() {
        return buffer_.Get();
    }

    auto get_address_of() {
        return buffer_.GetAddressOf();
    }

    auto const stride() {
        return stride_;
    }

    auto const stride_ptr() {
        return &stride_;
    }

private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> buffer_;
    uint32_t stride_{ sizeof(VertexT) };
    uint32_t vertex_count_{ 0 };
};

template <typename IndexT>
class index_buffer {
public:
    index_buffer() = default;
    index_buffer(index_buffer const&) = delete;
    index_buffer(index_buffer&&) = default;
    index_buffer& operator=(index_buffer&&) = default;
    index_buffer& operator=(index_buffer const&) = delete;
    ~index_buffer() = default;

public:
    HRESULT init(ID3D11Device* device, IndexT* data, uint32_t index_count) {
        buffer_.Reset();

        index_count_ = index_count;

        D3D11_BUFFER_DESC indexBufferDesc{
            .ByteWidth = stride_ * index_count_,
            .Usage = D3D11_USAGE_DEFAULT,
            .BindFlags = D3D11_BIND_INDEX_BUFFER,
            .CPUAccessFlags = 0,
            .MiscFlags = 0,
            .StructureByteStride = 0
        };

        D3D11_SUBRESOURCE_DATA indexBufferData{
            .pSysMem = data,
            .SysMemPitch = 0,
            .SysMemSlicePitch = 0
        };

        return device->CreateBuffer(&indexBufferDesc, &indexBufferData, buffer_.GetAddressOf());
    }

    auto get_buffer() {
        return buffer_.Get();
    }

    auto index_count() {
        return index_count_;
    }

private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> buffer_;
    uint32_t stride_{ sizeof(IndexT) };
    uint32_t index_count_{ 0 };
};

template <class T>
class constant_buffer {
public:
    constant_buffer() {
    }
    constant_buffer(constant_buffer<T> const&) = delete;
    constant_buffer(constant_buffer<T>&&) = default;
    constant_buffer& operator=(constant_buffer<T> const&) = delete;
    constant_buffer& operator=(constant_buffer<T>&&) = delete;
    ~constant_buffer() = default;


    ID3D11Buffer* get_buffer() const {
        return buffer_.Get();
    }

    auto get_address_of() const {
        return buffer_.GetAddressOf();
    }

    T& get_data() {
        return data_;
    }

    T const& get_data() const {
        return data_;
    }

    HRESULT init(ID3D11Device* device, ID3D11DeviceContext* device_context) {
        buffer_.Reset();

        device_context_ = device_context;

        D3D11_BUFFER_DESC desc{
            .ByteWidth = static_cast<UINT>(sizeof(T) + (16 - (sizeof(T) % 16))),
            .Usage = D3D11_USAGE_DYNAMIC,
            .BindFlags = D3D11_BIND_CONSTANT_BUFFER,
            .CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
            .MiscFlags = 0,
            .StructureByteStride = 0
        };

        return device->CreateBuffer(&desc, 0, buffer_.GetAddressOf());
    }

    bool apply_changes() {
        D3D11_MAPPED_SUBRESOURCE mapped_resource;
        throw_if_failed(device_context_->Map(buffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource));
        CopyMemory(mapped_resource.pData, &data_, sizeof(T));
        device_context_->Unmap(buffer_.Get(), 0);
        return true;
    }

private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> buffer_;
    ID3D11DeviceContext* device_context_ = nullptr;
    T data_;
};

} // namespace dxframework