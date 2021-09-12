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
    FILE* fDummy;
    freopen_s(&fDummy, "CONIN$", "r", stdin);
    freopen_s(&fDummy, "CONOUT$", "w", stderr);
    freopen_s(&fDummy, "CONOUT$", "w", stdout);
}

} // namespace utils
} // namespace dxframework