/***************************************************************************
  copyright: (C) 2010 by Michael Margraf
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef LC_FILTER_H
#define LC_FILTER_H

#include "filter.h"

// ladder filter containing inductors L and capacitors C
class LC_Filter : public Filter {
public:
  LC_Filter();

  static QString* createSchematic(tFilter*, bool);
};

#endif
