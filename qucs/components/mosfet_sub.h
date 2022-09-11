/***************************************************************************
                               mosfet_sub.h
                              --------------
    begin                : Fri Jun 4 2004
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

#ifndef MOSFET_SUB_H
#define MOSFET_SUB_H

#include "component.h"


class Basic_MOSFET : public MultiViewComponent {
public:
  Basic_MOSFET();
 ~Basic_MOSFET() {};
};

class MOSFET_sub : public Basic_MOSFET {
public:
  MOSFET_sub();
 ~MOSFET_sub() {};
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
  static Element* info_p(QString&, char* &, bool getNewOne=false);
  static Element* info_depl(QString&, char* &, bool getNewOne=false);

protected:
  void createSymbol();
  QString spice_netlist(bool isXyce = false);
};

#endif
