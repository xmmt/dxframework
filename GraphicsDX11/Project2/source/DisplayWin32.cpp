#include <DisplayWin32.hpp>
#include <Utils.hpp>

#include <windowsx.h>

using namespace GraphicsFramework;

namespace {
constexpr std::optional<InputDevice::Key> toInputDeviceKey(WPARAM key) {
    switch (key) {
        // clang-format off
    case VK_LBUTTON:    return InputDevice::Key::LEFT_MOUSE_BUTTON;
    case VK_RBUTTON:    return InputDevice::Key::RIGHT_MOUSE_BUTTON;
    case VK_BACK:       return std::nullopt;
    case VK_TAB:        return std::nullopt;
    case VK_RETURN:     return InputDevice::Key::ENTER;
    case 0x41:          return InputDevice::Key::A;
    case 0x42:          return std::nullopt;
    case 0x43:          return std::nullopt;
    case 0x44:          return InputDevice::Key::D;
    case 0x45:          return std::nullopt;
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
    case 0x51:          return std::nullopt;
    case 0x52:          return std::nullopt;
    case 0x53:          return InputDevice::Key::S;
    case 0x54:          return std::nullopt;
    case 0x55:          return std::nullopt;
    case 0x56:          return std::nullopt;
    case 0x57:          return InputDevice::Key::W;
    case 0x58:          return std::nullopt;
    case 0x59:          return std::nullopt;
    case 0x5A:          return std::nullopt;
    case VK_LSHIFT:     return InputDevice::Key::LEFT_SHIFT;
    case VK_RSHIFT:     return std::nullopt;
    case VK_LCONTROL:   return InputDevice::Key::LEFT_CTRL;
    case VK_RCONTROL:   return std::nullopt;
    default:            return std::nullopt;
        // clang-format on
    }
}

constexpr unsigned fromInputDeviceKey(InputDevice::Key key) {
    switch (key) {
        // clang-format off
    case InputDevice::Key::LEFT_MOUSE_BUTTON:   return VK_LBUTTON;
    case InputDevice::Key::RIGHT_MOUSE_BUTTON:  return VK_RBUTTON;
    case InputDevice::Key::ENTER:               return VK_RETURN;
    case InputDevice::Key::A:                   return 0x41;
    case InputDevice::Key::D:                   return 0x44;
    case InputDevice::Key::S:                   return 0x53;
    case InputDevice::Key::W:                   return 0x57;
    case InputDevice::Key::LEFT_SHIFT:          return VK_LSHIFT;
    case InputDevice::Key::LEFT_CTRL:           return VK_LCONTROL;
    default:                                    return 0x07; // undefined
        // clang-format on
    }
}
} // namespace

