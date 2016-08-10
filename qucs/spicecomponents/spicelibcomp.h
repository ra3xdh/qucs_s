/***************************************************************************
                               spicelibcomp.h
                              --------------
    begin                : Tue Mar 08 2016
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

#ifndef SPICELIBCOMP_H
#define SPICELIBCOMP_H

#include "components/component.h"


class SpiceLibComp : public MultiViewComponent  {
public:
  SpiceLibComp();
 ~SpiceLibComp() {};
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
  QString getSpiceModel();

protected:
  QString spice_netlist(bool isXyce);
  void createSymbol();
  void remakeSymbol(int No, QStringList &pin_names);
  int  loadSymbol(const QString&);
private:
  void getSymbolPatternsList(QStringList &symbols);
  void removeUnusedPorts();
};

#endif
