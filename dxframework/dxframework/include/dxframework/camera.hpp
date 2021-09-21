#pragma once

#include <DirectXMath.h>

namespace dxframework {

class camera {
public:
    camera();
    camera(camera const&) = default;
    camera(camera&&) = default;
    camera& operator=(camera const&) = default;
    camera& operator=(camera&&) = default;
    ~camera() = default;

public:
    void set_projection(float fov_degrees, float aspect_ratio, float near_z, float far_z);

    const DirectX::XMMATRIX& get_view_matrix() const;
    const DirectX::XMMATRIX& get_projection_matrix() const;

    const DirectX::XMVECTOR& get_position_vector() const;
    const DirectX::XMFLOAT3& get_position_float_3() const;
    const DirectX::XMVECTOR& get_rotation_vector() const;
    const DirectX::XMFLOAT3& get_rotation_float_3() const;

    void set_position(const DirectX::XMVECTOR& pos);
    void set_position(const DirectX::XMFLOAT3& pos);
    void set_position(float x, float y, float z);
    void add_position(const DirectX::XMVECTOR& pos);
    void add_position(const DirectX::XMFLOAT3& pos);
    void add_position(float x, float y, float z);
    void set_rotation(const DirectX::XMVECTOR& rot);
    void set_rotation(const DirectX::XMFLOAT3& rot);
    void set_rotation(float x, float y, float z);
    void add_rotation(const DirectX::XMVECTOR& rot);
    void add_rotation(const DirectX::XMFLOAT3& rot);
    void add_rotation(float x, float y, float z);
    void set_look_at_pos(DirectX::XMFLOAT3 look_at_pos);
    const DirectX::XMVECTOR& get_forward_vector();
    const DirectX::XMVECTOR& get_right_vector();
    const DirectX::XMVECTOR& get_backward_vector();
    const DirectX::XMVECTOR& get_left_vector();

private:
    void update_view_matrix_();
    DirectX::XMVECTOR pos_vector_;
    DirectX::XMVECTOR rot_vector_;
    DirectX::XMFLOAT3 pos_;
    DirectX::XMFLOAT3 rot_;
    DirectX::XMMATRIX view_matrix_;
    DirectX::XMMATRIX projection_matrix_;

    static const DirectX::XMVECTOR DEFAULT_FORWARD_VECTOR_;
    static const DirectX::XMVECTOR DEFAULT_UP_VECTOR_;
    static const DirectX::XMVECTOR DEFAULT_BACKWARD_VECTOR_;
    static const DirectX::XMVECTOR DEFAULT_LEFT_VECTOR_;
    static const DirectX::XMVECTOR DEFAULT_RIGHT_VECTOR_;

    DirectX::XMVECTOR vec_forward_;
    DirectX::XMVECTOR vec_left_;
    DirectX::XMVECTOR vec_right_;
    DirectX::XMVECTOR vec_backward_;
};

} // namespace dxframework