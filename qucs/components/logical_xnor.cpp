/***************************************************************************
                               logical_xnor.cpp
                              ------------------
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

#include "logical_xnor.h"

Logical_XNOR::Logical_XNOR()
{
  Description = QObject::tr("logical XNOR");
  Model = "XNOR";

  createSymbol();
  tx = x1+4;
  ty = y2+4;
}

Logical_XNOR::~Logical_XNOR()
{
}

Component* Logical_XNOR::newOne()
{
  Logical_XNOR* p = new Logical_XNOR();
  p->Props.getFirst()->Value = Props.getFirst()->Value;
  p->Props.getLast()->Value = Props.getLast()->Value;
  p->recreate(0);
  return p;
}

Element* Logical_XNOR::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("n-port XNOR");
  BitmapFile = (char *) "xnor";

  if(getNewOne)  return new Logical_XNOR();
  return 0;
}
