#ifndef GEOMETRY_ONE_POINT_H
#define GEOMETRY_ONE_POINT_H

#include <utility>

namespace qucs_s {
namespace geom {

inline constexpr int mirror_coordinate(int coord, int axis) noexcept {
    return axis - (coord - axis);
}


template <typename T>
inline constexpr void rotate_point_ccw(T& px, T& py) noexcept {
    std::swap(px, py);
    py = -py;
}


template <typename T>
constexpr void rotate_point_ccw(T& px, T& py, int rcx,
                                              int rcy) noexcept {
    // Point is at the center of rotation, no-op
    if (px == rcx && py == rcy) {
        return;
    }

    auto relative_x = px - rcx;
    auto relative_y = py - rcy;

    px = rcx + relative_y;
    py = rcy - relative_x;
}

} // namespace geom
} // namespace qucs_s

#endif