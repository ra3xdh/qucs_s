/***************************************************************************
                                 mosfet.h
                                ----------
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

#ifndef MOSFET_H
#define MOSFET_H

#include "mosfet_sub.h"


class MOSFET : public Basic_MOSFET {
public:
  MOSFET();
 ~MOSFET() {};
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
  static Element* info_p(QString&, char* &, bool getNewOne=false);
  static Element* info_depl(QString&, char* &, bool getNewOne=false);

protected:
  void createSymbol();
  QString netlist();
  QString spice_netlist(bool isXyce = false);
};

#endif
