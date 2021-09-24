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

std::wstring converter::string_to_wide(std::string str) {
    std::wstring wide_string(str.begin(), str.end());
    return wide_string;
}

std::string converter::get_directory_from_path(std::string const& file_path) {
    size_t off1 = file_path.find_last_of('\\');
    size_t off2 = file_path.find_last_of('/');
    if (off1 == std::string::npos && off2 == std::string::npos)
    {
        return "";
    }
    if (off1 == std::string::npos) {
        return file_path.substr(0, off2);
    }
    if (off2 == std::string::npos) {
        return file_path.substr(0, off1);
    }
    //If both exists, need to use the greater offset
    return file_path.substr(0, max(off1, off2));
}

std::string converter::get_file_extension( std::string const& file_name) {
    size_t off = file_name.find_last_of('.');
    if (off == std::string::npos) {
        return {};
    }
    return std::string(file_name.substr(off + 1));
}

} // namespace utils
} // namespace dxframework