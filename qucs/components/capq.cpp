/*
* capq.cpp - Lossy capacitor implementation
*
* copyright (C) 2015 Andres Martinez-Mera <andresmartinezmera@gmail.com>
*
* This is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2, or (at your option)
* any later version.
*
* This software is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this package; see the file COPYING.  If not, write to
* the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
* Boston, MA 02110-1301, USA.
*
*
*/
#include "capq.h"


CapQ::CapQ()
{
  Description = QObject::tr("Lossy capacitor");

  //Lines connection device and ports
  Lines.append(new qucs::Line( -4,-11, -4, 11,QPen(Qt::darkBlue,4)));
  Lines.append(new qucs::Line(  4,-11,  4, 11,QPen(Qt::darkBlue,4)));


  Lines.append(new qucs::Line(-30,  0, -4,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(  4,  0, 30,  0,QPen(Qt::darkBlue,2)));


  //Draw Q
  //Horizontal lines
  Lines.append(new qucs::Line( 10,  -10, 17,  -10,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 10,  -17, 17,  -17,QPen(Qt::darkBlue,2)));
  //Vertical lines
  Lines.append(new qucs::Line( 17,  -10, 17,  -17,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 10,  -10, 10,  -17,QPen(Qt::darkBlue,2)));
  //Middle line
  Lines.append(new qucs::Line( 18,  -9, 14,  -13,QPen(Qt::darkBlue,2)));



  Ports.append(new Port(-30,  0));
  Ports.append(new Port( 30,  0));

  x1 = -30; y1 = -13;
  x2 =  30; y2 =  13;

  tx = x1+4;
  ty = y2+4;
  Model = "CAPQ";
  Name  = "CAPQ";

  Props.append(new Property("C", "1 pF", true,
		QObject::tr("Capacitance")));
  Props.append(new Property("Q", "100", true,
		QObject::tr("Quality factor")));
  Props.append(new Property("f", "100 MHz", false,
		QObject::tr("Frequency at which Q is measured")));
  Props.append(new Property("Mode", "Linear", false,
		QObject::tr("Q frequency profile")+
		" [Linear, SquareRoot, Constant]"));
  Props.append(new Property("Temp", "26.85", false,
                QObject::tr("simulation temperature in degree Celsius")));
}
CapQ::~CapQ()
{
}

Component* CapQ::newOne()
{
  return new CapQ();
}

Element* CapQ::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Capacitor with Q");
  BitmapFile = (char *) "capq";

  if(getNewOne)  return new CapQ();
  return 0;
}
