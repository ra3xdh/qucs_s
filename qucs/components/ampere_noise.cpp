/***************************************************************************
                              ampere_noise.cpp
                             ------------------
    begin                : Sat Aug 23 2003
    copyright            : (C) 2003 by Michael Margraf
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

#include "ampere_noise.h"
#include "node.h"
#include "extsimkernels/verilogawriter.h"
#include "extsimkernels/spicecompat.h"


Ampere_noise::Ampere_noise()
{
  Description = QObject::tr("noise current source");
  Simulator = spicecompat::simQucsator;

  Arcs.append(new qucs::Arc(-12,-12, 24, 24,  0, 16*360,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-30,  0,-12,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 30,  0, 12,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( -7,  0,  7,  0,QPen(Qt::darkBlue,3)));
  Lines.append(new qucs::Line(  6,  0,  0, -4,QPen(Qt::darkBlue,3)));
  Lines.append(new qucs::Line(  6,  0,  0,  4,QPen(Qt::darkBlue,3)));

  Lines.append(new qucs::Line(-12,  1,  1,-12,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-10,  6, -7,  3,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(  3, -7,  6,-10,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( -7, 10, -2,  5,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(  7, -4, 10, -7,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( -1, 12, 12, -1,QPen(Qt::darkBlue,2)));

  Ports.append(new Port( 30,  0));
  Ports.append(new Port(-30,  0));

  x1 = -30; y1 = -15;
  x2 =  30; y2 =  15;

  tx = x1+4;
  ty = y2+4;
  Model = "Inoise";
  Name  = "I";

  Props.append(new Property("i", "1e-6", true,
		QObject::tr("current power spectral density in A^2/Hz")));
  Props.append(new Property("e", "0", false,
		QObject::tr("frequency exponent")));
  Props.append(new Property("c", "1", false,
		QObject::tr("frequency coefficient")));
  Props.append(new Property("a", "0", false,
		QObject::tr("additive frequency term")));

  rotate();  // fix historical flaw
}

Ampere_noise::~Ampere_noise()
{
}

Component* Ampere_noise::newOne()
{
  return new Ampere_noise();
}

Element* Ampere_noise::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Noise Current Source");
  BitmapFile = (char *) "noise_current";

  if(getNewOne)  return new Ampere_noise();
  return 0;
}

QString Ampere_noise::va_code()
{
    QString u = vacompat::normalize_value(getProperty("i")->Value);
    QString e = vacompat::normalize_value(getProperty("e")->Value);
    QString c = vacompat::normalize_value(getProperty("c")->Value);
    QString a = vacompat::normalize_value(getProperty("a")->Value);
    
    QString plus =  Ports.at(0)->Connection->Name;
    QString minus = Ports.at(1)->Connection->Name; 
    QString s = "";
    QString Ipm = vacompat::normalize_current(plus,minus,true); 

    if ( e == "0" ) 
    {
       s += QString("%1 <+ white_noise(%2,\"shot\" );\n").arg(Ipm).arg(u);
    }
    else 
    {
	  s += QString("%1 <+ flicker_noise(%2, %3, \"flicker\" );\n" ).arg(Ipm).arg(u).arg(e);
    }

    return s;
}
