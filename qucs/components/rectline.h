/***************************************************************************
                              rectline.h
                             ------------
    begin                : Thu Jun 26 2008
    copyright            : (C) 2008 by Bastien ROUCARIES
    email                : bastien.roucaries@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef RECTLINE_H
#define RECTLINE_H

#include "component.h"


class RectLine : public Component  {
public:
  RectLine();
 ~RectLine();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
};

#endif
