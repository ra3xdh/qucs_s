/***************************************************************************
                               logical_or.cpp
                              ----------------
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

#include "logical_or.h"

Logical_OR::Logical_OR()
{
  Description = QObject::tr("logical OR");
  Model = "OR";
  SpiceModel = "A";

  createSymbol();
  tx = x1+4;
  ty = y2+4;
}

Logical_OR::~Logical_OR()
{
}

Component* Logical_OR::newOne()
{
  Logical_OR* p = new Logical_OR();
  p->Props.getFirst()->Value = Props.getFirst()->Value;
  p->Props.getLast()->Value = Props.getLast()->Value;
  p->recreate(0);
  return p;
}

Element* Logical_OR::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("n-port OR");
  BitmapFile = (char *) "or";

  if(getNewOne)  return new Logical_OR();
  return 0;
}
