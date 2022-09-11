/***************************************************************************
                               coplanar.h
                              ------------
    begin                : Fri May 13 2005
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

#ifndef CPWSTEP_H
#define CPWSTEP_H

#include "component.h"


class CPWstep : public Component  {
public:
  CPWstep();
  ~CPWstep();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
};

#endif
