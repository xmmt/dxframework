#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <format>
#include <optional>
#include <functional>
#include <exception>
#include <source_location>

namespace dxframework {
namespace utils {

class debug_write {
public:
    debug_write() = delete;

public:
    template <typename... Args>
    static void info(Args&&... args) {
        info_(std::format(std::forward<Args>(args)...));
    }

    template <typename... Args>
    static void error(Args&&... args) {
        error_(std::format(std::forward<Args>(args)...));
    }

private:
    static void info_(std::string_view const s);
    static void error_(std::string_view const s);
};

void enable_console();

class converter {
public:
    static std::wstring string_to_wide(std::string str);
    static std::string get_directory_from_path(std::string const& file_path);
    static std::string get_file_extension(std::string const& file_name);
};

} // namespace utils

class com_exception : public std::exception {
public:
    com_exception(
      HRESULT const hr,
      std::source_location const location)
        : result_{ hr }
        , location_{ location } {
    }

    const char* what() const override {
        static char s_str[256] = {};
        std::format_to_n(
          s_str,
          std::size(s_str),
          "file: {}({}:{}) `{}`: Failure with HRESULT of {:#08x}",
          location_.file_name(),
          location_.line(),
          location_.column(),
          location_.function_name(),
          static_cast<unsigned int>(result_));
        //sprintf_s(s_str, "Failure with HRESULT of %08X",
        //  static_cast<unsigned int>(result));
        return s_str;
    }

private:
    HRESULT const result_;
    std::source_location const location_;
};

inline void throw_if_failed(
  HRESULT hr,
  std::source_location const location
  = std::source_location::current()) {
    if (FAILED(hr)) {
        throw com_exception(hr, location);
    }
}

} // namespace dxframework