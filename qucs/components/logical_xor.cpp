/***************************************************************************
                               logical_xor.cpp
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

#include "logical_xor.h"

Logical_XOR::Logical_XOR()
{
  Description = QObject::tr("logical XOR");
  Model = "XOR";
  SpiceModel = "A";

  createSymbol();
  tx = x1+4;
  ty = y2+4;
}

Logical_XOR::~Logical_XOR()
{
}

Component* Logical_XOR::newOne()
{
  Logical_XOR* p = new Logical_XOR();
  p->Props.getFirst()->Value = Props.getFirst()->Value;
  p->Props.getLast()->Value = Props.getLast()->Value;
  p->recreate(0);
  return p;
}

Element* Logical_XOR::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("n-port XOR");
  BitmapFile = (char *) "xor";

  if(getNewOne)  return new Logical_XOR();
  return 0;
}
