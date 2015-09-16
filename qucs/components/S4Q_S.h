/***************************************************************************
                             S4Q_S.h
                              -------------
    begin                : Thu May 21 2015
    copyright            : (C) 2015 by Vadim Kuznetsov
    email                : ra3xdh@gmail.com
             
    SPICE Version         : Wednesday Sept 16 2015
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

#ifndef S4Q_S_H
#define S4Q_S_H

#include "component.h"


class S4Q_S : public Component  {
public:
  S4Q_S();
  ~S4Q_S();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
protected:
  QString spice_netlist(bool isXyce = false);
};

#endif
