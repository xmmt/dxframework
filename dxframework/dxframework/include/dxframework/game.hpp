#pragma once

#include <dxframework/utils.hpp>
#include <dxframework/renderer.hpp>
#include <dxframework/input_device.hpp>

namespace dxframework {

class game {
public:
    game(renderer& ren, input_device& input_dev);
    virtual ~game() = default;

public:
    virtual void run();
    virtual void init();
    virtual void destroy();

protected:
    virtual void run_frame_(float delta) = 0;
    virtual void run_() = 0;
    virtual void init_() = 0;
    virtual void destroy_() = 0;

protected:
    bool initialized_{ false };
    renderer& renderer_;
    input_device& input_device_;
};

} // namespace dxframework
