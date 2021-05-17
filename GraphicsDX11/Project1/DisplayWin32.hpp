#pragma once

#include "pch.h"

class DisplayWin32 {
public:
    DisplayWin32(HINSTANCE hInstance, std::wstring applicationName);

public:
    int width();
    int height();
    HWND hWnd();
    void update();

private:
    int clientHeight_{};
    int clientWidth_{};
    HINSTANCE hInstance_{};
    HWND hWnd_{};
    HMODULE module_{};
    WNDCLASSEX wc_{};
};