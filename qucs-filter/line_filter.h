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

#ifndef LINE_FILTER_H
#define LINE_FILTER_H

#include "tl_filter.h"

// capacitive end-coupled, half-wavelength bandpass filter
class Line_Filter : public TL_Filter {
public:
  Line_Filter();

  static QString* createSchematic(tFilter*, tSubstrate*, bool);
};

#endif
