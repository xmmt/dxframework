#include <dxframework/utils.hpp>
#include <dxframework/input_device.hpp>
#include <dxframework/display.hpp>
#include <dxframework/renderer.hpp>
#include <dxframework/triangles_game.hpp>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <format>
#include <iostream>

using namespace std;
using namespace dxframework;

int WINAPI WinMain(
  _In_ HINSTANCE h_instance,
  _In_opt_ HINSTANCE h_prev_instance,
  _In_ LPSTR lp_cmd_line,
  _In_ int n_show_cmd) {

#if _DEBUG
    utils::enable_console();
#endif // _DEBUG

    input_device input_dev;
    input_dev.add_on_key_pressed_callback(input_device::key::W, [] {
        utils::debug_write::info("PRESSED W\n");
    });
    input_dev.add_on_key_pressed_callback(input_device::key::LEFT_MOUSE_BUTTON, [] {
        utils::debug_write::info("PRESSED LEFT MOUSE BUTTON\n");
    });
    input_dev.add_on_mouse_move_callback([](input_device::mouse_position position) {
        utils::debug_write::info("MOUSE {} {}\n", position.x, position.y);
    });

    display display{ "Triangles", h_instance, input_dev };
    renderer renderer{ display };

    triangles_game tr_game{ renderer, input_dev };
    renderer.init();
    tr_game.init();

    tr_game.run();

    return 0;
}