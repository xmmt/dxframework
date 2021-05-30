#pragma once

#include <Shader.hpp>

namespace GraphicsFramework {

class Material {
public:
    Material(VertexShader& vertexShader, PixelShader& pixelShader);
    virtual ~Material() = default;

private:
    VertexShader& vertexShader_;
    PixelShader& pixelShader_;
};

}; // namespace GraphicsFramework