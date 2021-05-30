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
        RENDERER_BAD_STATE,
        RENDERER_DRAW_ERROR,
        RENDERER_INIT_FAILED,
    };

    Kind kind;
    std::string text;

    static GameError fromRendererError(RendererError&& err) {
        return GameError{
            .kind = err.kind == RendererError::Kind::BAD_STATE ? Kind::RENDERER_BAD_STATE
              : err.kind == RendererError::Kind::DRAW_ERROR    ? Kind::RENDERER_DRAW_ERROR
              : err.kind == RendererError::Kind::INIT_FAILED   ? Kind::RENDERER_INIT_FAILED
                                                               : Kind::UNDEFINED,
            .text = std::move(err.text)
        };
    }
};

template <typename T>
using GameResult = Result<T, GameError>;

class Game {
public:
    Game(Renderer const& renderer, InputDevice const& inputDevice);
    virtual ~Game() = default;

public:
    virtual GameResult<void> run();
    virtual GameResult<void> init();
    virtual GameResult<void> destroy();

protected:
    virtual GameResult<void> runFrame_(float delta) = 0;
    virtual GameResult<void> run_() = 0;
    virtual GameResult<void> init_() = 0;
    virtual GameResult<void> destroy_() = 0;

protected:
    bool initialized_{ false };
    Renderer const& renderer_;
    InputDevice const& inputDevice_;
};

} // namespace GraphicsFramework
