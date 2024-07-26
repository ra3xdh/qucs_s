
#ifndef CONDUCTOR_H
#define CONDUCTOR_H
#include "element.h"

class WireLabel;

/** \class Conductor
  * \brief label for Node and Wire classes
  *
  */
class Conductor : public Element {
public:
  WireLabel *Label;
};

#endif