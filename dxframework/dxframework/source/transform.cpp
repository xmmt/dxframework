#include <dxframework/transform.hpp>

using namespace dxframework;
using namespace DirectX;
using namespace SimpleMath;

transform::transform(Vector3 pos) {
    m_world_ = Matrix::CreateFromQuaternion(Quaternion::Identity) * Matrix::CreateTranslation(pos);
}

transform& transform::get_parent() const {
    return *parent_;
}

std::vector<std::shared_ptr<transform>>& transform::get_child() {
    return children_;
}

void transform::add_child(transform* obj) {
    children_.push_back(std::unique_ptr<transform>(obj));
    obj->parent_ = std::unique_ptr<transform>(this);
    obj->m_world_ *= get_world_matrix().Invert();
}

void transform::set_parent(transform* p) {
    parent_ = std::unique_ptr<transform>(p);
    p->children_.push_back(std::unique_ptr<transform>(this));
    m_world_ *= p->get_world_matrix().Invert();
}

Vector3 transform::get_position() const {
    return m_translation_.Translation();
}

Vector3 transform::get_world_position() const {
    return get_world_matrix().Translation();
}

void transform::set_position(Vector3 pos) {
    m_world_ = get_world_matrix() * m_translation_.Invert();
    m_translation_ = Matrix::CreateTranslation(pos);
    m_world_ *= m_translation_;
}

void transform::add_position(Vector3 pos) {
    m_world_ = get_world_matrix() * Matrix::CreateTranslation(pos);
    m_translation_ *= Matrix::CreateTranslation(pos);
}

void transform::set_rotation(float x, float y, float z) {
    m_world_ = get_world_matrix() * m_translation_.Invert();
    m_translation_ = Matrix::CreateFromYawPitchRoll(x, y, z);
    m_world_ *= m_translation_;
}

void transform::add_local_rotation(Vector3 axis, float const angle) {
    rot_ = rot_ + DirectX::XMFLOAT3(axis * angle);
    m_rotation_ *= Matrix::CreateFromAxisAngle(axis, angle);
    m_world_ = get_world_matrix() * m_translation_.Invert() * Matrix::CreateFromAxisAngle(axis, angle) * m_translation_;
}

Matrix transform::get_world_matrix() const {
    auto result = m_world_;

    if (parent_) {
        result *= parent_->get_world_matrix();
    }

    return result;
}
