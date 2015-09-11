/***************************************************************************
                          S4Q_V_DC.h  -  description
                             -------------------
    begin                : Sat Aug 23 2003
    copyright            : (C) 2003 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
         
    SPICE Version         : Friday Sept 11 2015
    copyright            : (C) 2015 Mike Brinson
    email                : mbrin72043@yahoo.co.uk
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef S4Q_V_DC_H
#define S4Q_V_DC_H

#include "component.h"


class S4Q_V_DC : public Component  {
public:
  S4Q_V_DC();
  ~S4Q_V_DC();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
  QString spice_netlist(bool isXyce = false);
};

#endif
