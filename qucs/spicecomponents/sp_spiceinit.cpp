/***************************************************************************
                                sp_spiceinit.cpp
                               ---------------
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "sp_spiceinit.h"
#include "main.h"

#include <QFontMetrics>

SpiceSpiceinit::SpiceSpiceinit()
{
  isEquation = true;
  Type = isComponent;  // Analogue and digital component.
  Description = QObject::tr(".spiceinit file");

  QFont f = QucsSettings.font;
  f.setWeight(QFont::Light);
  f.setPointSizeF(12.0);
  QFontMetrics  metrics(f, 0);  // use the the screen-compatible metric
  QSize r = metrics.size(0, QObject::tr(".spiceinit"));
  int xb = r.width()  >> 1;
  int yb = r.height() >> 1;

  Lines.append(new Line(-xb, -yb, -xb,  yb,QPen(Qt::darkRed,2)));
  Lines.append(new Line(-xb,  yb,  xb+3,yb,QPen(Qt::darkRed,2)));
  Texts.append(new Text(-xb+4,  -yb-3, QObject::tr(".spiceinit"), QColor(0,0,0), 12.0));

  x1 = -xb-3;  y1 = -yb-5;
  x2 =  xb+9; y2 =  yb+3;

  tx = x1+4;
  ty = y2+4;

  Model = "SPICEINIT";
  Name  = "SPICEINIT";

  Props.append(new Property(".spiceinit contents", "", true,
                            "Insert .spiceinit contents"));
}

SpiceSpiceinit::~SpiceSpiceinit()
{
}

Component* SpiceSpiceinit::newOne()
{
  return new SpiceSpiceinit();
}

Element* SpiceSpiceinit::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr(".spiceinit contents");
  BitmapFile = (char *) "sp_spiceinit";

  if(getNewOne)  return new SpiceSpiceinit();
  return 0;
}

QString SpiceSpiceinit::getSpiceinit()
{
    QString spiceinit;
    if (isActive) spiceinit = Props.at(0)->Value+"\n";
    return spiceinit;
}
