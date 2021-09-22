#include <dxframework/triangle_component.hpp>

#include <mutex>

using namespace dxframework;

vertex_shader triangle_component::vs_;
pixel_shader triangle_component::ps_;
constant_buffer<CB_VS_vertexshader> triangle_component::cb_;

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

void triangle_component::draw(DirectX::XMMATRIX const& view_projection_matrix) {
    renderer_.set_shaders(vs_, ps_);
    cb_.get_data().wvp_matrix = view_projection_matrix;
    cb_.get_data().world_matrix = transform_.get_world_matrix();
    cb_.apply_changes();
    renderer_.draw(vertices_, colors_, indices_, view_projection_matrix);
}

void triangle_component::initialize() {
    static std::once_flag flag;
    std::call_once(flag, [this] {
        renderer_.init_shaders(vs_, ps_);
        ps_.init_with_source_code("../../../../dxframework/shaders/triangle_ps.hlsl", {}, "PS_main", "ps_5_0");
        vs_.init_with_source_code("../../../../dxframework/shaders/triangle_vs.hlsl", {}, "VS_main", "vs_5_0");
        //ps_.init_with_source_code("../../../../dxframework/shaders/ps.hlsl", {}, "PS_main", "ps_5_0");
        //vs_.init_with_source_code("../../../../dxframework/shaders/vs.hlsl", {}, "VS_main", "vs_5_0");

        cb_.init(renderer_.get_device(), renderer_.get_context());
        cb_.apply_changes();
        renderer_.get_context()->VSSetConstantBuffers(0, 1, cb_.get_address_of());
    });
}

void triangle_component::reload() {
}

void triangle_component::update() {
}