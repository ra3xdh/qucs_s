/***************************************************************************
                              truthdiagram.h
                             ----------------
    begin                : Sat Nov 12 2005
    copyright            : (C) 2005 by Michael Margraf
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

#ifndef TRUTHDIAGRAM_H
#define TRUTHDIAGRAM_H

#include "tabdiagram.h"


class TruthDiagram : public TabDiagram  {
public: 
  TruthDiagram(int _cx=0, int _cy=0);
 ~TruthDiagram();

  Diagram* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
  int calcDiagram();
};

#endif
