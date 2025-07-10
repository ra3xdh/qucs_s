#include "property.h"
#include <QPainter>


// x and y are relative to component's x and y
void Property::paint(int x, int y, QPainter* p, const QString& displayValue /* = QString() */)
{
    const QString content(QString::fromUtf8("%1=%2%3%4").
            arg(Name).
            arg(displayValue.isEmpty() ? Value : displayValue).
            arg(unit.isEmpty() ? "" : " ").
            arg(unit));

    p->drawText(x, y, 1, 1, Qt::TextDontClip, content, &br);
}
