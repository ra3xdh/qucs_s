/***************************************************************************
                          circulator.h  -  description
                             -------------------
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

#ifndef CIRCULATOR_H
#define CIRCULATOR_H

#include "component.h"


class Circulator : public Component  {
public:
  Circulator();
  ~Circulator();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
};

#endif
