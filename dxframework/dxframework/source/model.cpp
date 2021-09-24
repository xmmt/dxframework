#include <dxframework/model.hpp>

using namespace dxframework;
using namespace DirectX;

bool model::init(std::string const& file_path, ID3D11Device* device, ID3D11DeviceContext* device_context, constant_buffer<CB_VS_vertexshader>& cb_vs_vertexshader) {
    device_ = device;
    device_context_ = device_context;
    cb_vs_vertexshader_ = &cb_vs_vertexshader;

    //try {
        if (!load_model_(file_path))
            return false;
    //} catch (COMException& exception) {
    //    ErrorLogger::Log(exception);
    //    return false;
    //}

    return true;
}

void model::draw(XMMATRIX const& world_matrix, XMMATRIX const& view_projection_matrix) {
    device_context_->VSSetConstantBuffers(0, 1, cb_vs_vertexshader_->get_address_of());

    for (auto& i : meshes_) {
        cb_vs_vertexshader_->get_data().wvp_matrix = i.get_transform_matrix() * world_matrix * view_projection_matrix; //Calculate World-View-Projection Matrix
        cb_vs_vertexshader_->get_data().world_matrix = i.get_transform_matrix() * world_matrix;                        //Calculate World
        cb_vs_vertexshader_->apply_changes();
        i.draw();
    }
}

bool model::load_model_(std::string const& file_path) {
    directory_ = utils::converter::get_directory_from_path(file_path) + '/';

    Assimp::Importer importer;

    aiScene const* p_scene = importer.ReadFile(file_path, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);

    if (p_scene == nullptr)
        return false;

    process_node_(p_scene->mRootNode, p_scene, DirectX::XMMatrixIdentity());
    return true;
}

void model::process_node_(aiNode* node, aiScene const* scene, XMMATRIX const& parent_transform_matrix) {
    XMMATRIX node_transform_matrix = XMMatrixTranspose(XMMATRIX(&node->mTransformation.a1)) * parent_transform_matrix;

    for (uint32_t i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes_.push_back(this->process_mesh_(mesh, scene, node_transform_matrix));
    }

    for (uint32_t i = 0; i < node->mNumChildren; i++) {
        this->process_node_(node->mChildren[i], scene, node_transform_matrix);
    }
}

mesh model::process_mesh_(aiMesh* mesh, aiScene const* scene, XMMATRIX const& transform_matrix) {
    // Data to fill
    std::vector<vertex_type> vertices;
    std::vector<DWORD> indices;

    //Get vertices
    for (uint32_t i = 0; i < mesh->mNumVertices; i++) {
        vertex_type vertex;

        vertex.pos.x = mesh->mVertices[i].x;
        vertex.pos.y = mesh->mVertices[i].y;
        vertex.pos.z = mesh->mVertices[i].z;

        if (mesh->mNormals) {
            vertex.normal.x = mesh->mNormals[i].x;
            vertex.normal.y = mesh->mNormals[i].y;
            vertex.normal.z = mesh->mNormals[i].z;
        } else {
            vertex.normal.x = 0.f;
            vertex.normal.y = 1.f;
            vertex.normal.z = 0.f;
        }

        if (mesh->mTextureCoords[0]) {
            vertex.tex_coord.x = (float)mesh->mTextureCoords[0][i].x;
            vertex.tex_coord.y = (float)mesh->mTextureCoords[0][i].y;
        }

        vertices.push_back(vertex);
    }

    //Get indices
    for (uint32_t i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];

        for (uint32_t j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    std::vector<texture> textures;
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    std::vector<texture> diffuse_textures = load_material_textures_(material, aiTextureType::aiTextureType_DIFFUSE, scene);
    textures.insert(textures.end(), diffuse_textures.begin(), diffuse_textures.end());

    return ::mesh{ device_, device_context_, vertices, indices, textures, transform_matrix };
}

texture_storage_type model::determine_texture_storage_type_(aiScene const* p_scene, aiMaterial* p_mat, unsigned int index, aiTextureType texture_type) {
    if (p_mat->GetTextureCount(texture_type) == 0)
        return texture_storage_type::NONE;

    aiString path;
    p_mat->GetTexture(texture_type, index, &path);
    std::string texture_path{ path.C_Str() };
    //Check if texture is an embedded indexed texture by seeing if the file path is an index #
    if (texture_path[0] == '*') {
        if (p_scene->mTextures[0]->mHeight == 0) {
            return texture_storage_type::EMBEDDED_INDEX_COMPRESSED;
        } else {
            assert("SUPPORT DOES NOT EXIST YET FOR INDEXED NON COMPRESSED TEXTURES!" && 0);
            return texture_storage_type::EMBEDDED_INDEX_NON_COMPRESSED;
        }
    }
    //Check if texture is an embedded texture but not indexed (path will be the texture's name instead of #)
    if (auto p_tex = p_scene->GetEmbeddedTexture(texture_path.c_str())) {
        if (p_tex->mHeight == 0) {
            return texture_storage_type::EMBEDDED_COMPRESSED;
        } else {
            assert("SUPPORT DOES NOT EXIST YET FOR EMBEDDED NON COMPRESSED TEXTURES!" && 0);
            return texture_storage_type::EMBEDDED_NON_COMPRESSED;
        }
    }
    //Lastly check if texture is a filepath by checking for period before extension name
    if (texture_path.find('.') != std::string::npos) {
        return texture_storage_type::DISK;
    }

    return texture_storage_type::NONE; // No texture exists
}

std::vector<texture> model::load_material_textures_(aiMaterial* p_material, aiTextureType texture_type, aiScene const* p_scene) {
    std::vector<texture> material_textures;
    texture_storage_type storetype = texture_storage_type::INVALID;
    unsigned int texture_count = p_material->GetTextureCount(texture_type);

    if (texture_count == 0) //if there are no textures
    {
        storetype = texture_storage_type::NONE;
        aiColor3D ai_color(0.0f, 0.0f, 0.0f);
        switch (texture_type) {
        case aiTextureType_DIFFUSE:
            p_material->Get(AI_MATKEY_COLOR_DIFFUSE, ai_color);
            if (ai_color.IsBlack()) //if color = black, just use grey
            {
                material_textures.push_back(texture{ device_, colors::unloaded_texture_color, texture_type });
                return material_textures;
            }
            material_textures.push_back(texture{ device_, color(ai_color.r * 255, ai_color.g * 255, ai_color.b * 255), texture_type });
            return material_textures;
        }
    } else {
        for (uint32_t i = 0; i < texture_count; i++) {
            aiString path;
            p_material->GetTexture(texture_type, i, &path);
            texture_storage_type storetype = determine_texture_storage_type_(p_scene, p_material, i, texture_type);
            switch (storetype) {
            case texture_storage_type::DISK: {
                std::string filename = directory_ /* + '\\'*/ + path.C_Str();
                texture disk_texture(device_, filename, texture_type);
                material_textures.push_back(disk_texture);
                break;
            }
            }
        }
    }
    if (material_textures.size() == 0) {
        material_textures.push_back(texture{ device_, colors::unhandled_texture_color, aiTextureType::aiTextureType_DIFFUSE });
    }
    return material_textures;
}