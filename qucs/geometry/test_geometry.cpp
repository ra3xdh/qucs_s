#include "concepts.h"
#include "one_point.h"
#include "multi_point.h"

using namespace qucs_s::geom;

struct Point {
    int cx;
    int cy;
    constexpr int x() const { return cx; }
    constexpr int y() const { return cy; }
};

static_assert(PointLike<Point>);
static_assert(PointLike<Point*>);
static_assert(PointLike<Point&>);

namespace test_rotate_around_center {
template <int expected_x, int expected_y>
constexpr bool invoke_and_expect(int px, int py, int rcx, int rcy) {
    rotate_point_ccw(px, py, rcx, rcy);
    return px == expected_x && py == expected_y;
}

// Point is at the center of rotation
static_assert(invoke_and_expect<0, 0>(0, 0, 0, 0));

// Center of rotation is the origin (zero, zero)
static_assert(invoke_and_expect<-1, -1>(1, -1, 0, 0));
static_assert(invoke_and_expect<-1, 1>(-1, -1, 0, 0));
static_assert(invoke_and_expect<1, 1>(-1, 1, 0, 0));
static_assert(invoke_and_expect<1, -1>(1, 1, 0, 0));

// Arbitrary center of rotation
static_assert(invoke_and_expect<4, 7>(4, 7, 4, 7));
static_assert(invoke_and_expect<-4, 10>(1, -1, 4, 7));
static_assert(invoke_and_expect<10, -2>(-1, -1, 4, -7));
static_assert(invoke_and_expect<-10, 4>(-1, 1, -4, 7));
static_assert(invoke_and_expect<4, -12>(1, 1, -4, -7));
} // namespace test_rotate_around_center

namespace test_rotate_around_zero {
template <int expected_x, int exptected_y>
constexpr bool invoke_and_expect(int px, int py) {
    rotate_point_ccw(px, py);
    return px == expected_x && py == exptected_y;
}

static_assert(invoke_and_expect<0, 0>(0, 0));
static_assert(invoke_and_expect<-1, -1>(1, -1));
static_assert(invoke_and_expect<-1, 1>(-1, -1));
static_assert(invoke_and_expect<1, 1>(-1, 1));
static_assert(invoke_and_expect<1, -1>(1, 1));
} // namespace test_rotate_around_zero

namespace test_is_between {
/*
            A   B   C
            o   o   o
              D o   o E
                    o F
*/

constexpr Point A{-1, 1};
constexpr Point B{0, 1};
constexpr Point C{1, 1};
constexpr Point D{0, 0};
constexpr Point E{1, 0};
constexpr Point F{1, -1};

void run() {
    // Horizontal
    // in between
    assert(is_between(B, A, C));
    assert(is_between(B, C, A));
    // on edges
    assert(!is_between(A, A, C));
    assert(!is_between(C, A, C));
    // not in between
    assert(!is_between(D, A, C));

    // Vertical
    // in between
    assert(is_between(E, C, F));
    assert(is_between(E, F, C));
    // on edges
    assert(!is_between(C, C, F));
    assert(!is_between(F, F, C));
    // not in between
    assert(!is_between(D, C, F));

    // Diagonal
    // in between
    assert(is_between(D, A, F));
    assert(is_between(D, F, A));
    // on edges
    assert(!is_between(A, A, F));
    assert(!is_between(F, F, A));
    // not in between
    assert(!is_between(B, A, F));
}
} // namespace test_is_between

namespace test_distance {
/*
    A        B
    o        o

    C
    o
*/
constexpr Point A{-3, 3};
constexpr Point B{3, 3};
constexpr Point C{-3, -3};

void run() {
    assert(distance(A, B) == 6.0);
    assert(distance(A, C) == 6.0);
    assert(std::abs(distance(C, B) - 8.485281) < 0.000001);
}
} // namespace test_distance

namespace test_near_line {
/*
            o C

                o F
   A o              o B
        o E
            o D

*/
constexpr Point A{-10, 0};
constexpr Point B{10, 0};
constexpr Point C{0, 10};
constexpr Point D{0, -10};
constexpr Point E{-4, -4};
constexpr Point F{4, 4};

void run() {
    assert(!is_near_line(E, A, B, 3));
    assert(is_near_line(E, A, B, 5));

    assert(!is_near_line(F, A, B, 3));
    assert(is_near_line(F, A, B, 5));

    assert(!is_near_line(E, C, D, 3));
    assert(is_near_line(E, C, D, 5));

    assert(!is_near_line(F, C, D, 3));
    assert(is_near_line(F, C, D, 5));
}

} // namespace test_near_line

int main() {
    test_is_between::run();
    test_distance::run();
    test_near_line::run();
}