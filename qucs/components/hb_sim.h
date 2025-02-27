/***************************************************************************
                          hb_sim.h  -  description
                             -------------------
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

#ifndef HB_SIM_H
#define HB_SIM_H

#include "simulation.h"


class HB_Sim : public qucs::component::SimulationComponent  {
public:
  HB_Sim();
  ~HB_Sim();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
protected:
  QString spice_netlist(spicecompat::SpiceDialect dialect = spicecompat::SPICEDefault);
};

#endif
