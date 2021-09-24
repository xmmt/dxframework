#include <dxframework/camera.hpp>

#include <cmath>
#include <stdexcept>

using namespace dxframework;
using namespace DirectX;

XMVECTOR const camera::DEFAULT_FORWARD_VECTOR_{ DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f) };
XMVECTOR const camera::DEFAULT_UP_VECTOR_{ DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f) };
XMVECTOR const camera::DEFAULT_BACKWARD_VECTOR_{ DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f) };
XMVECTOR const camera::DEFAULT_LEFT_VECTOR_{ DirectX::XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f) };
XMVECTOR const camera::DEFAULT_RIGHT_VECTOR_{ DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f) };

camera::camera()
    : pos_{ 0.f, 0.f, 0.f }
    , pos_vector_{ XMLoadFloat3(&pos_) }
    , rot_{ 0.f, 0.f, 0.f }
    , rot_vector_{ XMLoadFloat3(&rot_) } {
    pos_vector_ = XMLoadFloat3(&pos_);
    rot_vector_ = XMLoadFloat3(&rot_);
    update_view_matrix_();
}

void camera::set_projection(float fov_degrees, float aspect_ratio, float near_z, float far_z) {
    float fov_radians = (fov_degrees / 360.0f) * XM_2PI;
    projection_matrix_ = XMMatrixPerspectiveFovLH(fov_radians, aspect_ratio, near_z, far_z);
}

XMMATRIX const& camera::get_view_matrix() const {
    return view_matrix_;
}

XMMATRIX const& camera::get_projection_matrix() const {
    return projection_matrix_;
}

XMVECTOR const& camera::get_position_vector() const {
    return pos_vector_;
}

XMFLOAT3 const& camera::get_position_float_3() const {
    return pos_;
}

XMVECTOR const& camera::get_rotation_vector() const {
    return rot_vector_;
}

XMFLOAT3 const& camera::get_rotation_float_3() const {
    return rot_;
}

void camera::set_position(XMVECTOR const& pos) {
    XMStoreFloat3(&pos_, pos);
    pos_vector_ = pos;
    update_view_matrix_();
}

void camera::set_position(XMFLOAT3 const& pos) {
    pos_ = pos;
    pos_vector_ = XMLoadFloat3(&pos_);
    update_view_matrix_();
}

void camera::set_position(float x, float y, float z) {
    pos_ = XMFLOAT3(x, y, z);
    pos_vector_ = XMLoadFloat3(&pos_);
    update_view_matrix_();
}

void camera::add_position(XMVECTOR const& pos) {
    pos_vector_ += pos;
    XMStoreFloat3(&pos_, pos_vector_);
    update_view_matrix_();
}

void camera::add_position(XMFLOAT3 const& pos) {
    pos_.x += pos.y;
    pos_.y += pos.y;
    pos_.z += pos.z;
    pos_vector_ = XMLoadFloat3(&pos_);
    update_view_matrix_();
}

void camera::add_position(float x, float y, float z) {
    pos_.x += x;
    pos_.y += y;
    pos_.z += z;
    pos_vector_ = XMLoadFloat3(&pos_);
    update_view_matrix_();
}

void camera::set_rotation(XMVECTOR const& rot) {
    rot_vector_ = rot;
    XMStoreFloat3(&rot_, rot);
    update_view_matrix_();
}

void camera::set_rotation(XMFLOAT3 const& rot) {
    rot_ = rot;
    rot_vector_ = XMLoadFloat3(&rot_);
    update_view_matrix_();
}

void camera::set_rotation(float x, float y, float z) {
    rot_ = XMFLOAT3(x, y, z);
    rot_vector_ = XMLoadFloat3(&rot_);
    update_view_matrix_();
}

void camera::add_rotation(XMVECTOR const& rot) {
    rot_vector_ += rot;
    XMStoreFloat3(&rot_, rot_vector_);
    update_view_matrix_();
}

