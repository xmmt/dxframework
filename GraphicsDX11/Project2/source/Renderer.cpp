#include <Renderer.hpp>

using namespace GraphicsFramework;

Renderer::Renderer(Display const& display)
    : initialized_{ false }
    , frameStarted_{ false }
    , display_{ display }
    , currentMaterial_{ nullptr } {
}

RenderResult<void> Renderer::init() {
    if (initialized_) {
        return Err(RendererError{
          .kind = RendererError::Kind::BAD_STATE,
          .text = "already initialized" });
    }

    auto res = init_();
    if (res.isErr()) {
        return res;
    }

    initialized_ = true;
    return Ok();
}

RenderResult<void> Renderer::destroy() {
    if (!initialized_) {
        return Err(RendererError{
          .kind = RendererError::Kind::BAD_STATE,
          .text = "not initialized" });
    }

    auto res = destroy_();
    if (res.isErr()) {
        return res;
    }

    initialized_ = false;
    return Ok();
}

RenderResult<void> Renderer::runLoop(std::function<void(float)> runFrame) const {
    if (!initialized_) {
        return Err(RendererError{
          .kind = RendererError::Kind::BAD_STATE,
          .text = "not initialized" });
    }

    auto res = display_.runLoop(runFrame);
    if (res.isErr()) {
        return Err(RendererError::fromDisplayError(res.unwrapErr()));
    }

    return Ok();
}

RenderResult<void> Renderer::prepareFrame() const {
    if (!initialized_) {
        return Err(RendererError{
          .kind = RendererError::Kind::BAD_STATE,
          .text = "not initialized" });
    }
    if (frameStarted_) {
        return Err(RendererError{
          .kind = RendererError::Kind::BAD_STATE,
          .text = "frame already started" });
    }

    auto res = prepareFrame_();
    if (res.isErr()) {
        return res;
    }

    frameStarted_ = true;
    return Ok();
}

RenderResult<void> Renderer::endFrame() const {
    if (!initialized_) {
        return Err(RendererError{
          .kind = RendererError::Kind::BAD_STATE,
          .text = "not initialized" });
    }
    if (!frameStarted_) {
        return Err(RendererError{
          .kind = RendererError::Kind::BAD_STATE,
          .text = "frame not started" });
    }

    if (!vertices_.empty()) {
        auto res = draw_(vertices_, colors_, indices_, *currentMaterial_);
        vertices_.clear();
        colors_.clear();
        indices_.clear();
        if (res.isErr()) {
            return res;
        }
    }

    auto res = endFrame_();
    if (res.isErr()) {
        return res;
    }

    frameStarted_ = false;
    return Ok();
}

RenderResult<void> Renderer::draw(
  Buffer<Vertex> const& vertices,
  Buffer<Color> const& colors,
  Buffer<int> const& indices,
  Material& material) const {
    if (!initialized_) {
        return Err(RendererError{
          .kind = RendererError::Kind::BAD_STATE,
          .text = "not initialized" });
    }
    if (!frameStarted_) {
        return Err(RendererError{
          .kind = RendererError::Kind::BAD_STATE,
          .text = "frame not started" });
    }

    if (currentMaterial_ != &material) {
        auto res = draw_(vertices_, colors_, indices_, material);
        vertices_.clear();
        colors_.clear();
        indices_.clear();
        if (res.isErr()) {
            return res;
        }
    }

    std::copy(vertices.begin(), vertices.end(), std::back_inserter(vertices_));
    std::copy(colors.begin(), colors.end(), std::back_inserter(colors_));
    std::copy(indices.begin(), indices.end(), std::back_inserter(indices_));

    return Ok();
}