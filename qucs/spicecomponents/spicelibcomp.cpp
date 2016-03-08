/***************************************************************************
                             spicelibcomp.cpp
                              --------------
    begin                : Tue Mar 08 2016
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

#include "spicelibcomp.h"
#include "qucs.h"
#include "schematic.h"
#include "main.h"
#include "misc.h"
#include "extsimkernels/spicecompat.h"

#include <QTextStream>
#include <QFileInfo>
#include <QMutex>

#include <limits.h>



SpiceLibComp::SpiceLibComp()
{
  Type = isComponent;   // both analog and digital
  Description = QObject::tr("SpiceLibComp");

  Props.append(new Property("File", "", false, QObject::tr("SpiceLibrary file")));
  Props.append(new Property("Device", "", false, QObject::tr("Subcircuit name")));
  Props.append(new Property("SymPattern", "auto", false, QObject::tr("[auto,opamp3t,opamp5t]")));

  Model = "SpLib";
  Name  = "X";
  SpiceModel = "X";

  // Do NOT call createSymbol() here. But create port to let it rotate.
  Ports.append(new Port(0, 0, false));
}

// ---------------------------------------------------------------------
Component* SpiceLibComp::newOne()
{
  SpiceLibComp *p = new SpiceLibComp();
  p->Props.getFirst()->Value = Props.getFirst()->Value;
  p->recreate(0);
  return p;
}

// -------------------------------------------------------
Element* SpiceLibComp::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("SpiceLibComp");
  BitmapFile = (char *) "spicelibcomp";

  if(getNewOne) {
    SpiceLibComp *p = new SpiceLibComp();
    p->recreate(0);   // createSymbol() is NOT called in constructor !!!
    return p;
  }
  return 0;
}

// ---------------------------------------------------------------------
// Makes the schematic symbol SpiceLibComp with the correct number
// of ports.
void SpiceLibComp::createSymbol()
{
  int No;
  QString FileName = QucsSettings.BinDir;
  FileName += QString("/../share/qucs/symbols/%1.sym").arg(Props.at(2)->Value);

  tx = INT_MIN;
  ty = INT_MIN;
  if(loadSymbol(FileName) > 0) {  // try to load SpiceLibComp symbol
    if(tx == INT_MIN)  tx = x1+4;
    if(ty == INT_MIN)  ty = y2+4;
    // remove unused ports
    QMutableListIterator<Port *> ip(Ports);
    Port *pp;
    while (ip.hasNext()) {
      pp = ip.next();
      if(!pp->avail) {
          pp = ip.peekNext();
          ip.remove();
      }
    }
  }
  else {
    QStringList pins;
    No = getPins(pins);
    Ports.clear();
    remakeSymbol(No,pins);  // no symbol was found -> create standard symbol
  }
}

// ---------------------------------------------------------------------
void SpiceLibComp::remakeSymbol(int No, QStringList &pin_names)
{
  QString nam;
  int h = 30*((No-1)/2) + 15;
  Lines.append(new Line(-15, -h, 15, -h,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 15, -h, 15,  h,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-15,  h, 15,  h,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-15, -h,-15,  h,QPen(Qt::darkBlue,2)));
  Texts.append(new Text(-10, -6,"lib"));

  int i=0, y = 15-h;
  while(i<No) {
    i++;
    Lines.append(new Line(-30,  y,-15,  y,QPen(Qt::darkBlue,2)));
    Ports.append(new Port(-30,  y));
    if (i<=pin_names.count()) nam = pin_names.at(i-1);
    Texts.append(new Text(-25,y-14,nam));

    if(i == No) break;
    i++;
    Lines.append(new Line( 15,  y, 30,  y,QPen(Qt::darkBlue,2)));
    Ports.append(new Port( 30,  y));
    if (i<=pin_names.count()) nam = pin_names.at(i-1);
    Texts.append(new Text( 19,y-14,nam));
    y += 60;
  }

  x1 = -30; y1 = -h-2;
  x2 =  30; y2 =  h+2;
  tx = x1+4;
  ty = y2+4;
}

// ---------------------------------------------------------------------
// Loads the symbol for the SpiceLibComp from the schematic file and
// returns the number of painting elements.
int SpiceLibComp::loadSymbol(const QString& DocName)
{
    QString FileString;
    QFile symfile(DocName);
    if (symfile.open(QIODevice::ReadOnly)) {
        QTextStream ts(&symfile);
        FileString = ts.readAll();
        symfile.close();
    } else return -1;

  QString Line;
  QTextStream stream(&FileString, QIODevice::ReadOnly);


  // read content *************************
  while(!stream.atEnd()) {
    Line = stream.readLine();
    if(Line == "<Symbol>") break;
  }

  x1 = y1 = INT_MAX;
  x2 = y2 = INT_MIN;

  int z=0, Result;
  while(!stream.atEnd()) {
    Line = stream.readLine();
    if(Line == "</Symbol>") {
      x1 -= 4;   // enlarge component boundings a little
      x2 += 4;
      y1 -= 4;
      y2 += 4;
      return z;      // return number of ports
    }

    Line = Line.trimmed();
    if(Line.at(0) != '<') return -5;
    if(Line.at(Line.length()-1) != '>') return -6;
    Line = Line.mid(1, Line.length()-2); // cut off start and end character
    Result = analyseLine(Line, 3);
    if(Result < 0) return -7;   // line format error
    z += Result;
  }

  return -8;   // field not closed
}

QString SpiceLibComp::spice_netlist(bool)
{
    QString s = QString("X%1 ").arg(Name);
    foreach(Port *p1, Ports) {
        s += " " + spicecompat::normalize_node_name(p1->Connection->Name);
    }
    s += " " + Props.at(1)->Value;
    for(Property *pp = Props.at(3); pp != 0; pp = Props.next()) {
        s += QString(" %1=%2").arg(pp->Name).arg(spicecompat::normalize_value(pp->Value));
    }
    s += "\n";
    return s;
}

QString SpiceLibComp::getSpiceModel()
{
    QString f = spicecompat::convert_relative_filename(Props.at(0)->Value);
    QString s = QString(".INCLUDE \"%1\"\n").arg(f);
    return s;
}

int SpiceLibComp::getPins(QStringList &pin_names)
{
    int r = 0;
    QString content;
    QString LibName = spicecompat::convert_relative_filename(Props.at(0)->Value);
    QFile f(LibName);
    if (f.open(QIODevice::ReadOnly)) {
        QTextStream ts(&f);
        content = ts.readAll();
        f.close();
    } else return 0;

    QTextStream stream(&content,QIODevice::ReadOnly);
    while (!stream.atEnd()) {
        QString lin = stream.readLine();
        QRegExp subckt_header("^\\s*\\.(S|s)(U|u)(B|b)(C|c)(K|k)(T|t)\\s.*");
        if (subckt_header.exactMatch(lin)) {
            QRegExp sep("\\s");
            QStringList lst2 = lin.split(sep,QString::SkipEmptyParts);
            QString name = lin.section(sep,1,1,QString::SectionSkipEmpty).toLower();
            QString refname = Props.at(1)->Value.toLower();
            if (name != refname) continue;
            lst2.removeFirst();
            lst2.removeFirst();
            foreach (QString s1, lst2) {
                if (!s1.contains('=')) pin_names.append(s1);
            }
            r = pin_names.count();
            break;
        }
    }

    return r;
}
