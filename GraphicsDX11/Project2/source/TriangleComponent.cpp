#include <TriangleComponent.hpp>

using namespace GraphicsFramework;

TriangleComponent::TriangleComponent(Renderer& renderer, Buffer<Vertex<4>> vertices, Buffer<Color<4>> colors, VertexShader& vertexShader, PixelShader& pixelShader)
    : GameComponent{ renderer }
    , vertices_{ std::move(vertices) }
    , colors_{ std::move(colors) }
    , vertexShader_{ vertexShader }
    , pixelShader_{ pixelShader } {
}

Result<void, GameComponentError> TriangleComponent::destroyResources() {
    return Ok();
}

Result<void, GameComponentError> TriangleComponent::draw() {
    renderer_.draw(vertices_, colors_, vertexShader_, pixelShader_);
    return Ok();
}

Result<void, GameComponentError> TriangleComponent::initialize() {
    return Ok();
}

Result<void, GameComponentError> TriangleComponent::reload() {
    return Ok();
}

Result<void, GameComponentError> TriangleComponent::update() {
    return Ok();
}