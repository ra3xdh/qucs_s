/***************************************************************************
                                sparamfile.h
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

#ifndef SPARAMFILE_H
#define SPARAMFILE_H

#include "component.h"


class SParamFile : public MultiViewComponent  {
public:
  SParamFile();
 ~SParamFile() {};
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
  static Element* info1(QString&, char* &, bool getNewOne=false);
  static Element* info2(QString&, char* &, bool getNewOne=false);

  QString getSubcircuitFile();

protected:
  QString netlist();
  void createSymbol();
  QString spice_netlist(bool isXyce);
};

#endif
