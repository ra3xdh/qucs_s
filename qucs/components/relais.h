/***************************************************************************
                                 relais.h
                                ----------
    begin                : Sat Feb 25 2006
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

#ifndef RELAIS_H
#define RELAIS_H

#include "component.h"


class Relais : public Component  {
public:
  Relais();
 ~Relais();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
  QString spice_netlist();
};

#endif
