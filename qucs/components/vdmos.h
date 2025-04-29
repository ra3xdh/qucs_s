/***************************************************************************
                                  jfet.h
                                 --------
    begin                : Fri Jun 4 2004
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

#ifndef VDMOS_H
#define VDMOS_H

#include "component.h"


class VDMOS : public MultiViewComponent {
public:
  VDMOS();
	 ~VDMOS() {};
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
  static Element* info_p(QString&, char* &, bool getNewOne=false);

protected:
  void createSymbol();
  QString spice_netlist(spicecompat::SpiceDialect dialect = spicecompat::SPICEDefault);
};

#endif
