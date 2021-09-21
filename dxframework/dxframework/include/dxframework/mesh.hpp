#pragma once

#include <dxframework/texture.hpp>
#include <dxframework/vertex.hpp>
#include <dxframework/shader.hpp>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <vector>

namespace dxframework {

class mesh {
public:
    mesh(ID3D11Device* device, ID3D11DeviceContext* device_context, std::vector<vertex_type>& vertices, std::vector<DWORD>& indices, std::vector<texture>& textures, DirectX::XMMATRIX const& transform_matrix);
    mesh(mesh const&) = default;
    mesh(mesh&&) = default;
    mesh& operator=(mesh const&) = default;
    mesh& operator=(mesh&&) = default;
    ~mesh() = default;

    void draw();
    DirectX::XMMATRIX const& get_transform_matrix();

private:
    vertex_buffer<vertex_type> vertex_buffer_;
    index_buffer<DWORD> index_buffer_;
    ID3D11DeviceContext* device_context_;
    std::vector<texture> textures_;
    DirectX::XMMATRIX transform_matrix_;
};

} // namespace dxframework
