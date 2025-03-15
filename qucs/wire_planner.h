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
    enum class PlanType { TwoStepXY, TwoStepYX, ThreeStepXY, ThreeStepYX, Straight };

    Planner();
    static std::vector<QPoint> plan(PlanType type, const QPoint from, const QPoint to);
    std::vector<QPoint> plan(const QPoint from, const QPoint to) const;
    void next();
    PlanType planType() const { return current->first; }
    PlanType setType(PlanType n);
  private:
    std::map<PlanType, RouterFunc>::const_iterator current;
};

} // namespace wire
} // namespace qucs_s

#endif
