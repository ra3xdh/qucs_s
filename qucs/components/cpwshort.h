/***************************************************************************
                                cpwshort.h
                              --------------
    begin                : Sun May 01 2005
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

#ifndef CPWSHORT_H
#define CPWSHORT_H

#include "component.h"


class CPWshort : public Component  {
public:
  CPWshort();
  ~CPWshort();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
};

#endif
