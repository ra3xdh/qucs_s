/***************************************************************************
                                 mutual2.h
                                -----------
    begin                : Sat Aug 20 2005
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

#ifndef MUTUAL2_H
#define MUTUAL2_H

#include "component.h"


class Mutual2 : public Component  {
public:
  Mutual2();
 ~Mutual2();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
protected:
  QString spice_netlist(bool isXyce);
};

#endif
