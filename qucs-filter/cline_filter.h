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

#ifndef CLINE_FILTER_H
#define CLINE_FILTER_H

#include "tl_filter.h"

// coupled transmission line filter
class CoupledLine_Filter : public TL_Filter {
public:
  CoupledLine_Filter();

  static QString* createSchematic(tFilter*, tSubstrate*, bool);
};

#endif
