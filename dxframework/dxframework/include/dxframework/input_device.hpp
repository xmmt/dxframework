#pragma once

#include <dxframework/utils.hpp>
#include <dxframework/types.hpp>

namespace dxframework {

class input_device {
public:
    enum class key : size_t {
        LEFT_MOUSE_BUTTON = 0,
        RIGHT_MOUSE_BUTTON,
        W,
        A,
        S,
        D,
        SPACEBAR,
        ESCAPE,
        ENTER,
        LEFT_SHIFT,
        LEFT_CTRL,
        _COUNT,
    };
    enum class event_type {
        PRESSED,
        RELEASED,
    };
    using mouse_position = types::coord<int>;

public:
    void add_on_key_pressed_callback(key key, std::function<void()> callback);
    void add_on_key_released_callback(key key, std::function<void()> callback);
    void add_on_mouse_move_callback(std::function<void(mouse_position)> callback);
    void set_key_state_getter(std::function<bool(key)> callback);
    void set_mouse_position_getter(std::function<std::tuple<int, int>()> callback);

public:
    void on_key_pressed(key key) const;
    void on_key_released(key key) const;
    void on_mouse_move(mouse_position position) const;

public:
    bool is_key_pressed(key key) const;
    std::tuple<int, int> get_mouse_position() const;

private:
    std::unordered_multimap<key, std::function<void()>> on_key_pressed_callbacks_;
    std::unordered_multimap<key, std::function<void()>> on_key_released_callbacks_;
    std::function<void(mouse_position)> on_mouse_move_;
    std::function<bool(key)> key_state_getter_;
    std::function<std::tuple<int, int>()> mouse_position_getter_;
};

} // namespace dxframework