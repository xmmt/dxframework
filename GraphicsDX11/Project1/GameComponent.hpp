#pragma once

#include "pch.h"

class GameComponent {
public:
    GameComponent() = default;

public:
    virtual void destroyResources() = 0;
    virtual void draw(ID3D11DeviceContext* context) = 0;
    virtual void initialize(ID3D11Device* device) = 0;
    virtual void reload() = 0;
    virtual void update() = 0;
};