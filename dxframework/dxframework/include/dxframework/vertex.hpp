#pragma once

#include <DirectXMath.h>

namespace dxframework {

struct vertex_type {
    vertex_type() = default;
    vertex_type(float x, float y, float z, float u, float v, float nx, float ny, float nz)
        : pos(x, y, z)
        , tex_coord(u, v)
        , normal(nx, ny, nz) {
    }

    DirectX::XMFLOAT3 pos;
    DirectX::XMFLOAT2 tex_coord;
    DirectX::XMFLOAT3 normal;
};

//using vertex = DirectX::XMFLOAT4;

} // namespace dxframework
