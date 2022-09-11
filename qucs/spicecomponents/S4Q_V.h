/***************************************************************************
                             S4Q_V.h
                              -------------
    begin                : Thu May 21 2015
    copyright            : (C) 2015 by Vadim Kuznetsov
    email                : ra3xdh@gmail.com
             
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

#ifndef S4Q_V_H
#define S4Q_V_H

#include "components/component.h"


class S4Q_V : public Component  {
public:
  S4Q_V();
  ~S4Q_V();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
protected:
  QString spice_netlist(bool isXyce = false);
};

#endif
