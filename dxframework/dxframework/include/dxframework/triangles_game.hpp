#pragma once

#include <dxframework/game.hpp>
#include <dxframework/game_component.hpp>

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
};

} // namespace dxframework