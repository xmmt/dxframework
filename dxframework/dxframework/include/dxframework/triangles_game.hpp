#pragma once

#include <dxframework/game.hpp>
#include <dxframework/game_component.hpp>
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
    camera camera_;
    float input_x_axis_{ 0.f };
    float input_y_axis_{ 0.f };
    float cam_speed_{ 0.04f };
};

} // namespace dxframework