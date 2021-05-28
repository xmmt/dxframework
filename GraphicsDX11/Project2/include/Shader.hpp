#pragma once

#include <Utils.hpp>

namespace GraphicsFramework {

class Shader {
public:
    Shader() = default;
    virtual ~Shader() = default;

public:
protected:
};

class VertexShader : public Shader {
public:
};

class PixelShader : public Shader {
public:
};

} // namespace GraphicsFramework