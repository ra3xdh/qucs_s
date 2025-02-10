#ifndef WIRE_PLANNER_H
#define WIRE_PLANNER_H

#include <QPoint>
#include <functional>
#include <map>

namespace qucs_s {
namespace wire {

class Planner {
  public:
    using RouterFunc = std::function<std::vector<QPoint>(QPoint, QPoint)>;
    enum class PlanType { TwoStepXY, TwoStepYX, ThreeStepXY, ThreeStepYX };

    Planner();
    static std::vector<QPoint> plan(PlanType type, const QPoint from, const QPoint to);
    std::vector<QPoint> plan(const QPoint from, const QPoint to) const;
    void next();
  private:
    std::map<PlanType, RouterFunc>::const_iterator current;
};

} // namespace wire
} // namespace qucs_s

#endif
