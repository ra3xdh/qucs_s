/***************************************************************************
                               logical_and.h
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

#ifndef LOGICAL_AND_H
#define LOGICAL_AND_H

#include "component.h"


class Logical_AND : public GateComponent  {
public:
  Logical_AND();
 ~Logical_AND();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
};

#endif
