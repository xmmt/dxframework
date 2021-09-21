#include <dxframework/utils.hpp>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <iostream>

namespace dxframework {
namespace utils {

void debug_write::info_(std::string_view const s) {
    std::cout << s;
    OutputDebugStringA(s.data());
}

void debug_write::error_(std::string_view const s) {
    std::cerr << s;
    OutputDebugStringA(s.data());
}

void enable_console() {
    AllocConsole();
    FILE* f_dummy;
    freopen_s(&f_dummy, "CONIN$", "r", stdin);
    freopen_s(&f_dummy, "CONOUT$", "w", stderr);
    freopen_s(&f_dummy, "CONOUT$", "w", stdout);
}

} // namespace utils
} // namespace dxframework