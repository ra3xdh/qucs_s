/***************************************************************************
                         ipulse.h  -  description
                             -------------------
    begin                : Sat Sep 18 2004
    copyright            : (C) 2004 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
    spice4qucs code added  Thurs. 19 March 2015
    copyright          : (C) 2015 by Vadim Kusnetsov (Vadim Kuznetsov (ra3xdh@gmail.com) 
                                           and Mike Brinson (mbrin72043@yahoo.co.uk
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef IPULSE_H
#define IPULSE_H

#include "component.h"


class iPulse : public Component  {
public:
  iPulse();
  ~iPulse();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
   QString spice_netlist(bool isXyce = false);
};

#endif
