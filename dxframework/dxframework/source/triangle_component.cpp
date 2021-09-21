#include <dxframework/triangle_component.hpp>

#include <mutex>

using namespace dxframework;

vertex_shader triangle_component::vs_;
pixel_shader triangle_component::ps_;

triangle_component::triangle_component(
  renderer& ren,
  buffer<vertex> vertices,
  buffer<color> colors)
    : game_component{ ren }
    , vertices_{ std::move(vertices) }
    , colors_{ std::move(colors) }
    , indices_{ 0, 1, 2 } {
}

triangle_component::~triangle_component() {
}

void triangle_component::destroy_resources() {
}

void triangle_component::draw() {
    renderer_.set_shaders(vs_, ps_);
    renderer_.draw(vertices_, colors_, indices_);
}

void triangle_component::initialize() {
    static std::once_flag flag;
    std::call_once(flag, [this] {
        renderer_.init_shaders(vs_, ps_);
        ps_.init_with_source_code("../../../../dxframework/shaders/triangle_ps.hlsl", {}, "PS_main", "ps_5_0");
        vs_.init_with_source_code("../../../../dxframework/shaders/triangle_vs.hlsl", {}, "VS_main", "vs_5_0");
        //ps_.init_with_source_code("../../../../dxframework/shaders/ps.hlsl", {}, "PS_main", "ps_5_0");
        //vs_.init_with_source_code("../../../../dxframework/shaders/vs.hlsl", {}, "VS_main", "vs_5_0");
    });
}

void triangle_component::reload() {
}

void triangle_component::update() {
}