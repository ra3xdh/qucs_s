/***************************************************************************
                         NMOS_SPICE.cpp  -  description
                   --------------------------------------
    begin                     : Fri Mar 9 2007
    copyright                 : (C) 2007 by Gunther Kraut
    email                     : gn.kraut@t-online.de
    spice4qucs code added  Sat. 30 May 2015
    copyright                 : (C) 2015 by Mike Brinson
    email                     : mbrin72043@yahoo.co.uk

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "MOS_SPICE.h"
#include "node.h"
#include "misc.h"
#include "extsimkernels/spicecompat.h"


MOS_SPICE::MOS_SPICE()
{
  Description = QObject::tr("Unified (M,X,3-,4-pin) MOS:\nMultiple line ngspice or Xyce M model specifications allowed using \"+\" continuation lines.\nLeave continuation lines blank when NOT in use.");

  Props.append(new Property("Letter", "M", true,"[M,X] SPICE letter"));
  Props.append(new Property("Pins", "4", true,"[3,4] Pins count"));
  Props.append(new Property("type", "nmos", true,"[nmos,pmos] Channel type"));
  Props.append(new Property("M", "", true,"Param list and\n .model spec."));
  Props.append(new Property("M_Line 2", "", false,"+ continuation line 1"));
  Props.append(new Property("M_Line 3", "", false,"+ continuation line 2"));
  Props.append(new Property("M_Line 4", "", false,"+ continuation line 3"));
  Props.append(new Property("M_Line 5", "", false,"+ continuation line 4"));

  createSymbol();

  x1 = -30; y1 = -30;
  x2 =   4; y2 =  30;

    tx = x1+4;
    ty = y2+4;

    Model = "MOS_SPICE";
    SpiceModel = "M";
    Name  = "M";



}

MOS_SPICE::~MOS_SPICE()
{
}

Component* MOS_SPICE::newOne()
{
  return new MOS_SPICE();
}

Element* MOS_SPICE::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("unified MOSFET (3-4 pin)");
  BitmapFile = (char *) "NMOS_SPICE";

  if(getNewOne)  return new MOS_SPICE();
  return 0;
}

Element* MOS_SPICE::info_NM3pin(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("M(NMOS 3 pin)");
  BitmapFile = (char *) "NMOS_SPICE_3";

  if(getNewOne)  {
      MOS_SPICE *p = new MOS_SPICE();
      p->Props.at(0)->Value = "M";
      p->Props.at(1)->Value = "3";
      p->Props.at(2)->Value = "nmos";
      p->recreate(0);
      return p;
  }
  return 0;
}

Element* MOS_SPICE::info_PM3pin(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("M(PMOS 3 pin)");
  BitmapFile = (char *) "PMOS_SPICE_3";

  if(getNewOne)  {
      MOS_SPICE *p = new MOS_SPICE();
      p->Props.at(0)->Value = "M";
      p->Props.at(1)->Value = "3";
      p->Props.at(2)->Value = "pmos";
      p->recreate(0);
      return p;
  }
  return 0;
}

Element* MOS_SPICE::info_NX3pin(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("X(NMOS 3 pin)");
  BitmapFile = (char *) "NMOS_SPICE_3";

  if(getNewOne)  {
      MOS_SPICE *p = new MOS_SPICE();
      p->Name = "X";
      p->Props.at(0)->Value = "X";
      p->Props.at(1)->Value = "3";
      p->Props.at(2)->Value = "nmos";
      p->recreate(0);
      return p;
  }
  return 0;
}

Element* MOS_SPICE::info_PX3pin(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("X(PMOS 3 pin)");
  BitmapFile = (char *) "PMOS_SPICE_3";

  if(getNewOne)  {
      MOS_SPICE *p = new MOS_SPICE();
      p->Name = "X";
      p->Props.at(0)->Value = "X";
      p->Props.at(1)->Value = "3";
      p->Props.at(2)->Value = "pmos";
      p->recreate(0);
      return p;
  }
  return 0;
}

Element* MOS_SPICE::info_NX4pin(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("X(NMOS 4 pin)");
  BitmapFile = (char *) "NMOS_SPICE";

  if(getNewOne)  {
      MOS_SPICE *p = new MOS_SPICE();
      p->Name = "X";
      p->Props.at(0)->Value = "X";
      p->Props.at(1)->Value = "4";
      p->Props.at(2)->Value = "nmos";
      p->recreate(0);
      return p;
  }
  return 0;
}

Element* MOS_SPICE::info_PX4pin(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("X(PMOS 4 pin)");
  BitmapFile = (char *) "PMOS_SPICE";

  if(getNewOne)  {
      MOS_SPICE *p = new MOS_SPICE();
      p->Name = "X";
      p->Props.at(0)->Value = "X";
      p->Props.at(1)->Value = "4";
      p->Props.at(2)->Value = "pmos";
      p->recreate(0);
      return p;
  }
  return 0;
}


QString MOS_SPICE::netlist()
{
    return QString("");
}

void MOS_SPICE::createSymbol()
{
    Lines.append(new qucs::Line(-14,-13,-14, 13,QPen(Qt::darkRed,3)));

    Lines.append(new qucs::Line(-30,  0,-20,  0,QPen(Qt::darkBlue,3)));
    Lines.append(new qucs::Line(-20,  0,-14,  0,QPen(Qt::darkRed,3)));

    Lines.append(new qucs::Line(-10,-11,  0,-11,QPen(Qt::darkRed,3)));

    Lines.append(new qucs::Line(  0,-11,  0,-20,QPen(Qt::darkRed,3)));
    Lines.append(new qucs::Line(  0,-20,  0,-30,QPen(Qt::darkBlue,3)));

    Lines.append(new qucs::Line(-10, 11,  0, 11,QPen(Qt::darkRed,3)));
    Lines.append(new qucs::Line(  0, 11,  0, 20,QPen(Qt::darkRed,3)));
    Lines.append(new qucs::Line(  0, 20,  0, 30,QPen(Qt::darkBlue,3)));

    Lines.append(new qucs::Line(-10,-16,-10, -7,QPen(Qt::darkRed,3)));
    Lines.append(new qucs::Line(-10,  7,-10, 16,QPen(Qt::darkRed,3)));

    Lines.append(new qucs::Line(-10, -8,-10,  8,QPen(Qt::darkRed,3)));
    Lines.append(new qucs::Line( -4, 24,  4, 20,QPen(Qt::darkRed,2)));

    Ports.append(new Port(  0,-30)); //D
    Ports.append(new Port(-30,  0)); //G
    Ports.append(new Port(  0, 30)); //S
    if (Props.at(1)->Value=="4") {
        Ports.append(new Port( 20,  0)); //B
        Lines.append(new qucs::Line( 10,  0, 20,  0,QPen(Qt::darkBlue,3)));
        Lines.append(new qucs::Line(-10,  0, 10,  0,QPen(Qt::darkRed,3)));
    } else {
        Lines.append(new qucs::Line(-10,  0, 0,  0,QPen(Qt::darkRed,3)));
        Lines.append(new qucs::Line(0,  0, 0,  10,QPen(Qt::darkRed,3)));
    }

    if (Props.at(2)->Value=="nmos") {
        Lines.append(new qucs::Line( -9,  0, -4, -5,QPen(Qt::darkRed,3)));
        Lines.append(new qucs::Line( -9,  0, -4,  5,QPen(Qt::darkRed,3)));
    } else {
        Lines.append(new qucs::Line( -1,  0, -6, -5,QPen(Qt::darkRed,3)));
        Lines.append(new qucs::Line( -1,  0, -6,  5,QPen(Qt::darkRed,3)));
    }
}

QString MOS_SPICE::spice_netlist(bool)
{
    QString s = spicecompat::check_refdes(Name,Props.at(0)->Value);
    foreach(Port *p1, Ports) {
        QString nam = p1->Connection->Name;
        if (nam=="gnd") nam = "0";
        s += " "+ nam+" ";   // node names
    }
 
    QString M= Props.at(3)->Value;
    QString M_Line_2= Props.at(4)->Value;
    QString M_Line_3= Props.at(5)->Value;
    QString M_Line_4= Props.at(6)->Value;
    QString M_Line_5= Props.at(7)->Value;

    if(  M.length()  > 0)          s += QString("%1").arg(M);
    if(  M_Line_2.length() > 0 )   s += QString("\n%1").arg(M_Line_2);
    if(  M_Line_3.length() > 0 )   s += QString("\n%1").arg(M_Line_3);
    if(  M_Line_4.length() > 0 )   s += QString("\n%1").arg(M_Line_4);
    if(  M_Line_5.length() > 0 )   s += QString("\n%1").arg(M_Line_5);
    s += "\n";

    return s;
}
