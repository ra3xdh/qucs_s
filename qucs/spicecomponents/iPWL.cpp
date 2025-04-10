/***************************************************************************
                         iPWL.cpp  -  description
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
#include "iPWL.h"
#include "node.h"
#include "extsimkernels/spicecompat.h"


iPWL::iPWL()
{
    Description = QObject::tr("SPICE I(PWL):\nMultiple line ngspice or Xyce I specifications allowed using \"+\" continuation lines.\nLeave continuation lines blank when NOT in use. ");
    Simulator = spicecompat::simSpice;

  // normal voltage source symbol
  Ellipses.append(new qucs::Ellips(-12,-12, 24, 24, QPen(Qt::darkRed,3)));
  Texts.append(new Text(26, 6,"PWL",Qt::darkRed,12.0,0.0,-1.0));
  // pins
  Lines.append(new qucs::Line(-30,  0,-12,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 30,  0, 12,  0,QPen(Qt::darkBlue,2)));
  // arrow
  Lines.append(new qucs::Line( -7,  0,  6,  0,QPen(Qt::darkRed,3, Qt::SolidLine, Qt::FlatCap)));
  Polylines.append(new qucs::Polyline(
    std::vector<QPointF>{{0, -4},{6, 0}, {0, 4}}, QPen(Qt::darkRed, 3, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin)));

  Ports.append(new Port( 30,  0));
  Ports.append(new Port(-30,  0));

  x1 = -30; y1 = -14;
  x2 =  30; y2 =  40;

  tx = x1+4;
  ty = y2+4;
  Model = "iPWL";
  SpiceModel = "I";
  Name  = "I";

  Props.append(new Property("PWL", "", true,"Piece-Wise Linear\nSpecification"));
 Props.append(new Property("Line_2", "", false,"+ continuation line 1"));
 Props.append(new Property("Line_3", "", false,"+ continuation line 2"));
 Props.append(new Property("Line_4", "", false,"+ continuation line 3"));
 Props.append(new Property("Line_5", "", false,"+ continuation line 4"));
 Props.append(new Property("Line_6", "", false,"+ continuation line 5"));
 Props.append(new Property("Line_7", "", false,"+ continuation line 6"));
 Props.append(new Property("Line_8", "", false,"+ continuation line 7"));
 Props.append(new Property("Line_9", "", false,"+ continuation line 8"));
 Props.append(new Property("Line_10", "", false,"+ continuation line 9"));

 rotate();  // fix historical flaw
}

iPWL::~iPWL()
{
}

Component* iPWL::newOne()
{
  return new iPWL();
}

Element* iPWL::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("I(PWL)");
  BitmapFile = (char *) "iPWL";

  if(getNewOne)  return new iPWL();
  return 0;
}

QString iPWL::netlist()
{
    return QString();
}

QString iPWL::spice_netlist(spicecompat::SpiceDialect dialect /* = spicecompat::SPICEDefault */)
{
  Q_UNUSED(dialect);

  QString s = spicecompat::check_refdes(Name,SpiceModel);

  s += " " + spicecompat::normalize_node_name(Ports.at(1)->Connection->Name);
  s += " " + spicecompat::normalize_node_name(Ports.at(0)->Connection->Name);


  QString PWL= Props.at(0)->Value;
  QString Line_2= Props.at(1)->Value;
  QString Line_3= Props.at(2)->Value;
  QString Line_4= Props.at(3)->Value;
  QString Line_5= Props.at(4)->Value;
  QString Line_6= Props.at(5)->Value;
  QString Line_7= Props.at(6)->Value;
  QString Line_8= Props.at(7)->Value;
  QString Line_9= Props.at(8)->Value;
  QString Line_10= Props.at(9)->Value;

  s += " ";

  if(  PWL.length()    > 0)    s += QStringLiteral("%1").arg(PWL);
  if(  Line_2.length() > 0 )   s += QStringLiteral("\n%1").arg(Line_2);
  if(  Line_3.length() > 0 )   s += QStringLiteral("\n%1").arg(Line_3);
  if(  Line_4.length() > 0 )   s += QStringLiteral("\n%1").arg(Line_4);
  if(  Line_5.length() > 0 )   s += QStringLiteral("\n%1").arg(Line_5);
  if(  Line_6.length() > 0 )   s += QStringLiteral("\n%1").arg(Line_6);
  if(  Line_7.length() > 0 )   s += QStringLiteral("\n%1").arg(Line_7);
  if(  Line_8.length() > 0)    s += QStringLiteral("\n%1").arg(Line_8);
  if(  Line_9.length() > 0 )   s += QStringLiteral("\n%1").arg(Line_9);
  if(  Line_10.length() > 0 )  s += QStringLiteral("\n%1").arg(Line_10);
  s += "\n";

  return s;
}
