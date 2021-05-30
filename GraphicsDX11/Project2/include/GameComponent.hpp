#pragma once

#include <Utils.hpp>
#include <Renderer.hpp>

namespace GraphicsFramework {

struct GameComponentError {
    enum class Kind {
        UNDEFINED,
    };

    Kind kind;
    std::string text;
};

template <typename T>
using GameCompResult = Result<T, GameComponentError>;

class GameComponent {
public:
    GameComponent(Renderer& renderer);
    virtual ~GameComponent() = default;

public:
    virtual GameCompResult<void> destroyResources() = 0;
    virtual GameCompResult<void> draw() = 0;
    virtual GameCompResult<void> initialize() = 0;
    virtual GameCompResult<void> reload() = 0;
    virtual GameCompResult<void> update() = 0;

protected:
    Renderer& renderer_;
};

} // namespace GraphicsFramework