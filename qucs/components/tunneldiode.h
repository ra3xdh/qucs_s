/***************************************************************************
    copyright            : (C) 2008 by Michael Margraf
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

#ifndef TUNNELDIODE_H
#define TUNNELDIODE_H

#include "component.h"

class TunnelDiode : public Component  {
public:
  TunnelDiode();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
};

#endif
