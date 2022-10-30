/***************************************************************************
                               logical_inv.h
                              ---------------
    begin                : Wed Sep 28 2005
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

#ifndef LOGICAL_INV_H
#define LOGICAL_INV_H

#include "component.h"


class Logical_Inv : public MultiViewComponent  {
public:
  Logical_Inv();
 ~Logical_Inv() {};
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);

protected:
  QString spice_netlist(bool isXyce);
  QString vhdlCode(int);
  QString verilogCode(int);
  void createSymbol();
};

#endif
