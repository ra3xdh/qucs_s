/***************************************************************************
    copyright            : (C) 2010 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
 ***************************************************************************/

#ifndef MUTUALX_H
#define MUTUALX_H

#include "component.h"


class MutualX : public MultiViewComponent  {
public:
  MutualX();
  ~MutualX() {};
  Component* newOne();
  static Element* info(QString& Name, char* &BitmapFile, bool getNewOne);

protected:
  void createSymbol();
  QString netlist();
};

#endif
