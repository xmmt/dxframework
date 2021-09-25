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

std::vector<transform*>& transform::get_child() {
    return children_;
}

void transform::add_child(transform* obj) {
    children_.push_back(obj);
    obj->parent_ = this;
    obj->m_world_ *= get_world_matrix().Invert();
}

void transform::set_parent(transform* p) {
    parent_ = p;
    p->children_.push_back(this);
    m_world_ *= p->get_world_matrix().Invert();
}

Vector3 transform::get_position() const {
    return m_translation_.Translation();
}

Vector3 transform::get_world_position() const {
    return get_world_matrix().Translation();
}

//void transform::set_position(Vector3 pos) {
//    m_world_ = get_world_matrix() * m_translation_.Invert();
//    m_translation_ = Matrix::CreateTranslation(pos);
//    m_world_ *= m_translation_;
//}

void transform::set_position(Vector3 pos) {
    m_translation_ = Matrix::CreateTranslation(pos);
}

//void transform::set_scale(float x, float y, float z) {
//    m_scale_ = Matrix::CreateScale(x, y, z);
//    m_world_ *= m_scale_;
//}

void transform::set_scale(float x, float y, float z) {
    m_scale_ = Matrix::CreateScale(x, y, z);
}

//void transform::add_position(Vector3 pos) {
//    m_world_ = get_world_matrix() * Matrix::CreateTranslation(pos);
//    m_translation_ *= Matrix::CreateTranslation(pos);
//}

void transform::add_position(Vector3 pos) {
    m_translation_ *= Matrix::CreateTranslation(pos);
}

//void transform::set_rotation(float x, float y, float z) {
//    m_world_ = get_world_matrix() * m_translation_.Invert();
//    m_translation_ = Matrix::CreateFromYawPitchRoll(x, y, z);
//    m_world_ *= m_translation_;
//}

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

//void transform::add_local_rotation(Vector3 axis, float const angle) {
//    rot_ = rot_ + DirectX::XMFLOAT3(axis * angle);
//    m_rotation_ *= Matrix::CreateFromAxisAngle(axis, angle);
//    m_world_ = get_world_matrix() * m_translation_.Invert() * Matrix::CreateFromAxisAngle(axis, angle) * m_translation_;
//}

void transform::add_local_rotation(Vector3 axis, float const angle) {
    m_rotation_ *= Matrix::CreateFromAxisAngle(axis, angle);
}

void transform::add_rotation(Vector3 axis, float const angle) {
    m_global_rotation_ *= Matrix::CreateFromAxisAngle(axis, angle);
}

Matrix transform::get_world_matrix() const {
    auto result = m_scale_ * m_rotation_ * m_translation_ * m_global_rotation_;

    if (parent_) {
        result *= parent_->get_world_matrix();
    }

    return result;
}
