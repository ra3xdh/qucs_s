/***************************************************************************
                               logical_nand.cpp
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

#include "logical_nand.h"

Logical_NAND::Logical_NAND()
{
  Description = QObject::tr("logical NAND");
  Model = "NAND";

  createSymbol();
  tx = x1+4;
  ty = y2+4;
}

Logical_NAND::~Logical_NAND()
{
}

Component* Logical_NAND::newOne()
{
  Logical_NAND* p = new Logical_NAND();
  p->Props.getFirst()->Value = Props.getFirst()->Value;
  p->Props.getLast()->Value = Props.getLast()->Value;
  p->recreate(0);
  return p;
}

Element* Logical_NAND::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("n-port NAND");
  BitmapFile = (char *) "nand";

  if(getNewOne)  return new Logical_NAND();
  return 0;
}
