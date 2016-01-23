/***************************************************************************
                          sp_include.h  -  description
                             -------------------
    begin                : Mon Dec 07 2015
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

#ifndef XSP_CMLIB_H
#define XSP_CMLIB_H

#include "components/component.h"


class XSP_CMlib : public Component  {

public:
  XSP_CMlib();
  ~XSP_CMlib();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);

  QString getSpiceInit();

protected:
  QString spice_netlist();

};

#endif
