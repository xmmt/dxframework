#pragma once

#include <DirectXMath.h>

struct CB_VS_vertexshader {
    DirectX::XMMATRIX wvp_matrix;
    DirectX::XMMATRIX world_matrix;
};

struct CB_PS_light {
    DirectX::XMFLOAT3 dynamic_light_color;
    float dynamic_light_strength;
    DirectX::XMFLOAT3 dynamic_light_position;
};

struct fog_buffer {
    DirectX::XMFLOAT4 g_fog_color;
    float g_fog_start;
    float g_fog_range;

    DirectX::XMFLOAT3 eye_pos_w;
};

struct light_buffer {
    DirectX::XMFLOAT3 ambient_light_color;
    float ambient_light_strength;
    float specular_power;
};