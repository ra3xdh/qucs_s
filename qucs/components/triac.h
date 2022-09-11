/***************************************************************************
                                 triac.h
                                ---------
    begin                : Sun Dec 23 2007
    copyright            : (C) 2007 by Michael Margraf
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

#ifndef TRIAC_H
#define TRIAC_H

#include "component.h"


class Triac : public Component  {
public:
  Triac();
 ~Triac() {};
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
};

#endif
