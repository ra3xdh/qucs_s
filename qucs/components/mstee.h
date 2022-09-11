/***************************************************************************
                                mstee.h
                               ---------
    begin                : Sat Aug 23 2003
    copyright            : (C) 2003 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MSTEE_H
#define MSTEE_H

#include "component.h"


class MStee : public MultiViewComponent  {
public:
  MStee();
 ~MStee();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);

protected:
  void createSymbol();
};

#endif
