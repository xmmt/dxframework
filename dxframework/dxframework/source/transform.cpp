#include <dxframework/transform.hpp>

using namespace dxframework;
using namespace DirectX;
using namespace SimpleMath;

transform::transform(Vector3 pos) {
    m_world_ = Matrix::CreateFromQuaternion(Quaternion::Identity) * Matrix::CreateTranslation(pos);
}

transform* transform::get_parent() const {
    return parent_;
}

Vector3 transform::get_position() const {
    return m_translation_.Translation();
}

Vector3 transform::get_world_position() const {
    return get_world_matrix().Translation();
}

void transform::set_position(Vector3 pos) {
    m_translation_ = Matrix::CreateTranslation(pos);
}

void transform::set_scale(float x, float y, float z) {
    m_scale_ = Matrix::CreateScale(x, y, z);
}

void transform::add_position(Vector3 pos) {
    m_translation_ *= Matrix::CreateTranslation(pos);
}

void transform::set_local_rotation(float x, float y, float z) {
    m_rotation_ = Matrix::CreateFromYawPitchRoll(x, y, z);
}

void transform::set_local_rotation(Vector3 axis, float const angle) {
    m_rotation_ = Matrix::CreateFromAxisAngle(axis, angle);
}

void transform::set_rotation(float x, float y, float z) {
    m_global_rotation_ = Matrix::CreateFromYawPitchRoll(x, y, z);
}

void transform::set_rotation(Vector3 axis, float const angle) {
    m_global_rotation_ = Matrix::CreateFromAxisAngle(axis, angle);
}

void transform::add_local_rotation(Vector3 axis, float const angle) {
    m_rotation_ *= Matrix::CreateFromAxisAngle(axis, angle);
}

void transform::add_rotation(Vector3 axis, float const angle) {
    m_global_rotation_ *= Matrix::CreateFromAxisAngle(axis, angle);
}

Matrix transform::get_world_matrix() const {
    return m_scale_ * m_rotation_ * m_translation_ * m_global_rotation_;
}
