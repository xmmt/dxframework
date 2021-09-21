#pragma once

#include <dxframework/game_component.hpp>

namespace dxframework {

class triangle_component : public game_component {
public:
    triangle_component(
      renderer& renderer,
      buffer<vertex> vertices,
      buffer<color> colors);
    ~triangle_component() override;

public:
    void destroy_resources() override;
    void draw() override;
    void initialize() override;
    void reload() override;
    void update() override;

private:
    buffer<vertex> vertices_;
    buffer<color> colors_;
    buffer<int> indices_{ 0, 1, 2 };
    static pixel_shader ps_;
    static vertex_shader vs_;
};

} // namespace dxframework