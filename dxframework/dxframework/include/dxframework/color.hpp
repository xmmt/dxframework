#pragma once

#include <cstddef>
#include <compare>

namespace dxframework {

class color {
public:
    constexpr color() noexcept
        : color_{ 0 } {
    }
    constexpr color(unsigned int val) noexcept
        : color_{ val } {
    }
    constexpr color(std::byte r, std::byte g, std::byte b) noexcept
        : rgba_{ r, g, b, std::byte{ 255 } } {
    }
    constexpr color(unsigned char r, unsigned char g, unsigned char b) noexcept
        : rgba_{
            std::byte{ r },
            std::byte{ g },
            std::byte{ b },
            std::byte{ 255 }
        } {
    }
    constexpr color(std::byte r, std::byte g, std::byte b, std::byte a) noexcept
        : rgba_{ r, g, b, a } {
    }
    constexpr color(unsigned char r, unsigned char g, unsigned char b, unsigned char a) noexcept
        : rgba_{
            std::byte{ r },
            std::byte{ g },
            std::byte{ b },
            std::byte{ a }
        } {
    }

    constexpr color(color const&) noexcept = default;
    constexpr color(color&&) noexcept = default;
    constexpr color& operator=(color const&) noexcept = default;
    constexpr color& operator=(color&&) noexcept = default;
    constexpr ~color() = default;

    constexpr std::strong_ordering operator<=>(color const& other) const noexcept {
        return color_ <=> other.color_;
    }

    constexpr std::byte get_r() const noexcept {
        return rgba_[0];
    }
    constexpr void set_r(std::byte r) noexcept {
        rgba_[0] = r;
    }

    constexpr std::byte get_g() const noexcept {
        return rgba_[1];
    }
    constexpr void set_g(std::byte g) noexcept {
        rgba_[1] = g;
    }

    constexpr std::byte get_b() const noexcept {
        return rgba_[2];
    }
    constexpr void set_b(std::byte b) noexcept {
        rgba_[2] = b;
    }

    constexpr std::byte get_a() const noexcept {
        return rgba_[3];
    }
    constexpr void set_a(std::byte a) noexcept {
        rgba_[3] = a;
    }

private:
    union {
        std::byte rgba_[4];
        unsigned int color_;
    };
};

namespace colors {

constexpr const color unloaded_texture_color(std::byte{ 100 }, std::byte{ 100 }, std::byte{ 100 }, std::byte{ 100 });
constexpr const color unhandled_texture_color(250, 0, 0);

} // namespace colors

} // namespace dxframework