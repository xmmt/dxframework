#pragma once

#include <Utils.hpp>
#include <Renderer.hpp>
#include <InputDevice.hpp>

namespace GraphicsFramework {

struct GameError {
public:
    enum class Kind {
        UNDEFINED,
        BAD_STATE,
    };

    Kind kind;
    std::string text;
};

class Game {
public:
    Game(Renderer const& renderer, InputDevice const& inputDevice);
    virtual ~Game() = default;

public:
    virtual Result<void, GameError> run() = 0;
    virtual Result<void, GameError> init() = 0;
    virtual Result<void, GameError> destroy() = 0;

protected:
    bool initialized_{ false };
    Renderer const& renderer_;
    InputDevice const& inputDevice_;
};

} // namespace GraphicsFramework
