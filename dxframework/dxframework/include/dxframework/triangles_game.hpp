#pragma once

#include <dxframework/game.hpp>
#include <dxframework/game_component.hpp>
#include <dxframework/triangle_component.hpp>
#include <dxframework/cube_component.hpp>
#include <dxframework/earth_component.hpp>
#include <dxframework/guitar_component.hpp>
#include <dxframework/sun_component.hpp>
#include <dxframework/camera.hpp>

namespace dxframework {

class triangles_game : public game {
public:
    triangles_game(renderer& renderer, input_device& input_dev);
    ~triangles_game() override = default;

private:
    void run_frame_(float delta) override;
    void run_() override;
    void init_() override;
    void destroy_() override;

private:
    std::vector<std::shared_ptr<game_component>> components_;
    std::vector<cube_component*> cubes_;
    earth_component* earth_;
    guitar_component* guitar_;
    sun_component* sun_;
    camera camera_;
    float input_x_axis_{ 0.f };
    float input_y_axis_{ 0.f };
    float input_z_axis_{ 0.f };
    float pitch_{ 0.f };
    float yaw_{ 0.f };
    bool w_pressed_{ false };
    bool s_pressed_{ false };
    bool a_pressed_{ false };
    bool d_pressed_{ false };
    bool e_pressed_{ false };
    bool q_pressed_{ false };
    bool up_pressed_{ false };
    bool down_pressed_{ false };
    bool left_pressed_{ false };
    bool right_pressed_{ false };
    bool left_mouse_button_pressed_{ false };
    std::tuple<int, int> mouse_start_pos_;
    std::tuple<int, int> mouse_cur_pos_;
    float cam_speed_{ 0.04f };

    triangle_component* tr_;

    constant_buffer<CB_PS_light> cb_ps_lightshader_;
    constant_buffer<fog_buffer> fogshader_;
    constant_buffer<light_buffer> lightshader_;
};

} // namespace dxframework
