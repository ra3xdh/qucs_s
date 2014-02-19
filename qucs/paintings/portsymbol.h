/***************************************************************************
                               portsymbol.h
                              --------------
    begin                : Sun Sep 5 2004
    copyright            : (C) 2004 by Michael Margraf
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

#ifndef PORTSYMBOL_H
#define PORTSYMBOL_H

#include "painting.h"


class PortSymbol : public Painting  {
public:
  PortSymbol(int cx_=0, int cy_=0, const QString& numberStr_="1",
                                   const QString& nameStr_="");
 ~PortSymbol();

  void paintScheme(Schematic*);
  void getCenter(int&, int&);
  void setCenter(int, int, bool relative=false);

  bool load(const QString&);
  QString save();
  QString saveCpp();
  QString saveJSON();
  void paint(ViewPainter*);
  bool getSelected(float, float, float);
  void Bounding(int&, int&, int&, int&);

  void rotate();
  void mirrorX();
  void mirrorY();

  int Angel;
  QString numberStr, nameStr;
};

#endif
