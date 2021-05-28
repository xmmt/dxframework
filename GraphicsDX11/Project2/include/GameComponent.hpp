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

class GameComponent {
public:
    GameComponent(Renderer& renderer);
    virtual ~GameComponent() = 0;

public:
    virtual Result<void, GameComponentError> destroyResources() = 0;
    virtual Result<void, GameComponentError> draw() = 0;
    virtual Result<void, GameComponentError> initialize() = 0;
    virtual Result<void, GameComponentError> reload() = 0;
    virtual Result<void, GameComponentError> update() = 0;

protected:
    Renderer& renderer_;
};

} // namespace GraphicsFramework