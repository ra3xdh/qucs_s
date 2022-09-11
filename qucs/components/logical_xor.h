/***************************************************************************
                               logical_xor.h
                              ---------------
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

#ifndef LOGICAL_XOR_H
#define LOGICAL_XOR_H

#include "component.h"


class Logical_XOR : public GateComponent  {
public:
  Logical_XOR();
 ~Logical_XOR();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
};

#endif
