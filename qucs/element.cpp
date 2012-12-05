/***************************************************************************
                          element.cpp  -  description
                             -------------------
    begin                : Sat Sep 20 2003
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

#include "element.h"
//#include "schematic.h"
Element::Element()
{
  Type = isDummy;
  isSelected = false;
  cx = cy = x1 = y1 = x2 = y2 = 0;
}

Element::~Element()
{
}

void Element::paintScheme(Schematic *)
{
}

void Element::setCenter(int, int, bool)
{
}

void Element::getCenter(int&, int&)
{
}
