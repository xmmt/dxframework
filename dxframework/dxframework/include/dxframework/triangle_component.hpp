#pragma once

#include <dxframework/game_component.hpp>
#include <dxframework/transform.hpp>

namespace dxframework {

class triangle_component : public game_component {
public:
    triangle_component(
      renderer& renderer,
      buffer<vertex> vertices,
      buffer<color_t> colors);
    ~triangle_component() override;

public:
    void destroy_resources() override;
    void draw(DirectX::XMMATRIX const& view_projection_matrix) override;
    void initialize() override;
    void reload() override;
    void update() override;

public:
    transform& get_transform();

private:
    buffer<vertex> vertices_;
    buffer<color_t> colors_;
    buffer<int> indices_{ 0, 1, 2 };
    static pixel_shader ps_;
    static vertex_shader vs_;
    static constant_buffer<CB_VS_vertexshader> cb_;
    transform transform_{ DirectX::SimpleMath::Vector3{} };
};

} // namespace dxframework