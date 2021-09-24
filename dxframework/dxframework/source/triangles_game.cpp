#include <dxframework/triangles_game.hpp>
#include <dxframework/triangle_component.hpp>
#include <dxframework/fruit_component.hpp>
#include <dxframework/floor_component.hpp>

using namespace dxframework;

triangles_game::triangles_game(renderer& ren, input_device& input_dev)
    : game{ ren, input_dev } {
}

void triangles_game::run_frame_(float delta) {
    renderer_.prepare_frame();
    input_y_axis_ = w_pressed_ ? s_pressed_ ? 0.f : 1.f : s_pressed_ ? -1.f
                                                                     : 0.f;
    input_x_axis_ = d_pressed_ ? a_pressed_ ? 0.f : 1.f : a_pressed_ ? -1.f
                                                                     : 0.f;
    input_z_axis_ = e_pressed_ ? q_pressed_ ? 0.f : 1.f : q_pressed_ ? -1.f
                                                                     : 0.f;
    pitch_ = up_pressed_ ? down_pressed_ ? 0.f : 1.f : down_pressed_ ? -1.f
                                                                     : 0.f;
    yaw_ = right_pressed_ ? left_pressed_ ? 0.f : 1.f : left_pressed_ ? -1.f
                                                                      : 0.f;
    //camera_.add_position(input_x_axis_ * cam_speed_, input_y_axis_ * cam_speed_, input_z_axis_ * cam_speed_);
    auto shift = DirectX::SimpleMath::Vector3{ camera_.get_forward_vector() } * input_y_axis_ * cam_speed_
      + DirectX::SimpleMath::Vector3{ camera_.get_right_vector() } * input_x_axis_ * cam_speed_
      + DirectX::SimpleMath::Vector3{ camera_.get_up_vector() } * input_z_axis_ * cam_speed_;
    camera_.add_position(shift.x, shift.y, shift.z);

    auto cam_pos = DirectX::SimpleMath::Vector3{ camera_.get_position_vector() };
    auto tr_pos = tr_->get_transform().get_position();
    auto dist = 5; //DirectX::SimpleMath::Vector3::Distance(tr_pos, cam_pos);
    auto dist_vec = tr_pos - cam_pos;
    auto for_vec = DirectX::SimpleMath::Vector3{ camera_.get_forward_vector() } * dist;
    //utils::debug_write::info("{} {} {}\n", dist_vec.x, dist_vec.y, dist_vec.z);
    //auto forward = DirectX::SimpleMath::Vector3{ camera_.get_forward_vector() };
    //utils::debug_write::info("forward {} {} {}\n", forward.x, forward.y, forward.z);
    //auto right = DirectX::SimpleMath::Vector3{ camera_.get_right_vector() };
    //utils::debug_write::info("forward {} {} {}\n", right.x, right.y, right.z);
    //auto up = DirectX::SimpleMath::Vector3{ camera_.get_up_vector() };
    //utils::debug_write::info("forward {} {} {}\n", up.x, up.y, up.z);

    //camera_.add_position(dist_vec.x, dist_vec.y, dist_vec.z);
    //camera_.add_position(-dist_vec.x, -dist_vec.y, -dist_vec.z);


    //camera_.add_position(for_vec.x, for_vec.y, for_vec.z);
    camera_.add_rotation(pitch_ * cam_speed_, yaw_ * cam_speed_, 0.f);
    //auto for_vec2 = DirectX::SimpleMath::Vector3{ camera_.get_forward_vector() } * dist;
    //camera_.add_position(-for_vec2.x, -for_vec2.y, -for_vec2.z);


    //camera_.add_position(-dist_vec.x, -dist_vec.y, -dist_vec.z);
    //camera_.add_position(dist_vec.x, dist_vec.y, dist_vec.z);


    //if (left_mouse_button_pressed_) {
    //    camera_.add_rotation(
    //      static_cast<float>(
    //        std::get<0>(mouse_cur_pos_) - std::get<0>(mouse_start_pos_))
    //        * 0.001f,
    //      static_cast<float>(
    //        std::get<1>(mouse_cur_pos_) - std::get<1>(mouse_start_pos_))
    //        * 0.001f,
    //      0.f);
    //    mouse_start_pos_ = mouse_cur_pos_;
    //}

    for (auto& component : components_) {
        component->update();
    }

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
      buffer<color_t>{ DirectX::XMFLOAT4{ 1.0f, 0.0f, 0.0f, 1.0f },
        DirectX::XMFLOAT4{ 0.0f, 0.0f, 1.0f, 1.0f },
        DirectX::XMFLOAT4{ 0.0f, 1.0f, 0.0f, 1.0f } }));
    components_.back()->initialize();
    tr_ = static_cast<triangle_component*>(components_.back().get());

    components_.push_back(std::make_shared<fruit_component>(
      renderer_,
      "../../../../dxframework/models/catt/Untitled.obj"));
    components_.back()->initialize();
    components_.push_back(std::make_shared<fruit_component>(
      renderer_,
      "../../../../dxframework/models/sunn/Untitled.obj"));
    components_.back()->initialize();
    //components_.push_back(std::make_shared<fruit_component>(
    //  renderer_,
    //  "../../../../dxframework/models/guitar/guitar.obj"));
    //components_.back()->initialize();
    components_.push_back(std::make_shared<fruit_component>(
      renderer_,
      "../../../../dxframework/models/earth/Untitled.obj"));
    components_.back()->initialize();
    components_.push_back(std::make_shared<floor_component>(
      renderer_,
      "../../../../dxframework/models/floor/Untitled.obj"));
    components_.back()->initialize();

    auto window_width = 800;
    auto window_height = 800;
    camera_.set_position(2.0f, 2.0f, -3.0f);
    camera_.set_projection(90.0f, static_cast<float>(window_width) / static_cast<float>(window_height), 0.1f, 1000.0f);

    //input_device_.add_on_key_pressed_callback(input_device::key::W, [this] { input_y_axis_ = min(input_y_axis_ + 1.f, 1.f); });
    //input_device_.add_on_key_pressed_callback(input_device::key::S, [this] { input_y_axis_ = max(input_y_axis_ - 1.f, -1.f); });
    //input_device_.add_on_key_pressed_callback(input_device::key::A, [this] { input_x_axis_ = max(input_x_axis_ - 1.f, -1.f); });
    //input_device_.add_on_key_pressed_callback(input_device::key::D, [this] { input_x_axis_ = min(input_x_axis_ + 1.f, 1.f); });
    //
    //input_device_.add_on_key_released_callback(input_device::key::W, [this] { input_y_axis_ -= 1.f; });
    //input_device_.add_on_key_released_callback(input_device::key::S, [this] { input_y_axis_ += 1.f; });
    //input_device_.add_on_key_released_callback(input_device::key::A, [this] { input_x_axis_ += 1.f; });
    //input_device_.add_on_key_released_callback(input_device::key::D, [this] { input_x_axis_ -= 1.f; });

    input_device_.add_on_key_pressed_callback(input_device::key::W, [this] { w_pressed_ = true; });
    input_device_.add_on_key_pressed_callback(input_device::key::S, [this] { s_pressed_ = true; });
    input_device_.add_on_key_pressed_callback(input_device::key::A, [this] { a_pressed_ = true; });
    input_device_.add_on_key_pressed_callback(input_device::key::D, [this] { d_pressed_ = true; });
    input_device_.add_on_key_pressed_callback(input_device::key::E, [this] { e_pressed_ = true; });
    input_device_.add_on_key_pressed_callback(input_device::key::Q, [this] { q_pressed_ = true; });

    input_device_.add_on_key_released_callback(input_device::key::W, [this] { w_pressed_ = false; });
    input_device_.add_on_key_released_callback(input_device::key::S, [this] { s_pressed_ = false; });
    input_device_.add_on_key_released_callback(input_device::key::A, [this] { a_pressed_ = false; });
    input_device_.add_on_key_released_callback(input_device::key::D, [this] { d_pressed_ = false; });
    input_device_.add_on_key_released_callback(input_device::key::E, [this] { e_pressed_ = false; });
    input_device_.add_on_key_released_callback(input_device::key::Q, [this] { q_pressed_ = false; });

    //input_device_.add_on_key_pressed_callback(input_device::key::LEFT_MOUSE_BUTTON, [this] { left_mouse_button_pressed_ = true; });
    //input_device_.add_on_key_released_callback(input_device::key::LEFT_MOUSE_BUTTON, [this] { left_mouse_button_pressed_ = false; });
    //
    //mouse_start_pos_ = input_device_.get_mouse_position();
    //
    //input_device_.add_on_mouse_move_callback([this](input_device::mouse_position pos) {
    //    mouse_cur_pos_ = std::make_tuple(pos.x, pos.y);
    //    if (not left_mouse_button_pressed_) {
    //        mouse_start_pos_ = mouse_cur_pos_;
    //    }
    //});

    input_device_.add_on_key_pressed_callback(input_device::key::UP, [this] { up_pressed_ = true; });
    input_device_.add_on_key_pressed_callback(input_device::key::DOWN, [this] { down_pressed_ = true; });
    input_device_.add_on_key_pressed_callback(input_device::key::LEFT, [this] { left_pressed_ = true; });
    input_device_.add_on_key_pressed_callback(input_device::key::RIGHT, [this] { right_pressed_ = true; });

    input_device_.add_on_key_released_callback(input_device::key::UP, [this] { up_pressed_ = false; });
    input_device_.add_on_key_released_callback(input_device::key::DOWN, [this] { down_pressed_ = false; });
    input_device_.add_on_key_released_callback(input_device::key::LEFT, [this] { left_pressed_ = false; });
    input_device_.add_on_key_released_callback(input_device::key::RIGHT, [this] { right_pressed_ = false; });
}

void triangles_game::destroy_() {
}