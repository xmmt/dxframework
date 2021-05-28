#pragma once

#include <GameComponent.hpp>
#include <Shader.hpp>

namespace GraphicsFramework {

class TriangleComponent : public GameComponent {
public:
    TriangleComponent(Renderer& renderer, Buffer<Vertex<4>> vertices, Buffer<Color<4>> colors, VertexShader& vertexShader, PixelShader& pixelShader);
    ~TriangleComponent() override = default;

public:
    Result<void, GameComponentError> destroyResources() override;
    Result<void, GameComponentError> draw() override;
    Result<void, GameComponentError> initialize() override;
    Result<void, GameComponentError> reload() override;
    Result<void, GameComponentError> update() override;

private:
    Buffer<Vertex<4>> vertices_;
    Buffer<Color<4>> colors_;
    VertexShader& vertexShader_;
    PixelShader& pixelShader_;
};

} // namespace GraphicsFramework