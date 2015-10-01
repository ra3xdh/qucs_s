/***************************************************************************
                          sp_model.h  -  description
                             -------------------
    begin                : Thu Oct 01 2015
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

#ifndef SP_MODEL_H
#define SP_MODEL_H

#include "component.h"


class S4Q_Model : public Component  {

public:
  S4Q_Model();
  ~S4Q_Model();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
  QString getSpiceModel();

protected:
  QString vhdlCode(int) { return QString(""); }
  QString verilogCode(int) { return QString(""); }
  QString netlist() { return QString(""); }
};

#endif