DisplayWin32::DisplayWin32(std::string title, HINSTANCE hInstance, InputDevice& inputDevice)
    : Display{ inputDevice }
    , hInstance_{ hInstance } {

    std::wstring wtitle{ title.begin(), title.end() };

    // Setup the windows class with default settings.
    wc_.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc_.lpfnWndProc = wndProc_;
    wc_.cbClsExtra = 0;
    wc_.cbWndExtra = 24;
    wc_.hInstance = hInstance_;
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
    int width = 800;
    int height = 800;

    // Place the window in the middle of the screen.
    auto posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
    auto posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;

    RECT windowRect = { 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

    auto dwStyle = WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX | WS_THICKFRAME; // WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP
    // Create the window with the screen settings and get the handle to it.
    hWnd_ = CreateWindowEx(WS_EX_APPWINDOW, wtitle.c_str(), wtitle.c_str(),
      dwStyle,
      posX, posY,
      windowRect.right - windowRect.left,
      windowRect.bottom - windowRect.top,
      nullptr, nullptr, hInstance, this);

    // SetWindowPos(hWnd_,
    //   HWND_TOP,
    //   posX, posY,
    //   windowRect.right - windowRect.left,
    //   windowRect.bottom - windowRect.top,
    //   SWP_NOSIZE | SWP_NOZORDER);

    ShowWindow(hWnd_, SW_SHOW);
    SetForegroundWindow(hWnd_);
    SetFocus(hWnd_);

    ShowCursor(true);

    inputDevice_.setKeyStateGetter([](InputDevice::Key key) -> bool {
        return GetKeyState(fromInputDeviceKey(key)) & 0x8000;
    });
    inputDevice_.setMousePositionGetter([hWnd{ hWnd_ }] {
        POINT point;
        //POINT* pp = &point;
        bool res = GetCursorPos(&point);
        //MapWindowPoints(NULL, hWnd, &pp, 1);
        res = ScreenToClient(hWnd, &point);
        return std::tuple<int, int>{ point.x, point.y };
    });
}

DisplayResult<void> DisplayWin32::runLoop(std::function<void(float)> runFrame) const {
    MSG msg_;
    bool isExitRequested_{ false };
    // Loop until there is a quit message from the window or the user.
    while (!isExitRequested_) {
        // Handle the windows messages.
        while (PeekMessage(&msg_, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg_);
            DispatchMessage(&msg_);
        }

        // If windows signals to end the application then exit out.
        if (msg_.message == WM_QUIT) {
            isExitRequested_ = true;
        }

        runFrame(0.f);

        // Utils::DebugWrite::info("mouse {} {}\n", std::get<0>(inputDevice_.getMousePosition().unwrap()), std::get<1>(inputDevice_.getMousePosition().unwrap()));
        //auto id = std::this_thread::get_id();
        //std::cout << "Main id: " << id << "\n";
    }
    return Ok();
}

LRESULT CALLBACK DisplayWin32::wndProc_(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam) {
    //auto id = std::this_thread::get_id();
    //WriteInfo() << "WndProc id: " << id << "\n";

    switch (uMessage) {
    case WM_NCCREATE: {
        SetLastError(0);
        SetWindowLongPtr(hWnd, DWLP_USER, (LONG_PTR)(((CREATESTRUCT*)lParam)->lpCreateParams));
        Utils::DebugWrite::info("SetWindowLongPtr: {}\n", GetLastError());
        return TRUE;
    }
        // Check if the window is being destroyed.
    case WM_DESTROY:
    case WM_CLOSE: {
        PostQuitMessage(0);
        return 0;
    }

    case WM_SIZE: {
        auto self = (DisplayWin32*)GetWindowLongPtr(hWnd, DWLP_USER);
        self->onSizeUpdated_({ LOWORD(lParam), HIWORD(lParam) });
        Utils::DebugWrite::info("Width {} Hight {}\n", LOWORD(lParam), HIWORD(lParam));
        return 0;
    }

    //case WM_WINDOWPOSCHANGED: {
    //    return 0;
    //}

    // Check if a key has been pressed on the keyboard.
    case WM_KEYDOWN: {
        // If a key is pressed send it to the input object so it can record that state.
        if (toInputDeviceKey(wParam).has_value()) {
            auto self = (DisplayWin32*)GetWindowLongPtr(hWnd, DWLP_USER);
            self->inputDevice_.onKeyPressed(toInputDeviceKey(wParam).value());
        }
        Utils::DebugWrite::info("Key: {}\n", wParam);

        if (static_cast<unsigned int>(wParam) == 27) {
            PostQuitMessage(0);
        }
        return 0;
    }

    // Check if a key has been released on the keyboard.
    case WM_KEYUP: {
        if (toInputDeviceKey(wParam).has_value()) {
            auto self = (DisplayWin32*)GetWindowLongPtr(hWnd, DWLP_USER);
            self->inputDevice_.onKeyReleased(toInputDeviceKey(wParam).value());
        }
        // If a key is released then send it to the input object so it can unset the state for that key.
        return 0;
    }

    case WM_MOUSEMOVE: {
        auto xPos = GET_X_LPARAM(lParam);
        auto yPos = GET_Y_LPARAM(lParam);
        auto self = (DisplayWin32*)GetWindowLongPtr(hWnd, DWLP_USER);
        self->inputDevice_.onMouseMove(InputDevice::MousePosition{ .x = xPos, .y = yPos });
        return 0;
    }

    case WM_LBUTTONDOWN: {
        auto self = (DisplayWin32*)GetWindowLongPtr(hWnd, DWLP_USER);
        self->inputDevice_.onKeyPressed(InputDevice::Key::LEFT_MOUSE_BUTTON);
        return 0;
    }
    case WM_LBUTTONUP: {
        auto self = (DisplayWin32*)GetWindowLongPtr(hWnd, DWLP_USER);
        self->inputDevice_.onKeyReleased(InputDevice::Key::LEFT_MOUSE_BUTTON);
        return 0;
    }
    case WM_RBUTTONDOWN: {
        auto self = (DisplayWin32*)GetWindowLongPtr(hWnd, DWLP_USER);
        self->inputDevice_.onKeyPressed(InputDevice::Key::RIGHT_MOUSE_BUTTON);
        return 0;
    }
    case WM_RBUTTONUP: {
        auto self = (DisplayWin32*)GetWindowLongPtr(hWnd, DWLP_USER);
        self->inputDevice_.onKeyReleased(InputDevice::Key::RIGHT_MOUSE_BUTTON);
        return 0;
    }
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:

    // All other messages pass to the message handler in the system class.
    default: {
        return DefWindowProc(hWnd, uMessage, wParam, lParam);
    }
    }
}

DisplayResult<DisplayWin32::Size> DisplayWin32::size() const {
    RECT rect;
    if (!GetWindowRect(hWnd_, &rect)) {
        return Err(DisplayError{
          .kind = DisplayError::Kind::UNDEFINED,
          .text = "" });
    }
    return Ok(Size{
      .width = static_cast<std::uint32_t>(rect.right - rect.left),
      .height = static_cast<std::uint32_t>(rect.bottom - rect.top) });
}

DisplayResult<void> DisplayWin32::show() {
    return Err(DisplayError{
      .kind = DisplayError::Kind::UNDEFINED,
      .text = "not implemented" });
}

DisplayResult<void> DisplayWin32::setTitle(std::string title) {
    return Err(DisplayError{
      .kind = DisplayError::Kind::UNDEFINED,
      .text = "not implemented" });
}

DisplayResult<HWND> DisplayWin32::hWnd() const {
    if (hWnd_ == 0) {
        return Err(DisplayError{
          .kind = DisplayError::Kind::BAD_STATE,
          .text = "window is not created",
        });
    }
    return Ok(hWnd_);
}