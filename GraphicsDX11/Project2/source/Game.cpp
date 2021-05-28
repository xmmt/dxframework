#include <Game.hpp>

using namespace GraphicsFramework;

Game::Game(Renderer const& renderer, InputDevice const& inputDevice)
    : initialized_{ false }
    , renderer_{ renderer }
    , inputDevice_{ inputDevice } {
}
