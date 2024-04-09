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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif


SpiceLibComp::SpiceLibComp()
{
  Type = isComponent;   // both analog and digital
  Description = QObject::tr("SPICE library device. You can attach symbol patterns to it.");
  Simulator = spicecompat::simSpice;

  QStringList patterns;
  getSymbolPatternsList(patterns);
  QString p_str = "[auto";
  if (!patterns.isEmpty()) p_str += "," + patterns.join(",");
  p_str += "]";
  Props.append(new Property("File", "", false, QObject::tr("SpiceLibrary file")));
  Props.append(new Property("Device", "", false, QObject::tr("Subcircuit entry (.SUBCKT) name")));
  Props.append(new Property("SymPattern", "auto", false, p_str));
  Props.append(new Property("Params", "", false, QObject::tr("Extra parameters list")));

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
  FileName += QString("/../share/" QUCS_NAME "/symbols/%1.sym").arg(Props.at(2)->Value);

  // Default symbol: LM358 in opamps.lib ---> opamps/LM358.sym
  QString LibName = misc::properAbsFileName(Props.at(0)->Value, containingSchematic);
  QString DefSym = LibName;
  QFileInfo inf(LibName); // Remove extension
  int l = inf.suffix().size();
  DefSym.chop(l+1);
  DefSym += "/" + Props.at(1)->Value + ".sym";
  QString CommonSym = inf.canonicalPath() + "/" + inf.baseName() + "/" + inf.baseName() + ".sym";

  tx = INT_MIN;
  ty = INT_MIN;
  if(loadSymbol(FileName) > 0) {  // try to load SpiceLibComp symbol
      removeUnusedPorts();
  } else if(loadSymbol(DefSym) > 0) {
      removeUnusedPorts();
  } else if(loadSymbol(CommonSym) > 0) { // CommonSymbol for all library
      removeUnusedPorts();
  } else {
    QStringList pins;
    No = spicecompat::getPins(LibName,Props.at(1)->Value,pins);
    Ports.clear();
    remakeSymbol(No,pins);  // no symbol was found -> create standard symbol
  }
}

void SpiceLibComp::removeUnusedPorts()
{
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

// ---------------------------------------------------------------------
void SpiceLibComp::remakeSymbol(int No, QStringList &pin_names)
{
  QString nam;
  int h = 30*((No-1)/2) + 15;
  Lines.append(new qucs::Line(-15, -h, 15, -h,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 15, -h, 15,  h,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-15,  h, 15,  h,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-15, -h,-15,  h,QPen(Qt::darkBlue,2)));
  Texts.append(new Text(-10, -6,"lib"));

  int i=0, y = 15-h;
  while(i<No) {
    i++;
    Lines.append(new qucs::Line(-30,  y,-15,  y,QPen(Qt::darkBlue,2)));
    Ports.append(new Port(-30,  y));
    if (i<=pin_names.count()) nam = pin_names.at(i-1);
    Texts.append(new Text(-25,y-14,nam));

    if(i == No) break;
    i++;
    Lines.append(new qucs::Line( 15,  y, 30,  y,QPen(Qt::darkBlue,2)));
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
    Result = analyseLine(Line, 4);
    if(Result < 0) return -7;   // line format error
    z += Result;
  }

  return -8;   // field not closed
}

QString SpiceLibComp::spice_netlist(bool)
{
    QString s = QString("X%1 ").arg(Name);
    for (Port *p1 : Ports) {
        s += " " + spicecompat::normalize_node_name(p1->Connection->Name);
    }
    s += QString(" %1 %2\n").arg(Props.at(1)->Value).arg(Props.at(3)->Value);
    return s;
}

QString SpiceLibComp::getSpiceModel()
{
    if (isActive != COMP_IS_ACTIVE) return QString("");
    QString f = misc::properAbsFileName(Props.at(0)->Value, containingSchematic);
    QString s = QString(".INCLUDE \"%1\"\n").arg(f);
    return s;
}



void SpiceLibComp::getSymbolPatternsList(QStringList &symbols)
{
    QString dir_name = QucsSettings.BinDir + "/../share/" QUCS_NAME "/symbols/";
    QDir sym_dir(dir_name);
    QStringList sym_files = sym_dir.entryList(QDir::Files);
    for (const QString& file : sym_files) {
        QFileInfo inf(file);
        symbols.append(inf.baseName());
    }
}
