#pragma once

#include <GameComponent.hpp>
#include <Material.hpp>

namespace GraphicsFramework {

class TriangleComponent : public GameComponent {
public:
    TriangleComponent(
      Renderer& renderer,
      Buffer<Vertex> vertices,
      Buffer<Color> colors,
      Material& material);
    ~TriangleComponent() override;

public:
    GameCompResult<void> destroyResources() override;
    GameCompResult<void> draw() override;
    GameCompResult<void> initialize() override;
    GameCompResult<void> reload() override;
    GameCompResult<void> update() override;

private:
    Buffer<Vertex> vertices_;
    Buffer<Color> colors_;
    Buffer<int> indices_{ 0, 1, 2 };
    Material& material_;
};

} // namespace GraphicsFramework