/***************************************************************************
                                 XspiceGeneric.h
                                -------------
    begin                : Tue Dec 28 2004
    copyright            : (C) 2004 by Michael Margraf
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

#ifndef XSPICEGENERIC_H
#define XSPICEGENERIC_H
#include "components/component.h"


class QProcess;
class QTextStream;
class QString;

class XspiceGeneric : public MultiViewComponent  {
public:
  XspiceGeneric();
 ~XspiceGeneric() {};
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
