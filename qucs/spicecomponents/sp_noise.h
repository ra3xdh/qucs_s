/***************************************************************************
                               sp_noise.h
                                ----------
    begin                : Tue May 26 2015
    copyright            : (C) 2015 by Vadim Kuznetsov
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

#ifndef SP_NOISE_H
#define SP_NOISE_H

#include "components/component.h"


class SpiceNoise : public Component  {
public:
  SpiceNoise();
  ~SpiceNoise();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);

protected:
  QString spice_netlist(bool isXyce = false);
};

#endif
