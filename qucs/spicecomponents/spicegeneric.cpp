/***************************************************************************
                             spicegeneric.cpp
                             ---------------
    begin                : Mon Mar 07 2016
    copyright            : (C) 2016 by Vadim Kuznetsov
    email                : ra3xdh@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <QtCore>
#include <QFontMetrics>

#include "spicegeneric.h"

#include "misc.h"
#include "node.h"
#include "main.h"
#include "extsimkernels/spicecompat.h"


SpiceGeneric::SpiceGeneric()
{
  Description = QObject::tr("SPICE generic device");
  // Property descriptions not needed, but must not be empty !
  Props.append(new Property("NPins", "3", true, QObject::tr("Number of pins")));
  Props.append(new Property("Letter", "Z", true, QObject::tr("SPICE device letter")));
  Props.append(new Property("Model", "", false, QObject::tr(".MODEL definition reference (optional)")));
  Props.append(new Property("Params", "", false, QObject::tr("Parameter string (optional)")));

  Model = "SPICE_dev";
  SpiceModel = "X";
  Name  = "X";
  changed = false;

  // Do NOT call createSymbol() here. But create port to let it rotate.
  Ports.append(new Port(0, 0));
}

// -------------------------------------------------------
Component* SpiceGeneric::newOne()
{
  SpiceGeneric *p = new SpiceGeneric();
  p->recreate(0);   // createSymbol() is NOT called in constructor !!!
  return p;
}

// -------------------------------------------------------
Element* SpiceGeneric::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("XSPICE generic device");
  BitmapFile = (char *) "spicegeneric";

  if(getNewOne) {
    SpiceGeneric *p = new SpiceGeneric();
    p->recreate(0);   // createSymbol() is NOT called in constructor !!!
    return p;
  }
  return 0;
}

// -------------------------------------------------------
void SpiceGeneric::createSymbol()
{
  QFont f = QucsSettings.font; // get the basic font
  // symbol text is smaller (10 pt default)
  f.setPointSize(10);
  // use the screen-compatible metric
  QFontMetrics  smallmetrics(f, 0);   // get size of text
  int fHeight = smallmetrics.lineSpacing();


  int No = Props.at(0)->Value.toInt();
  QString tmp;
  
  // draw symbol outline
  #define HALFWIDTH  27
  int h = 30*((No-1)/2) + 15;
  Lines.append(new Line(-HALFWIDTH, -h, HALFWIDTH, -h,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( HALFWIDTH, -h, HALFWIDTH,  h,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-HALFWIDTH,  h, HALFWIDTH,  h,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-HALFWIDTH, -h,-HALFWIDTH,  h,QPen(Qt::darkBlue,2)));

  int w, i = fHeight/2;

  tmp = QObject::tr("SPICE");
  w = smallmetrics.boundingRect(tmp).width();
  Texts.append(new Text(w/-2, -i, tmp));

  i = 0;
  int y = 15-h;
  while(i<No) { // add ports lines and numbers
    Lines.append(new Line(-40,  y,-HALFWIDTH,  y,QPen(Qt::darkBlue,2)));
    Ports.append(new Port(-40,  y));
    tmp = QString::number(i+1);
    w = smallmetrics.width(tmp);
    Texts.append(new Text(-40-w, y-fHeight-2, tmp)); // text right-aligned
    i++;

    if(i == No) break; // if odd number of ports there will be one port less on the right side
    Lines.append(new Line(HALFWIDTH,  y, 40,  y,QPen(Qt::darkBlue,2)));
    Ports.append(new Port( 40,  y));
    tmp = QString::number(i+1);
    Texts.append(new Text( 40, y-fHeight-2, tmp)); // text left-aligned
    y += 60;
    i++;
  }


  x1 = -30; y1 = -h-2;
  x2 =  30; y2 =  h+15;

  // compute component name text position - normal size font
  QFontMetrics  metrics(QucsSettings.font, 0);   // use the screen-compatible metric
  fHeight = metrics.lineSpacing();
  tx = x1+4;
  ty = y1 - fHeight - 4;
  foreach(Property *pp,Props) {
      if (pp->display) ty -= fHeight;
  }
  changed = true;
}

// ---------------------------------------------------
QString SpiceGeneric::netlist()
{
    return QString("");
}

QString SpiceGeneric::spice_netlist(bool)
{
    // form RefDes from unique device letter and device name
    QString s = Props.at(1)->Value + Name;
    foreach(Port *pp, Ports) {
        s += " " + spicecompat::normalize_node_name(pp->Connection->Name);
    }

    s += " " + Props.at(2)->Value; // Model
    s += " " + Props.at(3)->Value +"\n"; // Parameters

    return s;
}
