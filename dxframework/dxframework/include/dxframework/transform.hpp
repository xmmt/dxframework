#pragma once

#include <d3d11.h>
#include <SimpleMath.h>

#include <memory>
#include <vector>

namespace dxframework {

struct transform {
public:
    transform() = default;
    transform(DirectX::SimpleMath::Vector3 pos);
    ~transform() = default;

    transform& get_parent() const;
    std::vector<std::shared_ptr<transform>>& get_child();
    void add_child(transform* obj);
    void set_parent(transform* parent);

    DirectX::SimpleMath::Vector3 get_position() const;
    DirectX::SimpleMath::Vector3 get_world_position() const;
    void set_position(DirectX::SimpleMath::Vector3 pos);
    void add_position(DirectX::SimpleMath::Vector3 pos);
    void set_rotation(float x, float y, float z);

    void add_local_rotation(DirectX::SimpleMath::Vector3 axis, float angle);

    DirectX::SimpleMath::Matrix get_world_matrix() const;

private:
    DirectX::SimpleMath::Matrix m_world_{ DirectX::SimpleMath::Matrix::Identity };
    DirectX::SimpleMath::Matrix m_translation_{ DirectX::SimpleMath::Matrix::Identity };
    DirectX::SimpleMath::Matrix m_rotation_{ DirectX::SimpleMath::Matrix::Identity };
    DirectX::SimpleMath::Matrix m_scale_{ DirectX::SimpleMath::Matrix::Identity };

    DirectX::XMFLOAT3 rot_;

    std::unique_ptr<transform> parent_{ nullptr };
    std::vector<std::shared_ptr<transform>> children_;
};

} // namespace dxframework