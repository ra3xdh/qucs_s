/***************************************************************************
  copyright: (C) 2010 by Michael Margraf
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
