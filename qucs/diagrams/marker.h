/***************************************************************************
                                marker.h
                               ----------
    begin                : Sat Apr 10 2004
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

#ifndef MARKER_H
#define MARKER_H

#include "element.h"
#include "viewpainter.h"

class QPainter;
class Diagram;
class Graph;

typedef enum{
	nM_Rect = 0,
	nM_Deg,
	nM_Rad
} numMode_t;


class Marker : public Element {
public:
  Marker(Graph *pg_=0, int _nn=0, int cx_=0, int cy_=0);
 ~Marker();

private:
  void    initText(int);
public:
  void    createText();
  void    makeInvalid();
  bool    moveLeftRight(bool);
  bool    moveUpDown(bool);
  void    paint(ViewPainter*, int, int);
  void    paintScheme(QPainter*);
  void    setCenter(int, int, bool);
  void    Bounding(int& _x1, int& _y1, int& _x2, int& _y2);
  QString save();
  bool    load(const QString& Line);
  bool    getSelected(int, int);
  Marker* sameNewOne(Graph*);
  void    getTextSize();
  Graph const* graph() const {return pGraph;}
  int precision() const {return Precision;}
  std::vector<double> const& varPos() const {return VarPos;}
  const Diagram *diag() const;
public: // power matching stuff. some sort of VarPos (ab?)use
  double  powFreq() const {return VarPos[0];}
  double  powReal() const {return VarDep[0];}
  double  powImag() const {return VarDep[1];}

// private: // not yet
  Graph const *pGraph;   // the corresponding graph

private:
  std::vector<double> VarPos;   // values the marker is pointing to
  double VarDep[2];   // dependent value
  float  fCX, fCY;  // coordinates for the line from graph to marker body

public:
  QString Text;     // the string to be displayed in the marker text
  bool transparent; // background shines through marker body

// private: // not yet, cross-manipulated by MarkerDialog
  int Precision; // number of digits to show
  int numMode;   // real/imag or polar (deg/rad)

public: // shouldn't be there, cross-manipulated by MarkerDialog
        // to be implemented within SmithDiagram.
	double Z0;		//Only used in smith chart marker, to convert S to Z
};

#endif
