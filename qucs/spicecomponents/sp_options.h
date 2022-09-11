/***************************************************************************
                          equation.h  -  description
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

#ifndef SP_OPTIONS_H
#define SP_OPTIONS_H

#include "components/component.h"


class SpiceOptions : public Component  {

public:
  SpiceOptions();
  ~SpiceOptions();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
  static void splitEqn(QString &eqn, QStringList &tokens);
  QString getExpression(bool isXyce);

protected:
  QString vhdlCode(int) { return QString(""); }
  QString verilogCode(int) { return QString(""); }
  QString netlist() { return QString(""); }
};

#endif
