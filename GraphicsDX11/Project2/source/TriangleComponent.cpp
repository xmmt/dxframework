#include <TriangleComponent.hpp>

using namespace GraphicsFramework;

TriangleComponent::TriangleComponent(
  Renderer& renderer,
  Buffer<Vertex> vertices,
  Buffer<Color> colors,
  Material& material)
    : GameComponent{ renderer }
    , vertices_{ std::move(vertices) }
    , colors_{ std::move(colors) }
    , indices_{ 0, 1, 2 }
    , material_{ material } {
}

TriangleComponent::~TriangleComponent() {
}

GameCompResult<void> TriangleComponent::destroyResources() {
    return Ok();
}

GameCompResult<void> TriangleComponent::draw() {
    renderer_.draw(vertices_, colors_, indices_, material_);
    return Ok();
}

GameCompResult<void> TriangleComponent::initialize() {
    return Ok();
}

GameCompResult<void> TriangleComponent::reload() {
    return Ok();
}

GameCompResult<void> TriangleComponent::update() {
    return Ok();
}