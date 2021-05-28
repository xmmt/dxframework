#include <Renderer.hpp>

using namespace GraphicsFramework;

Renderer::Renderer(Display const& display)
    : initialized_{ false }
    , frameStarted_{ false }
    , display_{ display }
    , currentVertexShader{ nullptr }
    , currentPixelShader{ nullptr } {
}

Result<void, RendererError> Renderer::init() {
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

Result<void, RendererError> Renderer::destroy() {
    if (initialized_) {
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

Result<void, RendererError> Renderer::prepareFrame() {
    if (initialized_) {
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

Result<void, RendererError> Renderer::endFrame() {
    if (initialized_) {
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
        auto res = draw_(vertices_, colors_, indices_, vertexShader, pixelShader);
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

Result<void, RendererError> Renderer::draw(Buffer<Vertex<4>> const& vertices, Buffer<Color<4>> const& colors, Buffer<int> const& indices, VertexShader& vertexShader, PixelShader& pixelShader) {
    if (initialized_) {
        return Err(RendererError{
          .kind = RendererError::Kind::BAD_STATE,
          .text = "not initialized" });
    }
    if (!frameStarted_) {
        return Err(RendererError{
          .kind = RendererError::Kind::BAD_STATE,
          .text = "frame not started" });
    }

    if (currentVertexShader != &vertexShader || currentPixelShader != &pixelShader) {
        auto res = draw_(vertices_, colors_, indices_, vertexShader, pixelShader);
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