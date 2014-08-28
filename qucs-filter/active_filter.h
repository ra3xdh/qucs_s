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

#ifndef ACTIVE_FILTER_H
#define ACTIVE_FILTER_H

#include "filter.h"

// active RC filter using operational amplifiers
class Active_Filter : public Filter {
public:
  Active_Filter();

  static QString* createSchematic(tFilter*, double);
};

#endif
