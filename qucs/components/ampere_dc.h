/***************************************************************************
                          ampere_dc.h  -  description
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

#ifndef AMPERE_DC_H
#define AMPERE_DC_H

#include "component.h"


class Ampere_dc : public Component  {
public:
  Ampere_dc();
  ~Ampere_dc();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
   QString spice_netlist(bool isXyce = false);
};

#endif
