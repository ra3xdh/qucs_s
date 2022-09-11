/***************************************************************************
                          iprobe.h  -  description
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

#ifndef IPROBE_H
#define IPROBE_H

#include "component.h"


class iProbe : public Component  {
public:
  iProbe();
  ~iProbe();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
  QString getExpression(bool) { return "";}
  QString getProbeVariable(bool isXyce = false);
protected:
  QString spice_netlist(bool isXyce);
};

#endif
