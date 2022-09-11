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
#include <QString>
#include <QStringList>
#include <QFontMetrics>
#include <QtGlobal>

#include "xspicegeneric.h"

#include "misc.h"
#include "node.h"
#include "main.h"
#include "extsimkernels/spicecompat.h"


XspiceGeneric::XspiceGeneric()
{
  Description = QObject::tr("XSPICE generic device");
  // Property descriptions not needed, but must not be empty !
  Props.append(new Property("PortList", "v,v", true, QObject::tr("PortsList")));
  Props.append(new Property("Model", "generic_model", false, QObject::tr(".MODEL definition reference")));

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

  QStringList t_ports = Props.at(0)->Value.split(',');
  QStringList n_ports;
  int k=0;
  foreach (QString t_port, t_ports) {
      t_port.remove(']').remove('[');
      if ((t_port.remove(' ').endsWith('d'))&&
          (t_port!="d")) {
          n_ports.append(t_port+QString::number(k)+"+");
          n_ports.append(t_port+QString::number(k)+"-");
      } else n_ports.append(t_port+QString::number(k));
      k++;
  }


  int No = n_ports.count();
  QString tmp;
  
  // draw symbol outline
  #define HALFWIDTH  27
  int h = 30*((No-1)/2) + 15;
  Lines.append(new qucs::Line(-HALFWIDTH, -h, HALFWIDTH, -h,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( HALFWIDTH, -h, HALFWIDTH,  h,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-HALFWIDTH,  h, HALFWIDTH,  h,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-HALFWIDTH, -h,-HALFWIDTH,  h,QPen(Qt::darkBlue,2)));

  int w, i = fHeight/2;

  tmp = QObject::tr("XSPICE");
  w = smallmetrics.boundingRect(tmp).width();
  Texts.append(new Text(w/-2, -i, tmp));

  i = 0;
  int y = 15-h;
  while(i<No) { // add ports lines and numbers
    Lines.append(new qucs::Line(-40,  y,-HALFWIDTH,  y,QPen(Qt::darkBlue,2)));
    Ports.append(new Port(-40,  y));
    // tmp = PortNames.section(',', i, i).mid(4);
    tmp = n_ports.at(i);
    w = smallmetrics.boundingRect(tmp).width();
    Texts.append(new Text(-40-w, y-fHeight-2, tmp)); // text right-aligned
    i++;

    if(i == No) break; // if odd number of ports there will be one port less on the right side
    Lines.append(new qucs::Line(HALFWIDTH,  y, 40,  y,QPen(Qt::darkBlue,2)));
    Ports.append(new Port( 40,  y));
    tmp = n_ports.at(i);
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
    QString s = spicecompat::check_refdes(Name,SpiceModel);

    QStringList t_ports = Props.at(0)->Value.split(',');

    int i=0;
    foreach(QString t_port,t_ports) {
        QString nod =  spicecompat::normalize_node_name(Ports.at(i)->Connection->Name);

        if (t_port.contains('[')) { // Process array ports
            t_port.remove('[');  // Defined by brackets
            s += '[';
        }
        bool closing_bracket = t_port.contains(']');
        if (closing_bracket) t_port.remove(']');

        if ((t_port.remove(' ').endsWith('d'))&&
            (t_port!="d")) {
            i++;
            QString nod1 =  spicecompat::normalize_node_name(Ports.at(i)->Connection->Name);
            s += QString(" %%1(%2 %3) ").arg(t_port).arg(nod).arg(nod1);
        } else {
            s += QString(" %%1(%2) ").arg(t_port).arg(nod);
        }
        if (closing_bracket) s += ']';
        i++;
    }
    s += " " + Props.at(1)->Value + "\n";

    return s;
}
