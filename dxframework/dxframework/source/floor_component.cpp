#include <dxframework/floor_component.hpp>

#include <mutex>

using namespace dxframework;

vertex_shader floor_component::vs_;
pixel_shader floor_component::ps_;
constant_buffer<CB_VS_vertexshader> floor_component::cb_;

floor_component::floor_component(renderer& renderer, std::string file_path)
    : game_component{ renderer }
    , file_path_{ std::move(file_path) } {
    transform_.set_position({ -6.f, -3.f, 0.f });
    //transform_.set_rotation(0.f, DirectX::XM_PIDIV2, 0.f);
    transform_.set_scale(.1f, .01f, .1f);
}

void floor_component::destroy_resources() {
}

void floor_component::draw(DirectX::XMMATRIX const& view_projection_matrix) {
    renderer_.set_shaders(vs_, ps_);
    model_.draw(transform_.get_world_matrix(), view_projection_matrix);
}

void floor_component::initialize() {
    static std::once_flag flag;
    std::call_once(flag, [this] {
        std::vector<D3D11_INPUT_ELEMENT_DESC> input_elements = {
            D3D11_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            D3D11_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            D3D11_INPUT_ELEMENT_DESC{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };

        vs_ = vertex_shader{ renderer_.get_device(), std::move(input_elements) };
        ps_ = pixel_shader{ renderer_.get_device() };

        ps_.init_with_source_code("../../../../dxframework/shaders/ps.hlsl", {}, "PS_main", "ps_5_0");
        vs_.init_with_source_code("../../../../dxframework/shaders/vs.hlsl", {}, "VS_main", "vs_5_0");
        cb_.init(renderer_.get_device(), renderer_.get_context());
        cb_.apply_changes();
    });
    model_.init(file_path_, renderer_.get_device(), renderer_.get_context(), cb_);
}

void floor_component::reload() {
}

void floor_component::update() {
    //transform_.add_local_rotation({ 0.f, 1.f, 0.f }, 0.05f);
}
