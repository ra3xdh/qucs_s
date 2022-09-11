/***************************************************************************
                               thyristor.h
                              -------------
    begin                : Sun Dec 23 2007
    copyright            : (C) 2007 by Michael Margraf
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

#ifndef THYRISTOR_H
#define THYRISTOR_H

#include "component.h"


class Thyristor : public Component  {
public:
  Thyristor();
 ~Thyristor() {};
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
};

#endif
