#include <dxframework/display.hpp>
#include <dxframework/utils.hpp>

#include <windowsx.h>
//#include<uxtheme.h>

using namespace dxframework;

namespace {
constexpr std::optional<input_device::key> to_input_device_key(WPARAM key) {
    switch (key) {
        // clang-format off
    case VK_LBUTTON:    return input_device::key::LEFT_MOUSE_BUTTON;
    case VK_RBUTTON:    return input_device::key::RIGHT_MOUSE_BUTTON;
    case VK_BACK:       return std::nullopt;
    case VK_TAB:        return std::nullopt;
    case VK_RETURN:     return input_device::key::ENTER;
    case 0x41:          return input_device::key::A;
    case 0x42:          return std::nullopt;
    case 0x43:          return std::nullopt;
    case 0x44:          return input_device::key::D;
    case 0x45:          return input_device::key::E;
    case 0x46:          return std::nullopt;
    case 0x47:          return std::nullopt;
    case 0x48:          return std::nullopt;
    case 0x49:          return std::nullopt;
    case 0x4A:          return std::nullopt;
    case 0x4B:          return std::nullopt;
    case 0x4C:          return std::nullopt;
    case 0x4D:          return std::nullopt;
    case 0x4E:          return std::nullopt;
    case 0x4F:          return std::nullopt;
    case 0x50:          return std::nullopt;
    case 0x51:          return input_device::key::Q;
    case 0x52:          return std::nullopt;
    case 0x53:          return input_device::key::S;
    case 0x54:          return std::nullopt;
    case 0x55:          return std::nullopt;
    case 0x56:          return std::nullopt;
    case 0x57:          return input_device::key::W;
    case 0x58:          return std::nullopt;
    case 0x59:          return std::nullopt;
    case 0x5A:          return std::nullopt;
    case VK_LSHIFT:     return input_device::key::LEFT_SHIFT;
    case VK_RSHIFT:     return std::nullopt;
    case VK_LCONTROL:   return input_device::key::LEFT_CTRL;
    case VK_RCONTROL:   return std::nullopt;
    case VK_LEFT:       return input_device::key::LEFT;
    case VK_UP:         return input_device::key::UP;
    case VK_RIGHT:      return input_device::key::RIGHT;
    case VK_DOWN:       return input_device::key::DOWN;
    default:            return std::nullopt;
        // clang-format on
    }
}

constexpr unsigned from_input_device_key(input_device::key key) {
    switch (key) {
        // clang-format off
    case input_device::key::LEFT_MOUSE_BUTTON:   return VK_LBUTTON;
    case input_device::key::RIGHT_MOUSE_BUTTON:  return VK_RBUTTON;
    case input_device::key::ENTER:               return VK_RETURN;
    case input_device::key::A:                   return 0x41;
    case input_device::key::D:                   return 0x44;
    case input_device::key::S:                   return 0x53;
    case input_device::key::W:                   return 0x57;
    case input_device::key::E:                   return 0x45;
    case input_device::key::Q:                   return 0x51;
    case input_device::key::LEFT_SHIFT:          return VK_LSHIFT;
    case input_device::key::LEFT_CTRL:           return VK_LCONTROL;
    case input_device::key::LEFT:                return VK_LEFT;
    case input_device::key::UP:                  return VK_UP;
    case input_device::key::RIGHT:               return VK_RIGHT;
    case input_device::key::DOWN:                return VK_DOWN;
    default:                                     return 0x07; // undefined
        // clang-format on
    }
}
} // namespace

