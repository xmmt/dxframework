#include <Triangles.hpp>

using namespace GraphicsFramework;

Triangles::Triangles(Renderer const& renderer, InputDevice const& inputDevice)
    : Game{ renderer, inputDevice } {
}

Result<void, GameError> Triangles::run() {
    if (!initialized_) {
        return Err(GameError{
          .kind = GameError::Kind::BAD_STATE,
          .text = "not initialized" });
    }
    return Ok();
}

Result<void, GameError> Triangles::init() {
    if (initialized_) {
        return Err(GameError{
          .kind = GameError::Kind::BAD_STATE,
          .text = "already initialized" });
    }
    initialized_ = true;
    return Ok();
}

Result<void, GameError> Triangles::destroy() {
    if (!initialized_) {
        return Err(GameError{
          .kind = GameError::Kind::BAD_STATE,
          .text = "not initialized" });
    }
    initialized_ = false;
    return Ok();
}