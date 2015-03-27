/***************************************************************************
                         vPWL.cpp  -  description
                   --------------------------------------
    begin                    : Fri Mar 9 2007
    copyright              : (C) 2007 by Gunther Kraut
    email                     : gn.kraut@t-online.de
    spice4qucs code added  Thurs. 26 March 2015
    copyright              : (C) 2015 by Mike Brinson
    email                    : mbrin72043@yahoo.co.uk

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "vPWL.h"
#include "node.h"
#include "misc.h"
#include "extsimkernels/spicecompat.h"


vPWL::vPWL()
{
  Description = QObject::tr("PWL voltage source");

  // normal voltage source symbol
  Arcs.append(new Arc(-12,-12, 24, 24,     0, 16*360,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-30,  0,-12,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 30,  0, 12,  0,QPen(Qt::darkBlue,2)));
  
  Lines.append(new Line( 18,  5, 18, 11,QPen(Qt::red,1)));
  Lines.append(new Line( 21,  8, 15,  8,QPen(Qt::red,1)));
  Lines.append(new Line(-18,  5,-18, 11,QPen(Qt::black,1)));

  // write 'PWL outside voltage source symbol
  //
 
 //P
  Lines.append(new Line( 10, 16, 16, 16, QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 16, 16, 16, 20,  QPen(Qt::darkBlue,2)));
  Lines.append(new Line(  16, 20, 13, 20, QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 13, 20, 13, 16,  QPen(Qt::darkBlue,2)));

 //W
  Lines.append(new Line( 16, 24, 10, 26,  QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 10, 26, 13, 28, QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 13, 28, 10, 30,  QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 10, 30, 16, 32, QPen(Qt::darkBlue,2)));
  
  //L
  Lines.append(new Line( 16, 36, 10, 36,  QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 10, 36, 10, 40, QPen(Qt::darkBlue,2))); 

  Ports.append(new Port( 30,  0));
  Ports.append(new Port(-30,  0));

  x1 = -30; y1 = -14;
  x2 =  30; y2 =  40;

  tx = x1+4;
  ty = y2+4;
  Model = "vPWL";
  SpiceModel = "V";
  Name  = "V";

  Props.append(new Property("PWL1", " ", true,"Expression"));
 Props.append(new Property("PWL2", " ", false,"Expression"));
 Props.append(new Property("PWL3", " ", false,"Expression"));
 Props.append(new Property("PWL4", " ", false,"Expression"));
 Props.append(new Property("PWL5", " ", false,"Expression"));
 Props.append(new Property("PWL6", " ", false,"Expression"));
 Props.append(new Property("PWL7", " ", false,"Expression"));
 Props.append(new Property("PWL8", " ", false,"Expression"));
 Props.append(new Property("PWL9", " ", false,"Expression"));
 Props.append(new Property("PWL10", " ", false,"Expression"));
 
 
  rotate();  // fix historical flaw
}

vPWL::~vPWL()
{
}

Component* vPWL::newOne()
{
  return new vPWL();
}

Element* vPWL::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("PWL");
  BitmapFile = (char *) "vPWL";

  if(getNewOne)  return new vPWL();
  return 0;
}

QString vPWL::netlist()
{
    return QString("");
}

QString vPWL::spice_netlist(bool isXyce)
{
    QString s = spicecompat::check_refdes(Name,SpiceModel);
    foreach(Port *p1, Ports) {
        QString nam = p1->Connection->Name;
        if (nam=="gnd") nam = "0";
        s += " "+ nam;   // node names
    }


QString PWL1= Props.at(0)->Value;
QString PWL2= Props.at(1)->Value;
QString PWL3= Props.at(2)->Value;
QString PWL4= Props.at(3)->Value;
QString PWL5= Props.at(4)->Value;
QString PWL6= Props.at(5)->Value;
QString PWL7= Props.at(6)->Value;
QString PWL8= Props.at(7)->Value;
QString PWL9= Props.at(8)->Value;
QString PWL10= Props.at(9)->Value;

    s += QString(" DC 0  AC 0 %1\n%2\n%3\n%4\n%5\n%6\n%7\n%8\n%9\n%10\n").arg(PWL1).arg(PWL2).arg(PWL3).
                              arg(PWL4).arg(PWL5).arg(PWL6).arg(PWL7).arg(PWL8).arg(PWL9).arg(PWL10);
    return s;
}
