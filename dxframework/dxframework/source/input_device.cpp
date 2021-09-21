#include <dxframework/input_device.hpp>

using namespace dxframework;

void input_device::add_on_key_pressed_callback(key key, std::function<void()> callback) {
    on_key_pressed_callbacks_.emplace(key, std::move(callback));
}

void input_device::add_on_key_released_callback(key key, std::function<void()> callback) {
    on_key_released_callbacks_.emplace(key, std::move(callback));
}

void input_device::add_on_mouse_move_callback(std::function<void(mouse_position)> callback) {
    on_mouse_move_ = std::move(callback);
}

void input_device::set_key_state_getter(std::function<bool(key)> callback) {
    key_state_getter_ = std::move(callback);
}

void input_device::set_mouse_position_getter(std::function<std::tuple<int, int>()> callback) {
    mouse_position_getter_ = std::move(callback);
}

void input_device::on_key_pressed(key key) const {
    auto r = on_key_pressed_callbacks_.equal_range(key);
    for (auto it = r.first; it != r.second; std::advance(it, 1)) {
        it->second();
    }
}

void input_device::on_key_released(key key) const {
    auto r = on_key_released_callbacks_.equal_range(key);
    for (auto it = r.first; it != r.second; std::advance(it, 1)) {
        it->second();
    }
}

void input_device::on_mouse_move(mouse_position position) const {
    if (on_mouse_move_) {
        on_mouse_move_(std::move(position));
    }
}

bool input_device::is_key_pressed(key key) const {
    if (key_state_getter_) {
        return key_state_getter_(key);
    }
    throw std::runtime_error("onkeyState callback is not set");
}

std::tuple<int, int> input_device::get_mouse_position() const {
    if (mouse_position_getter_) {
        return mouse_position_getter_();
    }
    throw std::runtime_error("onmouse_position callback is not set");
}