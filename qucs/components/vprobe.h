/***************************************************************************
                                vprobe.h
                               ----------
    begin                : Sat Feb 18 2006
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

#ifndef VPROBE_H
#define VPROBE_H

#include "component.h"


class vProbe : public Component  {
public:
  vProbe();
 ~vProbe();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
  QString getProbeVariable(bool isXyce = false);
protected:
  QString spice_netlist(bool isXyce);
};

#endif
