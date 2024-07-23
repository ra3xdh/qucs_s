#ifndef SIMULATION_H
#define SIMULATION_H
#include "component.h"
#include <QString>

namespace qucs::component {
class SimulationComponent : public Component {
private:
     QString label_text;
     void updateComponentBounds(const QRect& label_bounds);
     QPen pen() const;
protected:
     void initSymbol(const QString& label);
     void drawSymbol(QPainter* p) override;
     // Override in your subclass if you want other color
     // for your simulation component
     virtual Qt::GlobalColor color() const { return Qt::darkBlue; }
};
}
#endif