#pragma once

#include <dxframework/utils.hpp>
#include <dxframework/renderer.hpp>

namespace dxframework {

class game_component {
public:
    game_component(renderer& ren);
    virtual ~game_component() = default;

public:
    virtual void destroy_resources() = 0;
    virtual void draw() = 0;
    virtual void initialize() = 0;
    virtual void reload() = 0;
    virtual void update() = 0;

protected:
    renderer& renderer_;
};

} // namespace dxframework