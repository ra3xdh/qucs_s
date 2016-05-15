/***************************************************************************
                             spicegeneric.h
                             ---------------
    begin                : Mon Mar 07 2016
    copyright            : (C) 2016 by Vadim Kuznetsov
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

#ifndef SPICEGENERIC_H
#define SPICEGENERIC_H
#include "components/component.h"


class QProcess;
class QTextStream;
class QString;

class SpiceGeneric : public MultiViewComponent  {
public:
  SpiceGeneric();
 ~SpiceGeneric() {};
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);

private:
  bool changed;

protected:
  QString netlist();
  void createSymbol();
  QString spice_netlist(bool isXyce);

};

#endif
