/***************************************************************************
  copyright: (C) 2010 by Michael Margraf
 ***************************************************************************/

#ifndef EQN_FILTER_H
#define EQN_FILTER_H

#include "filter.h"

// equaton defined filter
class Equation_Filter : public Filter {
public:
  Equation_Filter();

  static QString* createSchematic(tFilter*);
};

#endif
