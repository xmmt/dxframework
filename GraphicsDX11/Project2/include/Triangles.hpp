#pragma once

#include <Game.hpp>

namespace GraphicsFramework {

class Triangles : public Game {
public:
    Triangles(Renderer const& renderer, InputDevice const& inputDevice);
    virtual ~Triangles() = default;

public:
    Result<void, GameError> run() override;
    Result<void, GameError> init() override;
    Result<void, GameError> destroy() override;

private:
};

} // namespace GraphicsFramework