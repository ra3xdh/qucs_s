/***************************************************************************
  copyright: (C) 2010 by Michael Margraf
 ***************************************************************************/

#ifndef STEPZ_FILTER_H
#define STEPZ_FILTER_H

#include "tl_filter.h"

// stepped-impedance transmission line filter
class StepImpedance_Filter : public TL_Filter {
public:
  StepImpedance_Filter();

  static QString* createSchematic(tFilter*, tSubstrate*, bool);
};

#endif
