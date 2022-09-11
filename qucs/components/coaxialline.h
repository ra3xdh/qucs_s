/***************************************************************************
                              coaxialline.h
                             ---------------
    begin                : Sun Jan 29 2006
    copyright            : (C) 2006 by Michael Margraf
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

#ifndef COAXIALLINE_H
#define COAXIALLINE_H

#include "component.h"


class CoaxialLine : public Component  {
public:
  CoaxialLine();
 ~CoaxialLine();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
};

#endif
