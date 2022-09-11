/***************************************************************************
                               xyce_script.h
                                ----------
    begin                : Wed Jun 22 2016
    copyright            : (C) 2016 by Vadim Kuznetsov
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

#ifndef SP_INCLSCRIPT_H
#define SP_INCLSCRIPT_H

#include "components/component.h"


class InclScript : public Component  {
public:
  InclScript();
  ~InclScript();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
  QString getExpression(bool isXyce);

protected:
  QString vhdlCode(int) { return QString(""); }
  QString verilogCode(int) { return QString(""); }
  QString netlist() { return QString(""); }
};

#endif
