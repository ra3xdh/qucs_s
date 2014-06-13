/***************************************************************************
  copyright: (C) 2010 by Michael Margraf
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
