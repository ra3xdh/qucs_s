/***************************************************************************
                               d_flipflop.h
                              --------------
    begin                : Fri Jan 06 2006
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

#ifndef D_FLIPFLOP_H
#define D_FLIPFLOP_H

#include "component.h"


class D_FlipFlop : public Component  {
public:
  D_FlipFlop();
 ~D_FlipFlop() {};
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);

protected:
  QString vhdlCode(int);
  QString verilogCode(int);
};

#endif
