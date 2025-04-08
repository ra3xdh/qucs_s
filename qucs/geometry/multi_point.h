#ifndef GEOMETRY_MULTI_POINT_H
#define GEOMETRY_MULTI_POINT_H

#include "concepts.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <functional>
#include <iterator>
#include <vector>

namespace internal {

template <qucs_s::geom::PointLike P> inline constexpr auto get_x(P&& point) {
    if constexpr (std::is_pointer_v<std::decay_t<P>>) {
        return point->x();
    } else {
        return point.x();
    }
}

template <qucs_s::geom::PointLike P> inline constexpr auto get_y(P&& point) {
    if constexpr (std::is_pointer_v<std::decay_t<P>>) {
        return point->y();
    } else {
        return point.y();
    }
}

template <typename T1, typename T2>
inline constexpr auto gt_lt(T1 m, std::pair<T2, T2> minmax) {
    return minmax.first < m && m < minmax.second;
}

constexpr double float_cmp_tolerance = 0.01;
} // namespace internal

namespace qucs_s::geom {

// Returns distance betweein two points
inline constexpr double distance(PointLike auto&& point_a, PointLike auto&& point_b) noexcept {
    using ::internal::get_x;
    using ::internal::get_y;
    const auto dx = get_x(point_a) - get_x(point_b);
    const auto dy = get_y(point_a) - get_y(point_b);
    return std::sqrt(dx * dx + dy * dy);
}

// Checks if distance from point M to line AB is less than Dist
template <PointLike P1, PointLike P2, PointLike P3>
constexpr bool is_near_line(P1&& point_m, P2&& point_a, P3&& point_b, int dist) {
    using ::internal::get_x;
    using ::internal::get_y;
    using ::internal::gt_lt;

    const auto ax = get_x(point_a);
    const auto ay = get_y(point_a);
    const auto bx = get_x(point_b);
    const auto by = get_y(point_b);

    // horizontal line
    if (ay == by) {
        return gt_lt(get_y(point_m), std::pair{ay - dist, ay + dist}) &&
               gt_lt(get_x(point_m), std::minmax(ax, bx));
    }

    // vertical line
    if (ax == bx) {
        return gt_lt(get_x(point_m), std::pair{ax - dist, ax + dist}) &&
               gt_lt(get_y(point_m), std::minmax(ay, by));
    }

    // generic cases below

    // at first check if it's near line ends
    if (distance(point_m, point_a) < dist || distance(point_m, point_b) < dist) {
        return true;
    }

    if (gt_lt(get_x(point_m), std::minmax(ax, bx)) && gt_lt(get_y(point_m), std::minmax(ay, by))) {

        const auto ab = distance(point_a, point_b);
        const auto am = distance(point_a, point_m);
        const auto bm = distance(point_b, point_m);

        // Is on line?
        if (am + bm - ab < internal::float_cmp_tolerance) {
            return true;
        }

        // half of perimeter
        const auto hp = (ab + am + bm) / 2;

        // Area of triangle ABM using Heron's formula
        const auto S = std::sqrt(hp * (hp - ab) * (hp - am) * (hp - bm));

        // At the same time it's area can be found as (1/2) * ab * h,
        // where h is the distance from ab to m

        return S / (0.5 * ab) < dist;
    }

    return false;
}

// Checks if point M lies on a line connecting points A and B, but on the points
inline constexpr bool is_between(PointLike auto&& point_m,
                                 PointLike auto&& point_a,
                                 PointLike auto&& point_b) noexcept {

    using namespace ::internal;

    const auto ax = get_x(point_a);
    const auto ay = get_y(point_a);
    const auto bx = get_x(point_b);
    const auto by = get_y(point_b);

    const auto x_minmax = std::minmax(ax, bx);
    const auto y_minmax = std::minmax(ay, by);

    const auto mx = get_x(point_m);
    const auto my = get_y(point_m);

    // corner case: points a and b form a horizontal line
    if (ay == by) {
        return my == ay && gt_lt(mx, x_minmax);
    }

    // corner case: points a and b form a vertical line
    if (ax == bx) {
        return mx == ax && gt_lt(my, y_minmax);
    }

    // trivial negative cases
    if (!gt_lt(mx, x_minmax) || !gt_lt(my, y_minmax)) {
        return false;
    }

    return (distance(point_m, point_a) + distance(point_m, point_b) -
            distance(point_a, point_b)) <= float_cmp_tolerance;
}

} // namespace qucs_s::geom

namespace internal {
// A comparator for points which helps sort them in order
// from "begin" point towards the "end" point. For example,
//
// if there are points
//  A
//  o
//    o B
//      o C
//
// then if Orderer::compare is used as comparator in std::sort,
//  Orderer(A, C) gives A B C
//  Orderer(C, A) gives C B A
//
// This class is actually quite dumb and is used presuming that
// all the points being sorted lie on the line between "begin"
// and "end"

template <qucs_s::geom::PointLike P> class Orderer {
  public:
    explicit Orderer(P&& begin, P&& end)
        : x_comparator(get_x(begin) < get_x(end)
                           ? [](int a, int b) { return a < b; }
                           : [](int a, int b) { return a > b; }),
          y_comparator(get_y(begin) < get_y(end)
                           ? [](int a, int b) { return a < b; }
                           : [](int a, int b) { return a > b; }) {}

    bool compare(P a, P b) const {
        if (get_x(a) == get_x(b)) {
            return y_comparator(get_y(a), get_y(b));
        }

        return x_comparator(get_x(a), get_x(b));
    }

  private:
    std::function<bool(int, int)> x_comparator;
    std::function<bool(int, int)> y_comparator;
};
} // namespace internal

namespace qucs_s::geom {
/**
    Given two points A and B and a set of points, returns a vector of
    points, which lie on the line AB (including points A and B), all
    ordered from A to B.

    Consider example:
    A   B   C
    o   o   o
      D o   o E
            o F

    on_line(A, C, {A, B, C, D, E, F}) = {A, B, C}
    on_line(C, A, {A, B, C, D, E, F}) = {C, B, A}
    on_line(C, F, {A, B, C, D, E, F}) = {C, E, F}
    on_line(F, C, {A, B, C, D, E, F}) = {F, E, C}
    on_line(A, F, {A, B, C, D, E, F}) = {A, D, F}
    on_line(F, A, {A, B, C, D, E, F}) = {F, A, D}
*/
//template <std::ranges::forward_range Container, PointLike P>
//    requires std::same_as<std::iter_value_t<Container>, P>
//std::vector<P> on_line(P first, P last, Container* points) {
template<PointLike P, typename It>
std::vector<P> on_line(P first, P last, It begin, It end) {
    assert(first != last);

    std::vector<P> in_between{first, last};

    std::copy_if(begin, end,
         std::back_inserter(in_between),
        [first, last](P point) { return is_between(point, first, last); });

    const ::internal::Orderer<P> ord(std::move(first), std::move(last));
    std::ranges::sort(in_between, [ord](P point_a, P point_b) {
        return ord.compare(point_a, point_b);
    });

    return in_between;
}
} // namespace qucs_s::geom

#endif