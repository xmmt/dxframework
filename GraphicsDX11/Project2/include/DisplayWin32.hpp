#include <Display.hpp>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace GraphicsFramework {

class DisplayWin32 : public Display {
public:
    DisplayWin32(std::string title, HINSTANCE hInstance, InputDevice& inputDevice);
    virtual ~DisplayWin32() = default;

public:
    virtual Result<Size, DisplayError> size() const override;
    virtual Result<void, DisplayError> show() override;
    virtual Result<void, DisplayError> setTitle(std::string title) override;

    Result<HWND, DisplayError> hWnd() const;

private:
    static LRESULT CALLBACK wndProc_(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);

private:
    HINSTANCE hInstance_;
    HWND hWnd_{};
    WNDCLASSEX wc_{};
};

} // namespace GraphicsFramework