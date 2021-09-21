#include <dxframework/mesh.hpp>

using namespace dxframework;

mesh::mesh(ID3D11Device* device, ID3D11DeviceContext* device_context, std::vector<vertex_type>& vertices, std::vector<DWORD>& indices, std::vector<texture>& textures, DirectX::XMMATRIX const& transform_matrix)
    : device_context_{ device_context }
    , textures_{ textures }
    , transform_matrix_{ transform_matrix } {
    throw_if_failed(vertex_buffer_.init(device, vertices.data(), vertices.size()));
    throw_if_failed(index_buffer_.init(device, indices.data(), indices.size()));
}

void mesh::draw() {
    uint32_t offset{ 0 };

    for (int i = 0; i < textures_.size(); i++) {
        if (textures_[i].get_type() == aiTextureType::aiTextureType_DIFFUSE) {
            device_context_->PSSetShaderResources(0, 1, textures_[i].get_texture_resource_view_address());
            break;
        }
    }

    device_context_->IASetVertexBuffers(0, 1, vertex_buffer_.get_address_of(), vertex_buffer_.stride_ptr(), &offset);
    device_context_->IASetIndexBuffer(index_buffer_.get_buffer(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
    device_context_->DrawIndexed(index_buffer_.index_count(), 0, 0);
}

DirectX::XMMATRIX const& mesh::get_transform_matrix() {
    return transform_matrix_;
}
