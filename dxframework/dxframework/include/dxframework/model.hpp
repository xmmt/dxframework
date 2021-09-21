#pragma once

#include <dxframework/mesh.hpp>

namespace dxframework {

class model {
public:
    bool init(std::string const& file_path, ID3D11Device* device, ID3D11DeviceContext* device_context, constant_buffer<CB_VS_vertexshader>& cb_vs_vertexshader);
    void draw(DirectX::XMMATRIX const& world_matrix, DirectX::XMMATRIX const& view_projection_matrix);

private:
    bool load_model_(std::string const& file_path);
    void process_node_(aiNode* node, aiScene const* scene, DirectX::XMMATRIX const& parent_transform_matrix);
    mesh process_mesh_(aiMesh* mesh, aiScene const* scene, DirectX::XMMATRIX const& transform_matrix);
    texture_storage_type determine_texture_storage_type_(aiScene const* p_scene, aiMaterial* p_mat, unsigned int index, aiTextureType texture_type);
    std::vector<texture> load_material_textures_(aiMaterial* p_material, aiTextureType texture_type, aiScene const* p_scene);
    int get_texture_index_(aiString* p_str);

private:
    std::vector<mesh> meshes_;
    ID3D11Device* device_{ nullptr };
    ID3D11DeviceContext* device_context_{ nullptr };
    constant_buffer<CB_VS_vertexshader>* cb_vs_vertexshader_{ nullptr };
    std::string directory_;
};

} // namespace dxframework
