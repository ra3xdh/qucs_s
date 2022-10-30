/***************************************************************************
                               logical_buf.h
                              ---------------
    begin                : Sat Nov 8 2008
    copyright            : (C) 2008 by Stefan Jahn
    email                : stefan@lkcc.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef LOGICAL_BUF_H
#define LOGICAL_BUF_H

#include "component.h"


class Logical_Buf : public MultiViewComponent  {
public:
  Logical_Buf();
 ~Logical_Buf() {};
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);

protected:
  QString vhdlCode(int);
  QString verilogCode(int);
  QString spice_netlist(bool isXyce);
  void createSymbol();
};

#endif
