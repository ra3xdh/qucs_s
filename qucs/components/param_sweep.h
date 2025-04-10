/***************************************************************************
                               param_sweep.h
                              ---------------
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

#ifndef PARAM_SWEEP_H
#define PARAM_SWEEP_H

#include "simulation.h"


class Param_Sweep : public qucs::component::SimulationComponent  {
public:
  Param_Sweep();
  ~Param_Sweep();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
  void recreate();

  QString getNgspiceBeforeSim(QString sim, int lvl=0);
  QString getNgspiceAfterSim(QString sim, int lvl=0);
  QString getCounterVar();

protected:
  QString spice_netlist(spicecompat::SpiceDialect dialect = spicecompat::SPICEDefault);
  QString param_split_str=";";
};

#endif
