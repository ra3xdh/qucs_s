/***************************************************************************
                                 mutualx.cpp
                                -----------
    begin                : Mon Nov 24 2014
    copyright            : (C) 2010 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
    adapted for Qucs     : Vadim Kuznetsov
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


#ifndef MUTUALX_H
#define MUTUALX_H

#include "component.h"


class MutualX : public MultiViewComponent  {
public:
  MutualX();
  ~MutualX() {}

  Component* newOne();
  static Element* info(QString& Name, char* &BitmapFile, bool getNewOne);

protected:
  void createSymbol();
  QString netlist();
  QString spice_netlist(bool isXyce);
};

#endif
