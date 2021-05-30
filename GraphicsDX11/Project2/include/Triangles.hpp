#pragma once

#include <Game.hpp>
#include <GameComponent.hpp>

namespace GraphicsFramework {

class Triangles : public Game {
public:
    Triangles(Renderer const& renderer, InputDevice const& inputDevice);
    ~Triangles() override = default;

private:
    GameResult<void> runFrame_(float delta) override;
    GameResult<void> run_() override;
    GameResult<void> init_() override;
    GameResult<void> destroy_() override;

private:
    std::vector<std::shared_ptr<GameComponent>> components_;
};

} // namespace GraphicsFramework