/***************************************************************************
                          sp_nutmeg.cpp  -  description
                             -------------------
    begin                : Fri May 22 2015
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

#ifndef SP_NUTMEG_H
#define SP_NUTMEG_H

#include "component.h"


class NutmegEquation : public Component  {

public:
  NutmegEquation();
  ~NutmegEquation();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
  QString getEquations(QString sim, QStringList &dep_vars);

protected:
  QString vhdlCode(int) { return QString(""); }
  QString verilogCode(int) { return QString(""); }
  QString netlist() { return QString(""); }
};

#endif
