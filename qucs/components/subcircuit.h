/***************************************************************************
                               subcircuit.h
                              --------------
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

#ifndef SUBCIRCUIT_H
#define SUBCIRCUIT_H

#include "component.h"


class Subcircuit : public MultiViewComponent  {
public:
  Subcircuit();
 ~Subcircuit() {};
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);

  QString getSubcircuitFile();

protected:
  QString netlist();
  QString spice_netlist(bool isXyce);
  QString vhdlCode(int);
  QString verilogCode(int);
  void createSymbol();
  void remakeSymbol(int No);
  int  loadSymbol(const QString&);
};

#endif
