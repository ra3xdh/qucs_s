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

#ifndef XSP_CODEMODEL_H
#define XSP_CODEMODEL_H

#include "components/component.h"


class XSP_CodeModel : public Component  {

public:
  XSP_CodeModel();
  ~XSP_CodeModel();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);

protected:
  QString spice_netlist();

};

#endif
