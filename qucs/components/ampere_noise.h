/***************************************************************************
                          ampere_noise.h  -  description
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

#ifndef AMPERE_NOISE_H
#define AMPERE_NOISE_H

#include "component.h"


class Ampere_noise : public Component  {
public:
  Ampere_noise();
  ~Ampere_noise();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
protected:
  QString va_code();
};

#endif
