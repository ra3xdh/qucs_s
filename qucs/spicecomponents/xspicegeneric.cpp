/***************************************************************************
                              XspiceGeneric.cpp
                             ---------------
    begin                : Tue Dez 28 2004
    copyright            : (C) 2004 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
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

#include "xspicegeneric.h"

#include "misc.h"
#include "node.h"
#include "main.h"
#include "extsimkernels/spicecompat.h"


XspiceGeneric::XspiceGeneric()
{
  Description = QObject::tr("SPICE netlist file");
  // Property descriptions not needed, but must not be empty !
  Props.append(new Property("Npins", "2", true, tr("Number of pins")));
  Props.append(new Property("Model", "generic_model", false, tr(".MODEL definition reference")));

  Model = "XSPICE_A";
  SpiceModel = "A";
  Name  = "A";
  changed = false;

  // Do NOT call createSymbol() here. But create port to let it rotate.
  Ports.append(new Port(0, 0));
}

// -------------------------------------------------------
Component* XspiceGeneric::newOne()
{
  XspiceGeneric *p = new XspiceGeneric();
  p->recreate(0);   // createSymbol() is NOT called in constructor !!!
  return p;
}

// -------------------------------------------------------
Element* XspiceGeneric::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("XSPICE generic device");
  BitmapFile = (char *) "xspicegeneric";

  if(getNewOne) {
    XspiceGeneric *p = new XspiceGeneric();
    p->recreate(0);   // createSymbol() is NOT called in constructor !!!
    return p;
  }
  return 0;
}

// -------------------------------------------------------
void XspiceGeneric::createSymbol()
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
  #define HALFWIDTH  17
  int h = 30*((No-1)/2) + 15;
  Lines.append(new Line(-HALFWIDTH, -h, HALFWIDTH, -h,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( HALFWIDTH, -h, HALFWIDTH,  h,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-HALFWIDTH,  h, HALFWIDTH,  h,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-HALFWIDTH, -h,-HALFWIDTH,  h,QPen(Qt::darkBlue,2)));

  int w, i = fHeight/2;

  tmp = QObject::tr("xspice");
  w = smallmetrics.boundingRect(tmp).width();
  Texts.append(new Text(w/-2, -i, tmp));

  i = 1;
  int y = 15-h;
  while(i<No) { // add ports lines and numbers
    Lines.append(new Line(-30,  y,-HALFWIDTH,  y,QPen(Qt::darkBlue,2)));
    Ports.append(new Port(-30,  y));
    // tmp = PortNames.section(',', i, i).mid(4);
    tmp = QString::number(i);
    w = smallmetrics.width(tmp);
    Texts.append(new Text(-20-w, y-fHeight-2, tmp)); // text right-aligned
    i++;

    // if(i == No) break; // if odd number of ports there will be one port less on the right side
    Lines.append(new Line(HALFWIDTH,  y, 30,  y,QPen(Qt::darkBlue,2)));
    Ports.append(new Port( 30,  y));
    tmp = QString::number(i);
    Texts.append(new Text( 20, y-fHeight-2, tmp)); // text left-aligned
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
  if(Props.first()->display) ty -= fHeight;
  changed = true;
}

// ---------------------------------------------------
QString XspiceGeneric::netlist()
{
    return QString("");
}

QString XspiceGeneric::spice_netlist(bool)
{
    QString s = SpiceModel;

    foreach(Port *pp,Ports) {
        s += " " + spicecompat::normalize_node_name(pp->Connection->Name);
    }
    s += " " + Props.at(1)->Value + "\n";

    return s;
}
