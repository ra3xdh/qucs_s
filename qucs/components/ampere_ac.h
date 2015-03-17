/***************************************************************************
                          ampere_ac.h  -  description
                             -------------------
    begin                : Sun May 23 2004
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

#ifndef AMPERE_AC_H
#define AMPERE_AC_H

#include "component.h"


class Ampere_ac : public Component  {
public:
  Ampere_ac();
  ~Ampere_ac();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
protected:
  QString spice_netlist(bool isXyce);
};

#endif
