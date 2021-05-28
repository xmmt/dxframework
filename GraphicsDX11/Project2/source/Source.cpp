#include <Triangles.hpp>
#include <Utils.hpp>
#include <InputDevice.hpp>

#if _WIN32
#    include <RendererDirectX.hpp>
#    include <DisplayWin32.hpp>
#endif // _WIN32

using namespace GraphicsFramework;

#if _WIN32
#    define WIN32_LEAN_AND_MEAN
#    include <Windows.h>

//#pragma comment(lib, "d3d11.lib")
//#pragma comment(lib, "dxgi.lib")
//#pragma comment(lib, "d3dcompiler.lib")
//#pragma comment(lib, "dxguid.lib")

int WINAPI WinMain(
  _In_ HINSTANCE hInstance,
  _In_opt_ HINSTANCE hPrevInstance,
  _In_ LPSTR lpCmdline,
  _In_ int nShowCmd) {

#    if _DEBUG
    Utils::EnableConsole();
#    endif // _DEBUG
#endif // _WIN32

    InputDevice inputDevice;
    inputDevice.addOnKeyPressedCallback(InputDevice::Key::W, [] {
        Utils::DebugWrite::info("PRESSED W\n");
    });
    inputDevice.addOnKeyPressedCallback(InputDevice::Key::LEFT_MOUSE_BUTTON, [] {
        Utils::DebugWrite::info("PRESSED LEFT MOUSE BUTTON\n");
    });
    // inputDevice.addOnMouseMoveCallback([](InputDevice::MousePosition position) {
    //     Utils::DebugWrite::info("MOUSE {} {}\n", position.x, position.y);
    // });

#if _WIN32
    DisplayWin32 display{ "Triangles", hInstance, inputDevice };
    RendererDirectX renderer{ display };
#endif // _WIN32

    Triangles triangles{ renderer, inputDevice };

    auto res = triangles.run();

    if (res.isErr()) {
        Utils::DebugWrite::error("ERROR: Game \"{}\" was finished with error {}\n", "Triangles", res.unwrapErr().text);
    }

    Sleep(5000);

    return 0;
}