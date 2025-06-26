#include "property.h"
#include <QPainter>

// x and y are relative to component's x and y
void Property::paint(int x, int y, QPainter* p)
{
    p->drawText(x, y, 1, 1, Qt::TextDontClip, Name + "=" + Value, &br);
}