/***************************************************************************
                               logical_nor.cpp
                              -----------------
    begin                : Sun Sep 25 2005
    copyright            : (C) 2005 by Michael Margraf
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

#include "logical_nor.h"

Logical_NOR::Logical_NOR()
{
  Description = QObject::tr("logical NOR");
  Model = "NOR";
  SpiceModel = "A";

  createSymbol();
  tx = x1+4;
  ty = y2+4;
}

Logical_NOR::~Logical_NOR()
{
}

Component* Logical_NOR::newOne()
{
  Logical_NOR* p = new Logical_NOR();
  p->Props.front()->Value = Props.front()->Value;
  p->Props.back()->Value = Props.back()->Value;
  p->recreate();
  return p;
}

Element* Logical_NOR::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("n-port NOR");
  BitmapFile = (char *) "nor";

  if(getNewOne)  return new Logical_NOR();
  return 0;
}
