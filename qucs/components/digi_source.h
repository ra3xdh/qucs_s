/***************************************************************************
                               digi_source.h
                              ---------------
    begin                : Oct 3 2005
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

#ifndef DIGI_SOURCE_H
#define DIGI_SOURCE_H

#include "component.h"


class Digi_Source : public Component  {
public:
  Digi_Source();
 ~Digi_Source();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);

protected:
  QString netlist();
  QString vhdlCode(int);
  QString verilogCode(int);
  QString spice_netlist(bool isXyce = false);
};

#endif
