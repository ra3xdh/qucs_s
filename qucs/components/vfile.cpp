/***************************************************************************
                                vfile.cpp
                               -----------
    begin                : Fri April 07 2007
    updated drawing      : Wed October 03 2007
    copyright            : (C) 2007 by Gunther Kraut
    email                : gn.kraut@t-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "vfile.h"
#include "schematic.h"
#include "misc.h"

#include "extsimkernels/spicecompat.h"

vFile::vFile()
{
  Description = QObject::tr("file based voltage source");

  Arcs.append(new qucs::Arc(-12,-12, 24, 24,     0, 16*360,QPen(Qt::darkBlue,2)));
  Arcs.append(new qucs::Arc( -3, -7,  7,  7,16*270, 16*180,QPen(Qt::darkBlue,2)));
  Arcs.append(new qucs::Arc( -3,  0,  7,  7, 16*90, 16*180,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-30,  0,-12,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 30,  0, 12,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 18,  5, 18, 11,QPen(Qt::red,1)));
  Lines.append(new qucs::Line( 21,  8, 15,  8,QPen(Qt::red,1)));
  Lines.append(new qucs::Line(-18,  5,-18, 11,QPen(Qt::black,1)));

  Lines.append(new qucs::Line( -6,-17, -6,-21,QPen(Qt::darkBlue,1)));
  Lines.append(new qucs::Line( -8,-17, -8,-21,QPen(Qt::darkBlue,1)));
  Lines.append(new qucs::Line(-10,-17,-10,-21,QPen(Qt::darkBlue,1)));
  Lines.append(new qucs::Line( -3,-15, -3,-23,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-13,-15,-13,-23,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( -3,-23,-13,-23,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( -3,-15,-13,-15,QPen(Qt::darkBlue,2)));

  Ports.append(new Port( 30,  0));
  Ports.append(new Port(-30,  0));

  x1 = -30; y1 = -14;
  x2 =  30; y2 =  14;

  tx = x1+4;
  ty = y2+4;
  Model = "Vfile";
  Name  = "V";
  SpiceModel = "A";

  Props.append(new Property("File", "vfile.dat", true,
		QObject::tr("name of the sample file")));
  Props.append(new Property("Interpolator", "linear", false,
        QObject::tr("interpolation type")+" [hold, linear]"));
  Props.append(new Property("Repeat", "no", false,
		QObject::tr("repeat waveform")+" [no, yes]"));
  Props.append(new Property("G", "1", false, QObject::tr("voltage gain")));
  Props.append(new Property("T", "0", false, QObject::tr("delay time")));

  rotate();  // fix historical flaw
}

vFile::~vFile()
{
}

Component* vFile::newOne()
{
  return new vFile();
}

Element* vFile::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("File Based Voltage Source");
  BitmapFile = (char *) "vfile";

  if(getNewOne)  return new vFile();
  return 0;
}

// -------------------------------------------------------
QString vFile::getSubcircuitFile()
{
  // construct full filename
  QString FileName = Props.getFirst()->Value;
  return misc::properAbsFileName(FileName);
}

// -------------------------------------------------------
QString vFile::netlist()
{
  QString s = Model+":"+Name;

  // output all node names
  for (Port *p1 : Ports)
    s += " "+p1->Connection->Name;   // node names

  // output file properties
  Property *p2 = Props.first();
  s += " "+p2->Name+"=\"{"+getSubcircuitFile()+"}\"";

  // output all remaining properties
  for(p2 = Props.next(); p2 != 0; p2 = Props.next())
    s += " "+p2->Name+"=\""+p2->Value+"\"";

  return s + "\n";
}

QString vFile::spice_netlist(bool isXyce)
{
    Q_UNUSED(isXyce);
    QString s = SpiceModel + Name;
    QString modname = "mod_" + Model + Name;
    QString p1 = spicecompat::normalize_node_name(Ports.at(0)->Connection->Name);
    QString p2 = spicecompat::normalize_node_name(Ports.at(1)->Connection->Name);
    s += QString(" %vd([%1 %2]) %3\n").arg(p1).arg(p2).arg(modname);
    QString file = getSubcircuitFile();
    QString sc = getProperty("G")->Value;
    QString step = "false";
    QString delay = getProperty("T")->Value;
    if (getProperty("Interpolator")->Value != "linear") step = "true";
    s += QString(".MODEL %1 filesource (file=\"%2\" amplscale=[%3] amplstep=%4 "
                 "amploffset=[0] timeoffset=%5 timescale=1)\n")
            .arg(modname).arg(file).arg(sc).arg(step).arg(delay);

    return s;
}
