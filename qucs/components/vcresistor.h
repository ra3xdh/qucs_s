/***************************************************************************
                         vcresistor.h  -  description
                             -------------------
    begin                : Feb 13 2014
    copyright            : (C) 2014 Richard Crozier
    email                : richard dot crozier at yahoo.co.uk
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef VCRESISTOR_H
#define VCRESISTOR_H

#include "component.h"


class vcresistor : public Component  {
public:
  vcresistor();
  ~vcresistor();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
};

#endif
