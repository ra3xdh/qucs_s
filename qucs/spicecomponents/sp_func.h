/***************************************************************************
                          sp_ic.h  -  description
                             -------------------
    begin                : Mon May 25 2015
    copyright            : (C) 2015 by Vadim Kuznetsov
    email                : ra3xdh@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SP_FUNC_H
#define SP_FUNC_H

#include "components/component.h"


class SpiceFunc : public Component  {

public:
  SpiceFunc();
  ~SpiceFunc();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
  QString getExpression(bool isXyce);

protected:
  QString vhdlCode(int) { return QString(""); }
  QString verilogCode(int) { return QString(""); }
  QString netlist() { return QString(""); }
};

#endif
