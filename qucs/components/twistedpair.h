/***************************************************************************
                                twistedpair.h
                               ---------------
    begin                : Dec 19 2006
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

#ifndef TWISTEDPAIR_H
#define TWISTEDPAIR_H

#include "component.h"


class TwistedPair : public Component  {
public:
  TwistedPair();
 ~TwistedPair();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
};

#endif
