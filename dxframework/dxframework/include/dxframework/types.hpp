#pragma once

namespace dxframework {
namespace types {

template <typename T>
struct coord {
    T x;
    T y;
};

template <typename T>
struct size {
    T width;
    T height;
};

} // namespace types
} // namespace dxframework