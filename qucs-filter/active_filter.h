/***************************************************************************
  copyright: (C) 2010 by Michael Margraf
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
