#include <dxframework/game.hpp>

using namespace dxframework;

game::game(renderer& ren, input_device& input_dev)
    : initialized_{ false }
    , renderer_{ ren }
    , input_device_{ input_dev } {
}

void game::run() {
    if (!initialized_) {
        throw std::runtime_error("game is not initialized");
    }
    run_();
    renderer_.run_loop([this](float delta) { run_frame_(delta); });
}

void game::init() {
    if (initialized_) {
        throw std::runtime_error("game is already initialized");
    }
    init_();
    initialized_ = true;
}

void game::destroy() {
    if (!initialized_) {
        throw std::runtime_error("game is not initialized");
    }
    destroy_();
    initialized_ = false;
}