/***************************************************************************
                        amplifier.h  -  description
                             -------------------
    begin                : Sat Oct 30 2004
    copyright            : (C) 2004 by Michael Margraf
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

#ifndef AMPLIFIER_H
#define AMPLIFIER_H

#include "component.h"


class Amplifier : public Component  {
public:
  Amplifier();
  ~Amplifier();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
};

#endif
