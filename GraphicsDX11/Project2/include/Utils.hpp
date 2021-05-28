#pragma once

#include <Utils/Result.hpp>

#include <format>
#include <optional>

namespace GraphicsFramework {
namespace Utils {

class DebugWrite {
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

void EnableConsole();

} // namespace Utils
} // namespace GraphicsFramework