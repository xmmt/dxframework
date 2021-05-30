#include <Material.hpp>

using namespace GraphicsFramework;

Material::Material(VertexShader& vertexShader, PixelShader& pixelShader)
    : vertexShader_{ vertexShader }
    , pixelShader_{ pixelShader } {
}