display::display(std::string title, HINSTANCE h_instance, input_device& input_device)
    : input_device_{ input_device }
    , h_instance_{ h_instance } {

    std::wstring wtitle{ title.begin(), title.end() };

    // Setup the windows class with default settings.
    wc_.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc_.lpfnWndProc = wnd_proc_;
    wc_.cbClsExtra = 0;
    wc_.cbWndExtra = 24;
    wc_.hInstance = h_instance_;
    wc_.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
    wc_.hIconSm = wc_.hIcon;
    wc_.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc_.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
    wc_.lpszMenuName = nullptr;
    wc_.lpszClassName = wtitle.c_str();
    wc_.cbSize = sizeof(WNDCLASSEX);

    // Register the window class.
    RegisterClassEx(&wc_);

    // Determine the resolution of the clients desktop screen.
    auto screenWidth = GetSystemMetrics(SM_CXSCREEN);
    auto screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // If windowed then set it to 800x800 resolution.
    int width = 1800;
    int height = 1800;

    // Place the window in the middle of the screen.
    auto posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
    auto posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;

    RECT windowRect = { 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

    auto dwStyle = WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX | WS_THICKFRAME; // WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP
    // Create the window with the screen settings and get the handle to it.
    h_wnd_ = CreateWindowEx(WS_EX_APPWINDOW, wtitle.c_str(), wtitle.c_str(),
      dwStyle,
      posX, posY,
      windowRect.right - windowRect.left,
      windowRect.bottom - windowRect.top,
      nullptr, nullptr, h_instance, this);

    // SetWindowPos(h_wnd_,
    //   HWND_TOP,
    //   posX, posY,
    //   windowRect.right - windowRect.left,
    //   windowRect.bottom - windowRect.top,
    //   SWP_NOSIZE | SWP_NOZORDER);

    //SetWindowTheme(h_wnd_, L" ", L" ");

    ShowWindow(h_wnd_, SW_SHOW);
    SetForegroundWindow(h_wnd_);
    SetFocus(h_wnd_);

    ShowCursor(true);

    input_device_.set_key_state_getter([](input_device::key key) -> bool {
        return GetKeyState(from_input_device_key(key)) & 0x8000;
    });
    input_device_.set_mouse_position_getter([h_wnd{ h_wnd_ }] {
        POINT point;
        //POINT* pp = &point;
        bool res = GetCursorPos(&point);
        //MapWindowPoints(NULL, h_wnd, &pp, 1);
        res = ScreenToClient(h_wnd, &point);
        return std::tuple<int, int>{ point.x, point.y };
    });
}

void display::run_loop(std::function<void(float)> run_frame) const {
    MSG msg_;
    bool is_exit_requested_{ false };
    // Loop until there is a quit message from the window or the user.
    while (!is_exit_requested_) {
        // Handle the windows messages.
        while (PeekMessage(&msg_, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg_);
            DispatchMessage(&msg_);
        }

        // If windows signals to end the application then exit out.
        if (msg_.message == WM_QUIT) {
            is_exit_requested_ = true;
        }

        run_frame(0.f);

        // utils::debug_write::info("mouse {} {}\n", std::get<0>(input_device_.get_mouse_position().unwrap()), std::get<1>(input_device_.get_mouse_position().unwrap()));
        //auto id = std::this_thread::get_id();
        //std::cout << "Main id: " << id << "\n";
    }
}

LRESULT CALLBACK display::wnd_proc_(HWND h_wnd, UINT u_message, WPARAM w_param, LPARAM l_param) {
    //auto id = std::this_thread::get_id();
    //WriteInfo() << "WndProc id: " << id << "\n";

    switch (u_message) {
    case WM_NCCREATE: {
        SetLastError(0);
        SetWindowLongPtr(h_wnd, DWLP_USER, (LONG_PTR)(((CREATESTRUCT*)l_param)->lpCreateParams));
        utils::debug_write::info("SetWindowLongPtr: {}\n", GetLastError());
        return TRUE;
    }
        // Check if the window is being destroyed.
    case WM_DESTROY:
    case WM_CLOSE: {
        PostQuitMessage(0);
        return 0;
    }

    case WM_SIZE: {
        auto self = (display*)GetWindowLongPtr(h_wnd, DWLP_USER);
        self->on_size_updated_({ LOWORD(l_param), HIWORD(l_param) });
        utils::debug_write::info("Width {} Hight {}\n", LOWORD(l_param), HIWORD(l_param));
        return 0;
    }

    //case WM_WINDOWPOSCHANGED: {
    //    return 0;
    //}

    // Check if a key has been pressed on the keyboard.
    case WM_KEYDOWN: {
        // If a key is pressed send it to the input object so it can record that state.
        if (to_input_device_key(w_param).has_value()) {
            auto self = (display*)GetWindowLongPtr(h_wnd, DWLP_USER);
            self->input_device_.on_key_pressed(to_input_device_key(w_param).value());
        }
        utils::debug_write::info("key: {}\n", w_param);

        if (static_cast<unsigned int>(w_param) == 27) {
            PostQuitMessage(0);
        }
        return 0;
    }

    // Check if a key has been released on the keyboard.
    case WM_KEYUP: {
        if (to_input_device_key(w_param).has_value()) {
            auto self = (display*)GetWindowLongPtr(h_wnd, DWLP_USER);
            self->input_device_.on_key_released(to_input_device_key(w_param).value());
        }
        // If a key is released then send it to the input object so it can unset the state for that key.
        return 0;
    }

    case WM_MOUSEMOVE: {
        auto xPos = GET_X_LPARAM(l_param);
        auto yPos = GET_Y_LPARAM(l_param);
        auto self = (display*)GetWindowLongPtr(h_wnd, DWLP_USER);
        self->input_device_.on_mouse_move(input_device::mouse_position{ .x = xPos, .y = yPos });
        return 0;
    }

    case WM_LBUTTONDOWN: {
        auto self = (display*)GetWindowLongPtr(h_wnd, DWLP_USER);
        self->input_device_.on_key_pressed(input_device::key::LEFT_MOUSE_BUTTON);
        return 0;
    }
    case WM_LBUTTONUP: {
        auto self = (display*)GetWindowLongPtr(h_wnd, DWLP_USER);
        self->input_device_.on_key_released(input_device::key::LEFT_MOUSE_BUTTON);
        return 0;
    }
    case WM_RBUTTONDOWN: {
        auto self = (display*)GetWindowLongPtr(h_wnd, DWLP_USER);
        self->input_device_.on_key_pressed(input_device::key::RIGHT_MOUSE_BUTTON);
        return 0;
    }
    case WM_RBUTTONUP: {
        auto self = (display*)GetWindowLongPtr(h_wnd, DWLP_USER);
        self->input_device_.on_key_released(input_device::key::RIGHT_MOUSE_BUTTON);
        return 0;
    }
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:

    // All other messages pass to the message handler in the system class.
    default: {
        return DefWindowProc(h_wnd, u_message, w_param, l_param);
    }
    }
}

display::display_size display::size() const {
    RECT rect;
    if (!GetWindowRect(h_wnd_, &rect)) {
        throw std::runtime_error("con not get window rect");
    }
    return display_size{
        .width = static_cast<std::uint32_t>(rect.right - rect.left),
        .height = static_cast<std::uint32_t>(rect.bottom - rect.top)
    };
}

void display::show() {
    throw std::runtime_error("not implemented");
}

void display::set_title(std::string title) {
    throw std::runtime_error("not implemented");
}

HWND display::h_wnd() const {
    if (h_wnd_ == 0) {
        throw std::runtime_error("window was not created");
    }
    return h_wnd_;
}

void display::add_on_size_updated_callback(std::function<void(display_size)> callback) {
    on_size_updated_callbacks_.emplace_back(std::move(callback));
}

void display::on_size_updated_(display_size size) const {
    for (auto&& callback : on_size_updated_callbacks_) {
        callback(size);
    }
}
