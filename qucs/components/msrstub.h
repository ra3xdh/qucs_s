/***************************************************************************
                       msrstub.h  -  description
                      ---------------------------
    begin                : Sun Mar 15 2009
    copyright            : (C) 2009 by Stefan Jahn
    email                : stefan@lkcc.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MSRSTUB_H
#define MSRSTUB_H

#include "component.h"

class MSrstub : public Component  {
public:
  MSrstub();
  ~MSrstub();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
};

#endif
