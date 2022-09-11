/***************************************************************************
                               opt_sim.h
                              -----------
    begin                : Sat Jul 22 2006
    copyright            : (C) 2006 by Michael Margraf
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

#ifndef OPT_SIM_H
#define OPT_SIM_H

#include "component.h"


class Optimize_Sim : public Component  {
public:
  Optimize_Sim();
 ~Optimize_Sim();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
  bool createASCOFiles();
  bool createASCOnetlist();
  bool loadASCOout();

protected:
  QString netlist();
};

#endif
