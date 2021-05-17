#include "pch.h"

#include "DisplayWin32.hpp"
#include "Game.hpp"

DisplayWin32::DisplayWin32(HINSTANCE hInstance, std::wstring applicationName)
    : hInstance_{ hInstance } {
    // Setup the windows class with default settings.
    wc_.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc_.lpfnWndProc = Game::messageHandler;
    wc_.cbClsExtra = 0;
    wc_.cbWndExtra = 0;
    wc_.hInstance = hInstance_;
    wc_.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
    wc_.hIconSm = wc_.hIcon;
    wc_.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc_.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
    wc_.lpszMenuName = nullptr;
    wc_.lpszClassName = applicationName.c_str();
    wc_.cbSize = sizeof(WNDCLASSEX);

    // Register the window class.
    RegisterClassEx(&wc_);

    // Determine the resolution of the clients desktop screen.
    auto screenWidth = GetSystemMetrics(SM_CXSCREEN);
    auto screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // If windowed then set it to 800x800 resolution.
    clientWidth_ = 800;
    clientHeight_ = 800;

    // Place the window in the middle of the screen.
    auto posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
    auto posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;

    RECT windowRect = { 0, 0, static_cast<LONG>(clientWidth_), static_cast<LONG>(clientHeight_) };
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

    auto dwStyle = WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX | WS_THICKFRAME; // WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP
    // Create the window with the screen settings and get the handle to it.
    hWnd_ = CreateWindowEx(WS_EX_APPWINDOW, applicationName.c_str(), applicationName.c_str(),
      dwStyle,
      posX, posY,
      windowRect.right - windowRect.left,
      windowRect.bottom - windowRect.top,
      nullptr, nullptr, hInstance, nullptr);


    ShowWindow(hWnd_, SW_SHOW);
    SetForegroundWindow(hWnd_);
    SetFocus(hWnd_);

    ShowCursor(true);
}

int DisplayWin32::width() {
    return clientWidth_;
}

int DisplayWin32::height() {
    return clientHeight_;
}

HWND DisplayWin32::hWnd() {
    return hWnd_;
}

void DisplayWin32::update() {
    RECT rect;
    if (GetWindowRect(hWnd_, &rect)) {
        clientWidth_ = rect.right - rect.left;
        clientHeight_ = rect.bottom - rect.top;
    }
}