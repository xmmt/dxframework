#include <Triangles.hpp>

using namespace GraphicsFramework;

Triangles::Triangles(Renderer const& renderer, InputDevice const& inputDevice)
    : Game{ renderer, inputDevice } {
}

GameResult<void> Triangles::runFrame_(float delta) {
    renderer_.prepareFrame();
    renderer_.endFrame();
    return Ok();
}

GameResult<void> Triangles::run_() {
    // renderer_.prepareFrame();
    // renderer_.endFrame();
    return Ok();
}

GameResult<void> Triangles::init_() {

    return Ok();
}

GameResult<void> Triangles::destroy_() {

    return Ok();
}