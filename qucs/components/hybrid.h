/***************************************************************************
    copyright            : (C) 2010 by Michael Margraf
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

#ifndef HYBRID_H
#define HYBRID_H

#include "component.h"

class Hybrid : public Component  {
public:
  Hybrid();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
};

#endif
