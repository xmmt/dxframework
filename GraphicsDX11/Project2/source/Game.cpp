#include <Game.hpp>

using namespace GraphicsFramework;

Game::Game(Renderer const& renderer, InputDevice const& inputDevice)
    : initialized_{ false }
    , renderer_{ renderer }
    , inputDevice_{ inputDevice } {
}

GameResult<void> Game::run() {
    if (!initialized_) {
        return Err(GameError{
          .kind = GameError::Kind::BAD_STATE,
          .text = "not initialized" });
    }

    {
        auto res = run_();
        if (res.isErr()) {
            return res;
        }
    }
    {
        auto res = renderer_.runLoop([this](float delta) { runFrame_(delta); });
        if (res.isErr()) {
            return Err(GameError::fromRendererError(res.unwrapErr()));
        }
    }

    return Ok();
}

GameResult<void> Game::init() {
    if (initialized_) {
        return Err(GameError{
          .kind = GameError::Kind::BAD_STATE,
          .text = "already initialized" });
    }

    auto res = init_();
    if (res.isErr()) {
        return res;
    }

    initialized_ = true;
    return Ok();
}

GameResult<void> Game::destroy() {
    if (!initialized_) {
        return Err(GameError{
          .kind = GameError::Kind::BAD_STATE,
          .text = "not initialized" });
    }

    auto res = destroy_();
    if (res.isErr()) {
        return res;
    }

    initialized_ = false;
    return Ok();
}