/***************************************************************************
                                 etr_sim.h
                                -----------
    begin                : Wed Dec 04 2013
    copyright            : (C) 2013 by Richard Crozier
    email                : richard.crozier@yahoo.co.uk
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ETR_SIM_H
#define ETR_SIM_H

#include "component.h"


class ETR_Sim : public Component  {
public:
  ETR_Sim();
  ~ETR_Sim();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
//  void recreate(Schematic*);
};

#endif
