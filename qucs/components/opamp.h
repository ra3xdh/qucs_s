/***************************************************************************
                           opamp.h  -  description
                             -------------------
    begin                : Sun Oct 31 2004
    copyright            : (C) 2004 by Michael Margraf
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

#ifndef OPAMP_H
#define OPAMP_H

#include "component.h"


class OpAmp : public Component  {
public:
  OpAmp();
  ~OpAmp();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
protected:
  QString spice_netlist(bool isXyce);
};

#endif
