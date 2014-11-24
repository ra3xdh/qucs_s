/***************************************************************************
    copyright            : (C) 2010 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
 ***************************************************************************/

#ifndef MUTUALX_H
#define MUTUALX_H

#include "component.h"


class MutualN : public MultiViewComponent  {
public:
  MutualN();
  static Element* info(QString&, QByteArray*, bool getNewOne=false);
  static char component_icon[230];

protected:
  void createSymbol(Schematic*);
};

#endif
