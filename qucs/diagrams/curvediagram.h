/***************************************************************************
                               curvediagram.h
                              ----------------
    begin                : Sat Apr 9 2005
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

#ifndef CURVEDIAGRAM_H
#define CURVEDIAGRAM_H

#include "diagram.h"


class CurveDiagram : public Diagram  {
public: 
  CurveDiagram(int _cx=0, int _cy=0);
 ~CurveDiagram();


  Diagram* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
  int  calcDiagram();
  void calcLimits();
  void calcCoordinate(const double*, const double*, const double*, float*, float*, Axis const*) const;
  void finishMarkerCoordinates(float&, float&) const;
  bool insideDiagram(float, float) const;

protected:
  void clip(Graph::iterator&) const;
};

#endif
