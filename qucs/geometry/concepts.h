#ifndef GEOMETRY_CONCEPTS_H
#define GEOMETRY_CONCEPTS_H

#include <concepts>

namespace qucs_s {
namespace geom {

template <typename T>
concept PointLike = requires(T t) {
    { t.x() } -> std::same_as<int>;
    { t.y() } -> std::same_as<int>;
} || requires(T t) {
    { t->x() } -> std::same_as<int>;
    { t->y() } -> std::same_as<int>;
};

}
} // namespace qucs_s

#endif