void camera::add_rotation(XMFLOAT3 const& rot) {
    rot_.x += rot.x;
    rot_.y += rot.y;
    rot_.z += rot.z;
    rot_vector_ = XMLoadFloat3(&rot_);
    update_view_matrix_();
}

void camera::add_rotation(float x, float y, float z) {
    rot_.x += x;
    rot_.y += y;
    rot_.z += z;
    rot_vector_ = XMLoadFloat3(&rot_);
    update_view_matrix_();
}

void camera::set_look_at_pos(XMFLOAT3 look_at_pos) {
    //verify that look at pos is not the same as cam pos. They cannot be the same as that wouldn't make sense and would result in undefined behavior.
    if ((look_at_pos.x - pos_.x) * (look_at_pos.x - pos_.x)
        + (look_at_pos.y - pos_.y) * (look_at_pos.y - pos_.y)
        + (look_at_pos.z - pos_.z) * (look_at_pos.z - pos_.z)
      < 0.00001f) {
        throw std::logic_error("camera position is the same as look_at");
    }

    look_at_pos.x = pos_.x - look_at_pos.x;
    look_at_pos.y = pos_.y - look_at_pos.y;
    look_at_pos.z = pos_.z - look_at_pos.z;

    float pitch = 0.f;
    if (look_at_pos.y != 0.f) {
        float const distance = std::sqrt(look_at_pos.x * look_at_pos.x + look_at_pos.z * look_at_pos.z);
        pitch = std::atan(look_at_pos.y / distance);
    }

    float yaw = 0.f;
    if (look_at_pos.x != 0.f) {
        yaw = std::atan(look_at_pos.x / look_at_pos.z);
    }
    if (look_at_pos.z > 0.f)
        yaw += XM_PI;

    this->set_rotation(pitch, yaw, 0.f);
}

XMVECTOR const& camera::get_forward_vector() {
    return vec_forward_;
}

XMVECTOR const& camera::get_right_vector() {
    return vec_right_;
}

XMVECTOR const& camera::get_backward_vector() {
    return vec_backward_;
}

XMVECTOR const& camera::get_left_vector() {
    return vec_left_;
}

XMVECTOR const& camera::get_up_vector() {
    return vec_up_;
}

void camera::update_view_matrix_() //updates view matrix and also updates the movement vectors
{
    //calculate camera rotation matrix
    XMMATRIX cam_rotation_matrix = XMMatrixRotationRollPitchYaw(rot_.x, rot_.y, rot_.z);
    //calculate unit vector of cam target based off camera forward value transformed by cam rotation matrix
    XMVECTOR cam_target = XMVector3TransformCoord(DEFAULT_FORWARD_VECTOR_, cam_rotation_matrix);
    //adjust cam target to be offset by the camera's current position
    cam_target += pos_vector_;
    //calculate up direction based on current rotation
    XMVECTOR up_dir = XMVector3TransformCoord(DEFAULT_UP_VECTOR_, cam_rotation_matrix);
    //rebuild view matrix
    view_matrix_ = XMMatrixLookAtLH(pos_vector_, cam_target, up_dir);

    //XMMATRIX vec_rotation_matrix = XMMatrixRotationRollPitchYaw(0.f, rot_.y, 0.f);
    XMMATRIX vec_rotation_matrix = XMMatrixRotationRollPitchYaw(rot_.x, rot_.y, rot_.z);
    vec_forward_ = XMVector3TransformCoord(DEFAULT_FORWARD_VECTOR_, vec_rotation_matrix);
    vec_backward_ = XMVector3TransformCoord(DEFAULT_BACKWARD_VECTOR_, vec_rotation_matrix);
    vec_left_ = XMVector3TransformCoord(DEFAULT_LEFT_VECTOR_, vec_rotation_matrix);
    vec_right_ = XMVector3TransformCoord(DEFAULT_RIGHT_VECTOR_, vec_rotation_matrix);
    vec_up_ = XMVector3TransformCoord(DEFAULT_UP_VECTOR_, vec_rotation_matrix);
    //vec_up_ = up_dir;
}