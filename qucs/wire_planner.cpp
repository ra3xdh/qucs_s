#include "wire_planner.h"
#include <numeric>

namespace qucs_s {
namespace wire {

inline bool is_horizontal_or_vertical(const QPoint from, const QPoint to) {
    return from.x() == to.x() || from.y() == to.y();
}

std::vector<QPoint> straight(const QPoint from, const QPoint to) {
    return {from, to};
}

std::vector<QPoint> two_step_xy(const QPoint from, const QPoint to) {
    if (is_horizontal_or_vertical(from, to)) {
        return {from, to};
    };

    /*
    From o---+
             |
             o To
    */
    return {from, {to.x(), from.y()}, to};
}

std::vector<QPoint> two_step_yx(const QPoint from, const QPoint to) {
    if (is_horizontal_or_vertical(from, to)) {
        return {from, to};
    };

    /*
    From o
         |
         +---o To
    */
    return {from, {from.x(), to.y()}, to};
}

std::vector<QPoint> three_step_xy(const QPoint from, const QPoint to) {
    if (is_horizontal_or_vertical(from, to)) {
        return {from, to};
    };

    /*
    From o---+
             |
             +---o To
    */
    int mid_x = std::midpoint(from.x(), to.x());
    return {from, {mid_x, from.y()}, {mid_x, to.y()}, to};
}

std::vector<QPoint> three_step_yx(const QPoint from, const QPoint to) {
    if (is_horizontal_or_vertical(from, to)) {
        return {from, to};
    };

    /*
      o From
      |
      +---+
          |
          o To
    */
    int mid_y = std::midpoint(from.y(), to.y());
    return {from, {from.x(), mid_y}, {to.x(), mid_y}, to};
}


static const std::map<Planner::PlanType, Planner::RouterFunc> routers = {
    {Planner::PlanType::Straight, straight},
    {Planner::PlanType::TwoStepXY, two_step_xy},
    {Planner::PlanType::TwoStepYX, two_step_yx},
    {Planner::PlanType::ThreeStepXY,three_step_xy},
    {Planner::PlanType::ThreeStepYX, three_step_yx}
};


Planner::Planner() : current{routers.begin()} {}

std::vector<QPoint> Planner::plan(PlanType type, const QPoint& from, const QPoint& to) {
    return routers.at(type)(from, to);
}

std::vector<QPoint> Planner::plan(const QPoint& from, const QPoint& to) const {
    return current->second(from, to);
}

void Planner::next() {
    ++current;

    if (current != routers.cend()) {
        return;
    }

    current = routers.cbegin();
}

Planner::PlanType Planner::setType(Planner::PlanType other) {
    auto old = current->first;
    while (current->first != other) next();
    return old;
}

} // namespace wire
} // namespace qucs_s
