#include <dxframework/triangles_game.hpp>
#include <dxframework/triangle_component.hpp>

using namespace dxframework;

triangles_game::triangles_game(renderer& ren, input_device& input_dev)
    : game{ ren, input_dev } {
}

void triangles_game::run_frame_(float delta) {
    renderer_.prepare_frame();
    camera_.add_position(input_x_axis_ * cam_speed_, input_y_axis_ * cam_speed_, 0.f);
    auto const vp_matrix = camera_.get_view_matrix() * camera_.get_projection_matrix();
    for (auto& component : components_) {
        component->draw(vp_matrix);
    }
    renderer_.end_frame();
}

void triangles_game::run_() {
}

void triangles_game::init_() {
    components_.push_back(std::make_shared<triangle_component>(renderer_,
      buffer<vertex>{ DirectX::XMFLOAT4{ 0.5f, 0.5f, 0.5f, 1.0f },
        DirectX::XMFLOAT4{ -0.5f, -0.5f, 0.5f, 1.0f },
        DirectX::XMFLOAT4{ 0.5f, -0.5f, 0.5f, 1.0f } },
      buffer<color>{ DirectX::XMFLOAT4{ 1.0f, 0.0f, 0.0f, 1.0f },
        DirectX::XMFLOAT4{ 0.0f, 0.0f, 1.0f, 1.0f },
        DirectX::XMFLOAT4{ 0.0f, 1.0f, 0.0f, 1.0f } }));
    components_.back()->initialize();

    auto window_width = 800;
    auto window_height = 800;
    camera_.set_position(2.0f, 2.0f, -3.0f);
    camera_.set_projection(90.0f, static_cast<float>(window_width) / static_cast<float>(window_height), 0.1f, 1000.0f);

    input_device_.add_on_key_pressed_callback(input_device::key::W, [this] { input_y_axis_ = 1.f; });
    input_device_.add_on_key_pressed_callback(input_device::key::S, [this] { input_y_axis_ = -1.f; });
    input_device_.add_on_key_pressed_callback(input_device::key::A, [this] { input_x_axis_ = 1.f; });
    input_device_.add_on_key_pressed_callback(input_device::key::D, [this] { input_x_axis_ = -1.f; });

    input_device_.add_on_key_released_callback(input_device::key::W, [this] { input_y_axis_ = 0.f; });
    input_device_.add_on_key_released_callback(input_device::key::S, [this] { input_y_axis_ = 0.f; });
    input_device_.add_on_key_released_callback(input_device::key::A, [this] { input_x_axis_ = 0.f; });
    input_device_.add_on_key_released_callback(input_device::key::D, [this] { input_x_axis_ = 0.f; });
}

void triangles_game::destroy_() {
}