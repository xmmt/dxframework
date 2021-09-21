#pragma once

#include <dxframework/input_device.hpp>
#include <dxframework/utils.hpp>
#include <dxframework/types.hpp>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <functional>

namespace dxframework {

class display {
public:
    using display_size = types::size<std::uint32_t>;

public:
    display(std::string title, HINSTANCE h_instance, input_device& input_device);
    ~display() = default;

public:
    void run_loop(std::function<void(float)> run_frame) const;

public:
    display_size size() const;
    void show();
    void set_title(std::string title);

    HWND h_wnd() const;

public:
    virtual void add_on_size_updated_callback(std::function<void(display_size)> callback);

private:
    void on_size_updated_(display_size size) const;

    static LRESULT CALLBACK wnd_proc_(HWND h_wnd, UINT u_message, WPARAM w_param, LPARAM l_param);

private:
    input_device& input_device_;
    std::list<std::function<void(display_size)>> on_size_updated_callbacks_;

    HINSTANCE h_instance_;
    HWND h_wnd_{};
    WNDCLASSEX wc_{};
};

} // namespace dxframework
