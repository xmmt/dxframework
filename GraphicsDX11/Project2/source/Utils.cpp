#include <Utils.hpp>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <iostream>

namespace GraphicsFramework {
namespace Utils {

void DebugWrite::info_(std::string_view const s) {
    std::cout << s;
    OutputDebugStringA(s.data());
}

void DebugWrite::error_(std::string_view const s) {
    std::cerr << s;
    OutputDebugStringA(s.data());
}

void EnableConsole() {
    AllocConsole();
    FILE* fDummy;
    freopen_s(&fDummy, "CONIN$", "r", stdin);
    freopen_s(&fDummy, "CONOUT$", "w", stderr);
    freopen_s(&fDummy, "CONOUT$", "w", stdout);
}

} // namespace Utils
} // namespace GraphicsFramework